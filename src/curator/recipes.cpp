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
   m_d.log().writeLn(
      "running install %s:%d",
      m_package["name"].as<sst::str>().get().c_str(),
      m_package["version"].as<sst::mint>().get());
}

void uninstallRecipe::execute()
{
   m_d.log().writeLn(
      "running uninstall %s:%d",
      m_package["name"].as<sst::str>().get().c_str(),
      m_package["version"].as<sst::mint>().get());
}

} // namespace curator
