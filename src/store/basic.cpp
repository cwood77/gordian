#define WIN32_LEAN_AND_MEAN
#include "../file/api.hpp"
#include "../file/manager.hpp"
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

void basicStore::loadConfiguration(sst::dict& d, console::iLog& l)
{
   m_pLog = &l;
   m_pRootSettings = &d;
   m_pMySettings = &(*m_pRootSettings)["basicStore-settings"].as<sst::dict>();
}

iStore *basicStore::upgradeIf()
{
   return NULL;
}

const char *basicStore::populateManifests()
{
   tcat::typePtr<file::iFileManager> pFm;
   m_strCache = pFm->calculatePath(file::iFileManager::kAppData,"manifests");
   pFm->createAllFoldersForFolder(m_strCache.c_str(),*m_pLog,true);
   return m_strCache.c_str();
}

const char *basicStore::populatePackage(const char *pPackageName)
{
   std::string packagePath("packages\\");
   packagePath += pPackageName;

   tcat::typePtr<file::iFileManager> pFm;
   m_strCache = pFm->calculatePath(file::iFileManager::kAppData,packagePath.c_str());
   pFm->createAllFoldersForFolder(m_strCache.c_str(),*m_pLog,true);
   return m_strCache.c_str();
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
