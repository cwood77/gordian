#include "../cmn/packageExt.hpp"
#include "../console/log.hpp"
#include "../exec/api.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../store/api.hpp"
#include "../tcatlib/api.hpp"
#include "directory.hpp"
#include "instr.hpp"
#include "recipes.hpp"

// experimental
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
//#include <wincrypt.h>
//#include <compressapi.h>

namespace curator {

void recipeVisitorBase::visit(compositeRecipe& n)
{
   for(auto it=n.children.begin();it!=n.children.end();++it)
      (*it)->acceptVisitor(*this);
}

compositeRecipe::~compositeRecipe()
{
   for(auto it=children.begin();it!=children.end();++it)
      delete *it;
}

void compositeRecipe::execute()
{
   for(auto it=children.begin();it!=children.end();++it)
      (*it)->execute();
}

void fetchRecipe::execute()
{
   m_d.log().writeLn(
      "running fetch %s:%d",
      m_package["name"].as<sst::str>().get().c_str(),
      m_package["version"].as<sst::mint>().get());
}

void unfetchRecipe::execute()
{
   m_d.log().writeLn(
      "running unfetch %s:%d",
      m_package["name"].as<sst::str>().get().c_str(),
      m_package["version"].as<sst::mint>().get());
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
   b.populate(m_d.config(),false);
}

void addToPathInstr::execute()
{
   // TODO
}

instrBase *addToPathInstr::invert()
{
   // TODO
   return NULL;
}

void removeFromPathInstr::execute()
{
   // TODO
}

instrBase *removeFromPathInstr::invert()
{
   // TODO
   return NULL;
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
   m_pScript->addVar("package-versions",m_package["version"].as<sst::mint>().toString().c_str());

   // set target install/uninstall location
   tcat::typePtr<file::iFileManager> pFm;
   file::iFileManager::pathRoots bitness = file::iFileManager::kProgramFiles32Bit;
   auto packageBitness = m_package.getOpt<sst::str>("bitness","32");
   if(packageBitness == "32")
      ;
   else if(packageBitness == "64")
      bitness = file::iFileManager::kProgramFiles64Bit;
   else
      throw std::runtime_error("unknown bitness");
   std::string targetPath = pFm->calculatePath(bitness,m_package["name"].as<sst::str>().get().c_str());
   m_pScript->addVar("target-path",targetPath.c_str());

   // set downloaded package location
   auto packageFullName = cmn::buildPackageFullName(m_package);
   std::string packagePath = m_d.store().populatePackage(packageFullName.c_str());
   m_pScript->addVar("package-path",packagePath.c_str());

   // resolve the script path
   m_scriptPath = c["path"].as<sst::str>().get();
   if(m_scriptPath.c_str()[0] == '$')
   {
      // one of our 'psuedopaths'
      if(m_scriptPath.c_str()[1] == 'L')
      {
         // gordian script library
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

// how are logs managed?  is that my job?

instrBase *batchFileInstr::invert()
{
   // TODO
   return NULL;
}

} // namespace curator
