#include "../cmn/packageExt.hpp"
#include "../cmn/win32.hpp"
#include "../console/log.hpp"
#include "../envvar/api.hpp"
#include "../exec/api.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../store/api.hpp"
#include "../tcatlib/api.hpp"
#include "directory.hpp"
#include "instr.hpp"
#include "recipes.hpp"

namespace curator {

void recipeVisitorBase::visit(compositeRecipe& n)
{
   for(auto it=n.children.begin();it!=n.children.end();++it)
      (*it)->acceptVisitor(*this);
}

compositeRecipe::~compositeRecipe()
{
   for(auto it=children.begin();it!=children.end();++it)
      (*it)->release();
}

void compositeRecipe::execute()
{
   for(auto it=children.begin();it!=children.end();++it)
      (*it)->execute();
}

void fetchRecipe::execute()
{
   auto packageFullName = cmn::buildPackageFullName(m_package);
   m_d.log().writeLn("running fetch %s",packageFullName.c_str());
   m_d.store().populatePackage(packageFullName.c_str());
}

void unfetchRecipe::execute()
{
   auto packageFullName = cmn::buildPackageFullName(m_package);
   m_d.log().writeLn("running unfetch %s",packageFullName.c_str());
   m_d.store().depopulatePackage(packageFullName.c_str());
}

inflatableRecipe::~inflatableRecipe()
{
   for(auto it=children.begin();it!=children.end();++it)
      (*it)->release();
}

void inflatableRecipe::execute()
{
   for(auto it=children.begin();it!=children.end();++it)
      (*it)->execute();
}

void installRecipe::execute()
{
   auto& name = m_package["name"].as<sst::str>().get();
   auto vers = m_package["version"].as<sst::mint>().get();

   m_d.log().writeLn("running install %s:%d",name.c_str(),vers);

   // run the installer
   inflatableRecipe::execute();

   // remove old versions
   do
   {
      auto& candidates = m_d.config()["installed"].as<sst::array>();
      size_t iPackage=0;
      for(;iPackage!=candidates.size();iPackage++)
      {
         auto& d = candidates[iPackage].as<sst::dict>();
         if(d["name"].as<sst::str>().get() == name)
            break;
      }
      if(iPackage != candidates.size())
      {
         candidates.erase(iPackage);
         continue;
      }
   }
   while(false);

   // mark the package installed
   auto& package = m_d.config()["installed"].as<sst::array>()
      .append<sst::dict>();
   package.add<sst::str>("name") = name;
   package.add<sst::mint>("version") = vers;
}

void installRecipe::inflate()
{
   instrBuilder b(m_d,children);
   b.populate(m_package,true);
}

void uninstallRecipe::execute()
{
   auto& name = m_package["name"].as<sst::str>().get();
   auto vers = m_package["version"].as<sst::mint>().get();

   m_d.log().writeLn("running uninstall %s:%d",name.c_str(),vers);

   // run the uninstaller
   inflatableRecipe::execute();

   auto& candidates = m_d.config()["installed"].as<sst::array>();
   size_t iPackage=0;
   for(;iPackage!=candidates.size();iPackage++)
   {
      auto& d = candidates[iPackage].as<sst::dict>();
      if(
         d["name"].as<sst::str>().get() == name &&
         d["version"].as<sst::mint>().get() == vers
      )
         break;
   }
   if(iPackage == candidates.size())
      throw std::runtime_error("internal software error");

   // mark the package uninstalled
   candidates.erase(iPackage);
}

void uninstallRecipe::inflate()
{
   instrBuilder b(m_d,children);
   b.populate(m_package,false);
}

void scheduleUninstallRecipe::execute()
{
   auto& name = m_package["name"].as<sst::str>().get();
   auto vers = m_package["version"].as<sst::mint>().get();

   auto& package = m_d.config()["deferred-delete"].as<sst::array>()
      .append<sst::dict>();
   package.add<sst::str>("name") = name;
   package.add<sst::mint>("version") = vers;
}

void upgradeCallingRecipe::execute()
{
   m_d.log().writeLn("propagate install call to future gordian");

   DWORD pid = ::GetCurrentProcessId();
   std::stringstream evtName;
   evtName << "_gordianUpgrade_" << pid;
   cmn::osEvent evt(evtName.str());

   m_d.log().writeLn("first flush any open files");
   tcat::typePtr<file::iFileManager> pFm;
   pFm->flushAllOpen();

   std::stringstream command;
   command
      << "\"" << getCalleePath()
      << "\\gordian.exe" << "\""
      << " --_upgrade"
      << " " << pid
      << " " << m_package["version"].as<sst::mint>().get()
      << " " << getExtraArg()
   ;
   m_d.log().writeLn(command.str());

   tcat::typePtr<exec::iProcessRunner> pExec;
   pExec->execute(command.str().c_str(),m_d.log(),false);

   m_d.log().writeLn("waiting for future acknowledge");
   evt.wait(10*1000); // 30 sec

   m_d.log().writeLn("back from the future; closing");
}

std::string upgradeCallUnpackedRecipe::getCalleePath()
{
   auto packageFullName = cmn::buildPackageFullName(m_package);
   return m_d.store().predictPackagePath(packageFullName.c_str());
}

std::string upgradeCallInstalledRecipe::getCalleePath()
{
   return cmn::buildPackageTargetPath(m_package);
}

void addToPathInstr::execute()
{
   m_d.log().writeLn("adding '%s' to PATH",m_path.c_str());
   tcat::typePtr<envvar::iPathVar> pathVar;
   pathVar->tie(m_d.log());
   pathVar->addToPath(m_path);
}

void addToPathInstr::config(sst::dict& c)
{
   std::string targetPath = cmn::buildPackageTargetPath(m_package);

   if(c.has("path"))
   {
      m_path = c["path"].as<sst::str>().get();
      if(m_path.c_str()[0] == '$')
      {
         // one of our 'psuedopaths'
         if(m_path.c_str()[1] == 'I')
         {
            // relative to install location
            m_path = targetPath + (m_path.c_str()+2);
         }
         else
            throw std::runtime_error("unknown script pseudopath");
      }
   }
   else
      m_path = targetPath;
}

instrBase *addToPathInstr::invert()
{
   auto *pNoob = new removeFromPathInstr(m_d,m_package);
   pNoob->m_path = m_path;
   return pNoob;
}

void removeFromPathInstr::execute()
{
   m_d.log().writeLn("removing '%s' from PATH",m_path.c_str());
   tcat::typePtr<envvar::iPathVar> pathVar;
   pathVar->tie(m_d.log());
   pathVar->removeFromPath(m_path);
}

instrBase *removeFromPathInstr::invert()
{
   throw std::runtime_error("unimplemented! 233");
}

void batchFileInstr::execute()
{
   // programm mode at the last minute, in case I was inverted
   m_pScript->addVar("mode",m_install ? "install" : "uninstall");

   // run!
   m_pScript->execute(m_scriptPath.c_str(),m_d.log());
}

void batchFileInstr::config(sst::dict& c)
{
   // propagate any user vars from package install steps
   for(auto it=c.asMap().begin();it!=c.asMap().end();++it)
      if(it->first != "type" && it->first != "path")
         if(auto pVal = it->second->is<sst::str>())
            m_pScript->addVar(it->first.c_str(),pVal->get().c_str());

   // set name/version from manifest
   m_pScript->addVar("package-name",m_package["name"].as<sst::str>().get().c_str());
   m_pScript->addVar("package-version",m_package["version"].as<sst::mint>().toString().c_str());

   // set target install/uninstall location
   auto packageBitness = m_package.getOpt<sst::str>("bitness","32");
   if(packageBitness != "32" && packageBitness != "64")
      throw std::runtime_error("unknown bitness");
   std::string targetPath = cmn::buildPackageTargetPath(m_package);
   m_pScript->addVar("target-path",targetPath.c_str());
   m_pScript->addVar("package-bitness",packageBitness.c_str());

   // set downloaded package location
   auto packageFullName = cmn::buildPackageFullName(m_package);
   std::string packagePath = m_d.store().predictPackagePath(packageFullName.c_str());
   m_pScript->addVar("package-path",packagePath.c_str());

   // resolve the script path
   m_scriptPath = c["path"].as<sst::str>().get();
   if(m_scriptPath.c_str()[0] == '$')
   {
      // one of our 'psuedopaths'
      if(m_scriptPath.c_str()[1] == 'L')
      {
         // gordian script library
         tcat::typePtr<file::iFileManager> pFm;
         m_scriptPath = pFm->calculatePath(file::iFileManager::kExeAdjacent,m_scriptPath.c_str()+3);
      }
      else if(m_scriptPath.c_str()[1] == 'P')
      {
         // relative to package location
         m_scriptPath = packagePath + (m_scriptPath.c_str()+2);
      }
      else
         throw std::runtime_error("unknown script pseudopath");
   }
}

instrBase *batchFileInstr::invert()
{
   m_install = !m_install;
   return NULL;
}

} // namespace curator
