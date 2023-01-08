#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../console/log.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../file/packager.hpp"
#include "../http/api.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "freeweb.hpp"
#include <cstring>
#include <fstream>

namespace store {
namespace fweb {

void multiPackage::split(std::string& path, size_t& nParts)
{
   tcat::typePtr<file::iPackagerFactory> pFac;
   pFac->tie(m_log,m_config);
   cmn::autoReleasePtr<file::iPackager> pPack(&pFac->compose(1<<3)); // split only
   const char *pLastPart = pPack->pack(path.c_str());

   // decide nParts from extension
   const char *pDot = pLastPart + ::strlen(pLastPart);
   for(;*pDot!='.';--pDot);
   int rval = ::sscanf(pDot,".sg%llu",&nParts);
   if(rval != 1)
      throw std::runtime_error("ISE");

   // return in 'path' just the filename without the full ext
   const char *pThumb = pDot;
   for(;*pThumb!='\\';--pThumb);
   path = std::string(pThumb+1,pDot-(pThumb+1)+3-10);
}

void htmlPage::saveToDisk(const std::string& path)
{
   std::ofstream out(path.c_str());
   out
      << "<html>" << std::endl
      << "<head>" << std::endl
      << "</head>" << std::endl
      << "<body>" << std::endl
      << "turn him to any cause of policy, the gordian knot of it he will unloose, familiar as his garter" << std::endl
      << "<!-- HERE IS WISDOM: " << m_baseName << ":" << m_nParts << "-->" << std::endl
      << "</body>" << std::endl
      << "</html>"
   ;
}

} // namespace fweb

void freewebStore::initConfiguration(sst::dict& d) const
{
   tcat::typePtr<file::iFileManager> fMan;

   auto& mySet = d.add<sst::dict>("freewebStore-settings");
   mySet
      .add<sst::str>("disk-path")
         = fMan->calculatePath(file::iFileManager::kUserData,"freewebstore");
   mySet
      .add<sst::str>("url") = "cdwe-gordian.infinityfreeapp.com";
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
      = settings()["disk-path"].as<sst::str>().get()
      + "\\packages\\"
      + pPackageName;
   return m_strCache.c_str();
}

const char *freewebStore::populateManifests()
{
   tcat::typePtr<http::iHttpReader> http;
   http->tie(*m_pLog);
   http->bind(
      settings()["url"].as<sst::str>().get(),
      settings()["disk-path"].as<sst::str>().get());
   http->download("main.html");

   throw std::runtime_error("unfinished");
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
         settings()["url"].as<sst::str>() = args[2];
         m_pLog->writeLn("set");
         return;
      }
   }
   else if(args.size() == 2 && args[0] == "add")
   {
      using namespace fweb;

      // split package
      std::string packNameWithExt = args[1] + ".ar.z.s";
      std::string packagePath = predictPackagePath(packNameWithExt.c_str());
      size_t nParts = 0;
      multiPackage(*m_pLog,*m_pRootSettings).split(packagePath,nParts);

      // update dir.sst
      std::string catPath = settings()["disk-path"].as<sst::str>().get() + "\\cat";
      std::string dirPath = catPath + "\\dir.sst";
      tcat::typePtr<file::iFileManager> fMan;
      {
         cmn::autoReleasePtr<file::iSstFile> pDir(&fMan->bindFile<file::iSstFile>(
            dirPath.c_str()));
         pDir->tie(*m_pLog);
         pDir->dict().add<sst::mint>(packagePath) = nParts;
         pDir->scheduleFor(file::iFileManager::kSaveOnClose);
      }

      // repack the catalog
      tcat::typePtr<file::iPackagerFactory> pFac;
      pFac->tie(*m_pLog,*m_pRootSettings);
      cmn::autoReleasePtr<file::iPackager> pPack(&pFac->compose(0x7)); // skip splitting
      packagePath = pPack->pack(catPath.c_str());

      // split the catalog
      multiPackage(*m_pLog,*m_pRootSettings).split(packagePath,nParts);

      // generate the mainhttp
      htmlPage main;
      main.setCatalogInfo(packagePath,nParts);
      std::string mainPath = settings()["disk-path"].as<sst::str>().get() + "\\main.html";
      main.saveToDisk(mainPath.c_str());
      return;
   }
   throw std::runtime_error("store only supports command 'config url <X>' and 'add <X>'");
}

tcatExposeTypeAs(freewebStore,freewebStore);
namespace workaround {
tcatExposeTypeAs(freewebStore,iCurrentStore);
} // namespace workaround

} // namespace store
