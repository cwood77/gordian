#define WIN32_LEAN_AND_MEAN
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "passthru.hpp"
#include <typeinfo.h>
#include <windows.h>

namespace store {

void passthruStore::initConfiguration(sst::dict& d) const
{
   d.add<sst::str>("store-protocol_") = typeid(passthruStore).name();
   d.add<sst::dict>("passthruStore-settings")
      .add<sst::str>("disk-path") = "C:\\Users\\Don\\dev\\gordian\\testdata";
}

void passthruStore::loadConfiguration(sst::dict& d, console::iLog& l)
{
   m_pLog = &l;
   m_pRootSettings = &d;
   m_pMySettings = &(*m_pRootSettings)["passthruStore-settings"].as<sst::dict>();
}

iStore *passthruStore::upgradeIf()
{
   return NULL;
}

const char *passthruStore::populateManifests()
{
   m_strCache = (*m_pMySettings)["disk-path"].as<sst::str>().get() + "\\manifests";
   return m_strCache.c_str();
}

const char *passthruStore::populatePackage(const char *pPackageName)
{
   m_strCache = (*m_pMySettings)["disk-path"].as<sst::str>().get() + "\\packages";
   return m_strCache.c_str();
}

tcatExposeTypeAs(passthruStore,passthruStore);
namespace workaround {
tcatExposeTypeAs(passthruStore,iCurrentStore);
} // namespace workaround

} // namespace store
