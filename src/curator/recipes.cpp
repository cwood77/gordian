#include "../console/log.hpp"
#include "../exec/api.hpp"
#include "../file/api.hpp"
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
   b.populate(m_d.config(),true);
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
   // tempfile
   // job
   // timeout
   // log parse

   // TODO

   //tcat::typePtr<exec::iScriptRunner> pScript;
   //pScript->execute("thingee",m_d.log());
   m_d.log().writeLn("would have run batch file");
}

void batchFileInstr::config(sst::dict& c)
{
   // TODO

   // what comes from the package sst?
   // - package name
   // - version

   // what comes from the config sst?
   // - timeout
   // - batchfile path

   // what comes from class fields?
   // - install path
   // - package path
   // - bool install/uninstall

   // what's made up by this class?
   // - log path
   // - log error token
}

// how are logs managed?  is that my job?

instrBase *batchFileInstr::invert()
{
   // TODO
   return NULL;
}

} // namespace curator
