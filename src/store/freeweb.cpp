#define WIN32_LEAN_AND_MEAN
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "freeweb.hpp"

namespace store {

void freewebStore::initConfiguration(sst::dict& d) const
{
   tcat::typePtr<file::iFileManager> fMan;

   auto& mySet = d.add<sst::dict>("freewebStore-settings");
   mySet
      .add<sst::str>("disk-path")
         = fMan->calculatePath(file::iFileManager::kUserData,"freewebstore");
   mySet
      .add<sst::str>("url") = "http://cdwe-gordian.infinityfreeapp.com/";
}

void freewebStore::loadConfiguration(sst::dict& d, console::iLog& l)
{
   m_pLog = &l;
   m_pRootSettings = &d;
   m_pMySettings = &(*m_pRootSettings)["freewebStore-settings"].as<sst::dict>();
}

iStore *freewebStore::upgradeIf()
{
   return NULL;
}

const char *freewebStore::predictPackagePath(const char *pPackageName)
{
   m_strCache
      = (*m_pMySettings)["disk-path"].as<sst::str>().get()
      + "\\packages\\"
      + pPackageName;
   return m_strCache.c_str();
}

const char *freewebStore::populateManifests()
{
   throw std::runtime_error("unimpled");
}

void freewebStore::depopulateManifests()
{
   throw std::runtime_error("unimpled");
}

const char *freewebStore::populatePackage(const char *pPackageName)
{
   throw std::runtime_error("unimpled");
}

void freewebStore::depopulatePackage(const char *pPackageName)
{
   throw std::runtime_error("unimpled");
}

tcatExposeTypeAs(freewebStore,freewebStore);
namespace workaround {
tcatExposeTypeAs(freewebStore,iCurrentStore);
} // namespace workaround

} // namespace store
