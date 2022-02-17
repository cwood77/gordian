#include "../file/api.hpp"
#include "api.hpp"

namespace store {

void basicStore::setPaths(const char *pManifestFolder, const char *pPackageFolder)
{
   m_manifestFolder = pManifestFolder;
   m_packageFolder = pPackageFolder;
}

void basicStore::loadConfiguration(sst::dict& d)
{
   m_pRootSettings = &d;
   if(!m_pRootSettings->has("basicStore-settings")
   {
      dict& mine = m_pRootSettings->add<sst::dict>("basicStore-settings");
      mine.add<sst::str>("url") = "https://192.168.39.176:8080/_gordian.html";
   }
   m_pMySettings = &(*m_pRootSettings)["basicStore-settings"].as<sst::dict>();
}

iStore *basicStore::upgradeIf()
{
   return new basicStore(*this);
}

void basicStore::populateManifests()
{
}

void basicStore::populatePackage(const char *pPackageName)
{
}

basicStore::basicStore(const basicStore& other)
: m_manifestFolder(other.m_manifestFolder)
, m_packageFolder(other.m_packageFolder)
, m_pRootSettings(other.m_pRootSettings)
, m_pMySettings(other.m_pMySettings)
{
}

} // namespace store
