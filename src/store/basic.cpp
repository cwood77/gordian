#define WIN32_LEAN_AND_MEAN
#include "../file/api.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "basic.hpp"
#include <typeinfo.h>
#include <windows.h>

namespace store {

void basicStore::initConfiguration(sst::dict& d) const
{
   d.add<sst::str>("store-protocol") = typeid(basicStore).name();
   d.add<sst::dict>("basicStore-settings")
      .add<sst::str>("url") = "https://192.168.39.176:8080/_gordian.html";
}

void basicStore::loadConfiguration(sst::dict& d)
{
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

tcatExposeTypeAs(basicStore,basicStore);
namespace workaround {
tcatExposeTypeAs(basicStore,iCurrentStore);
} // namespace workaround

} // namespace store

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
