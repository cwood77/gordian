#include "../console/log.hpp"
#include "../file/api.hpp"
#include "directory.hpp"
#include "recipes.hpp"

namespace curator {

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

void installRecipe::execute()
{
   auto& name = m_package["name"].as<sst::str>().get();
   auto vers = m_package["version"].as<sst::mint>().get();

   m_d.log().writeLn("running install %s:%d",name.c_str(),vers);

   // mark the package installed
   auto& package = m_d.config()["installed"].as<sst::array>()
      .append<sst::dict>();
   package.add<sst::str>("name") = name;
   package.add<sst::mint>("version") = vers;

   // run the installer
}

void uninstallRecipe::execute()
{
   auto& name = m_package["name"].as<sst::str>().get();
   auto vers = m_package["version"].as<sst::mint>().get();

   m_d.log().writeLn("running uninstall %s:%d",name.c_str(),vers);

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

   // run the uninstaller
}

} // namespace curator
