#include "../file/api.hpp"
#include "api.hpp"
#include "basic.hpp"

namespace store {

void basicStore::loadConfiguration(sst::dict& d)
{
   /*m_pRootSettings = &d;
   if(!m_pRootSettings->has("basicStore-settings")
   {
      dict& mine = m_pRootSettings->add<sst::dict>("basicStore-settings");
      mine.add<sst::str>("url") = "https://192.168.39.176:8080/_gordian.html";
   }
   m_pMySettings = &(*m_pRootSettings)["basicStore-settings"].as<sst::dict>();*/
}

iStore *basicStore::upgradeIf()
{
   return NULL; //new basicStore(*this);
}

const char *basicStore::populateManifests()
{
   return "";
}

const char *basicStore::populatePackage(const char *pPackageName)
{
   return "";
}

basicStore::basicStore(const basicStore& other)
: m_pRootSettings(other.m_pRootSettings)
, m_pMySettings(other.m_pMySettings)
{
}

} // namespace store
