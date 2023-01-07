#define WIN32_LEAN_AND_MEAN
#include "../console/log.hpp"
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

bool freewebStore::tryActivate(sst::dict& d, const std::string& name, std::set<std::string>& ans) const
{
   const std::string myName = "freeweb";
   ans.insert(myName);
   if(name != myName)
      return false;
   d.add<sst::str>("store-protocol") = typeid(freewebStore).name();
   return true;
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

void freewebStore::command(const std::vector<std::string>& args)
{
   if(args.size() == 3 && args[0] == "config")
   {
      if(args[1] == "url")
      {
         (*m_pMySettings)["url"].as<sst::str>() = args[2];
         m_pLog->writeLn("set");
         return;
      }
   }
   throw std::runtime_error("store only supports command 'config url <X>");
}

tcatExposeTypeAs(freewebStore,freewebStore);
namespace workaround {
tcatExposeTypeAs(freewebStore,iCurrentStore);
} // namespace workaround

} // namespace store
