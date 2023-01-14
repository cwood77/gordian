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

void multiDownload::download(const std::string& path, size_t nParts)
{
   for(size_t i=0;i<=nParts;i++)
   {
      std::stringstream fullName;
      fullName << path << ".ar.z.s.sg" << i;
      m_http.download(fullName.str());
   }
}

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

void htmlPage::loadFromDisk(const std::string& path)
{
   cmn::sizedAlloc a;
   {
      cmn::autoCFilePtr f(path,"rb");
      a.realloc(f.calculateFileSizeFromStart());
      ::fread(a.ptr(),1,a.size(),f.fp);
   }

   for(size_t i=0;i<a.size();i++)
   {
      const char *pThumb = a.ptr() + i;
      if(::strncmp(pThumb,"HERE IS WISDOM: ",16) == 0)
      {
         pThumb += 16;
         cmn::block<1024> baseName;
         unsigned long nParts;
         int rVal = ::sscanf(pThumb,"%[^:]:%lu",baseName.b,&nParts);
         if(rVal != 2)
            throw std::runtime_error("ISE parsing htmlPage");
         m_baseName = baseName.b;
         m_nParts = nParts;
         return;
      }
   }
   throw std::runtime_error("invalid html: doesn't seem like mine?");
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

   d.add<sst::str>("store-protocol") = typeid(freewebStore).name();
   auto& mySet = d.add<sst::dict>("freewebStore-settings");
   mySet
      .add<sst::str>("disk-path")
         = fMan->calculatePath(file::iFileManager::kUserData,"freewebstore");
   mySet
      //.add<sst::str>("url") = "cdwe-gordian.infinityfreeapp.com";
      .add<sst::str>("url") = "stedfast-cases.000webhostapp.com";
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
   downloadCatalogIf();
   m_strCache
      = settings()["disk-path"].as<sst::str>().get() + "\\"
      + m_catalogBaseName
      + "\\m";
   return m_strCache.c_str();
}

void freewebStore::depopulateManifests()
{
   tcat::typePtr<file::iFileManager> fMan;
   fMan->deleteFolderAndContentsIf(
      settings()["disk-path"].as<sst::str>().get().c_str(),
      *m_pLog,
      true);
}

const char *freewebStore::populatePackage(const char *pPackageName) // e.g. "vault-7-win32-rel"
{
   // catalog has already been downloaded, as the manifests have been read
   // lookup nParts from packagename in dir.sst
   tcat::typePtr<file::iFileManager> fMan;
   size_t nParts;
   {
      cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
         (settings()["disk-path"].as<sst::str>().get() + "\\cat\\dir.sst").c_str()
      ));
      pFile->tie(*m_pLog);
      if(!pFile->existed())
         throw std::runtime_error("dir.sst file not found in catalog");
      nParts = pFile->dict()[pPackageName].as<sst::mint>().get();
   }

   // create packages location if
   std::string path = settings()["disk-path"].as<sst::str>().get() + "\\packages";
   fMan->createAllFoldersForFile(path.c_str(),*m_pLog,true);

   m_pLog->writeLn("doing a multidownload with '%s'/%lld",pPackageName,nParts);

   // do a multi-download to packages location
   tcat::typePtr<http::iHttpReader> http;
   http->tie(*m_pLog);
   http->bind(
      settings()["url"].as<sst::str>().get(),
      path);
   fweb::multiDownload(*m_pLog,*http).download(pPackageName,nParts);

   // do a multi-package join and unpack
   tcat::typePtr<file::iPackagerFactory> pFac;
   pFac->tie(*m_pLog,*m_pRootSettings);
   cmn::autoReleasePtr<file::iPackager> pPack(&pFac->compose(0xFFFF)); // everything
   path += "\\";
   path += pPackageName;
   path += ".ar.z.s.sg0";
   m_strCache = pPack->unpack(path.c_str());

   // return the unpacked path
   return m_strCache.c_str();
}

void freewebStore::depopulatePackage(const char *pPackageName)
{
   m_strCache = predictPackagePath(pPackageName);
   m_pLog->writeLn("[passthru] depopulating %s",m_strCache.c_str());

   tcat::typePtr<file::iFileManager> pFm;
   pFm->deleteFolderAndContentsIf(m_strCache.c_str(),*m_pLog,true);
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

      m_pLog->writeLn("=== splitting %s",args[1].c_str());
      std::string packNameWithExt = args[1] + ".ar.z.s";
      std::string packagePath = predictPackagePath(packNameWithExt.c_str());
      size_t nParts = 0;
      multiPackage(*m_pLog,*m_pRootSettings).split(packagePath,nParts);

      std::string catPath = settings()["disk-path"].as<sst::str>().get() + "\\cat"; // assumes base name is 'cat'
      std::string dirPath = catPath + "\\dir.sst";
      m_pLog->writeLn("=== updating %s",dirPath.c_str());
      tcat::typePtr<file::iFileManager> fMan;
      {
         cmn::autoReleasePtr<file::iSstFile> pDir(&fMan->bindFile<file::iSstFile>(
            dirPath.c_str()));
         pDir->tie(*m_pLog);
         pDir->dict().add<sst::mint>(packagePath) = nParts;
         pDir->scheduleFor(file::iFileManager::kSaveOnClose);
      }

      m_pLog->writeLn("=== repacking %s",catPath.c_str());
      tcat::typePtr<file::iPackagerFactory> pFac;
      pFac->tie(*m_pLog,*m_pRootSettings);
      cmn::autoReleasePtr<file::iPackager> pPack(&pFac->compose(0x7)); // skip splitting
      packagePath = pPack->pack(catPath.c_str());

      m_pLog->writeLn("=== resplitting %s",packagePath.c_str());
      multiPackage(*m_pLog,*m_pRootSettings).split(packagePath,nParts);

      // generate the mainhttp
      htmlPage main;
      main.setCatalogInfo(packagePath,nParts);
      std::string mainPath = settings()["disk-path"].as<sst::str>().get() + "\\index2.html";
      m_pLog->writeLn("=== regenerating %s",mainPath.c_str());
      main.saveToDisk(mainPath.c_str());

      m_catDownloaded = false;
      return;
   }
   throw std::runtime_error("store only supports command 'config url <X>' and 'add <X>'");
}

void freewebStore::downloadCatalogIf()
{
   if(m_catDownloaded) return;

   tcat::typePtr<http::iHttpReader> http;
   http->tie(*m_pLog);
   http->bind(
      settings()["url"].as<sst::str>().get(),
      settings()["disk-path"].as<sst::str>().get());

   // pull down and pick apart main page
   http->download("index2.html");
   fweb::htmlPage main;
   main.loadFromDisk(settings()["disk-path"].as<sst::str>().get() + "\\index2.html");
   size_t nParts;
   main.getCatalogInfo(m_catalogBaseName,nParts);
//m_catalogBaseName = "cat"; nParts = 0;
   m_pLog->writeLn("html says catalog is '%s:%lld'",m_catalogBaseName.c_str(),nParts);

   // pull down catalog
   fweb::multiDownload(*m_pLog,*http).download(m_catalogBaseName,nParts);

   // whack the unpacked catalog if it exists (so I can unpack w/o error)
   tcat::typePtr<file::iFileManager> fMan;
   fMan->deleteFolderAndContentsIf(
      (settings()["disk-path"].as<sst::str>().get() + "\\"
      + m_catalogBaseName).c_str(),*m_pLog,true);

   // reassemble and unpack catalog
   tcat::typePtr<file::iPackagerFactory> pFac;
   pFac->tie(*m_pLog,*m_pRootSettings);
   cmn::autoReleasePtr<file::iPackager> pPack(&pFac->compose(0xFFFF)); // everything
   std::string catalogFullPath
      = settings()["disk-path"].as<sst::str>().get() + "\\"
      + m_catalogBaseName
      + ".ar.z.s.sg0";
   pPack->unpack(catalogFullPath.c_str());

   m_catDownloaded = true;
}

tcatExposeTypeAs(freewebStore,freewebStore);
namespace workaround {
tcatExposeTypeAs(freewebStore,iCurrentStore);
} // namespace workaround

} // namespace store
