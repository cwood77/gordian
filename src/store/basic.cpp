#define WIN32_LEAN_AND_MEAN
#include "../console/log.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "basic.hpp"
#include "passthru.hpp"
#include <typeinfo.h>
#include <windows.h>

namespace store {

void basicStore::initConfiguration(sst::dict& d) const
{
   d.add<sst::dict>("basicStore-settings")
      .add<sst::str>("url") = "https://192.168.39.176:8080/_gordian.html";
}

bool basicStore::tryActivate(sst::dict& d, const std::string& name, std::set<std::string>& ans) const
{
   const std::string myName = "basic";
   ans.insert(myName);
   if(name != myName)
      return false;
   d.add<sst::str>("store-protocol") = typeid(basicStore).name();
   return true;
}

void basicStore::loadConfiguration(sst::dict& d, console::iLog& l)
{
   m_pLog = &l;
   m_pRootSettings = &d;
   m_pMySettings = &(*m_pRootSettings)["basicStore-settings"].as<sst::dict>();
}

iStore *basicStore::upgradeIf()
{
   // comment this out when basic store is ready to test
   m_pLog->writeLn("[basicStore] delegating to passthru as I'm unfinished");
   tcat::typePtr<iStore> pOtherStore(typeid(passthruStore).name());
   pOtherStore->loadConfiguration(*m_pRootSettings,*m_pLog);
   store::iStore *pUpdatedStore = pOtherStore->upgradeIf();
   if(pUpdatedStore)
      pOtherStore.reset(pUpdatedStore);
   return pOtherStore.leak();
}

const char *basicStore::populateManifests()
{
   throw std::runtime_error("this should be unreachable");
   tcat::typePtr<file::iFileManager> pFm;
   m_strCache = pFm->calculatePath(file::iFileManager::kAppData,"manifests");
   pFm->createAllFoldersForFolder(m_strCache.c_str(),*m_pLog,true);
   return m_strCache.c_str();
}

const char *basicStore::populatePackage(const char *pPackageName)
{
   throw std::runtime_error("this should be unreachable");
   std::string packagePath("packages\\");
   packagePath += pPackageName;

   tcat::typePtr<file::iFileManager> pFm;
   m_strCache = pFm->calculatePath(file::iFileManager::kAppData,packagePath.c_str());
   pFm->createAllFoldersForFolder(m_strCache.c_str(),*m_pLog,true);
   return m_strCache.c_str();
}

void basicStore::command(const std::vector<std::string>& args)
{
   throw std::runtime_error("store doesn't support command");
}

tcatExposeTypeAs(basicStore,basicStore);
namespace workaround {
tcatExposeTypeAs(basicStore,iCurrentStore);
} // namespace workaround

} // namespace store

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
