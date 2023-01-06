#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../console/log.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../file/packager.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "passthru.hpp"
#include <typeinfo.h>
#include <windows.h>

namespace store {

void passthruStore::initConfiguration(sst::dict& d) const
{
   d.add<sst::dict>("passthruStore-settings")
      .add<sst::str>("disk-path") = "C:\\cygwin64\\home\\chris\\dev\\gordian\\testdata";
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

const char *passthruStore::predictPackagePath(const char *pPackageName)
{
   m_strCache
      = (*m_pMySettings)["disk-path"].as<sst::str>().get()
      + "\\packages\\"
      + pPackageName;
   return m_strCache.c_str();
}

const char *passthruStore::populateManifests()
{
   m_strCache = (*m_pMySettings)["disk-path"].as<sst::str>().get() + "\\manifests";
   return m_strCache.c_str();
}

const char *passthruStore::populatePackage(const char *pPackageName)
{
   m_strCache = predictPackagePath(pPackageName);
   m_pLog->writeLn("[passthru] populating %s",m_strCache.c_str());

   tcat::typePtr<file::iFileManager> pFm;
   if(!pFm->isFolder(m_strCache.c_str()))
   {
      m_pLog->writeLn("[passthru] unzipping");
      tcat::typePtr<file::iPackagerFactory> pFac;
      pFac->tie(*m_pLog,*m_pRootSettings);
      cmn::autoReleasePtr<file::iPackager> pPack(&pFac->compose(0xFFFF));
      std::string archive = m_strCache + ".ar.z.s";
      pPack->unpack(archive.c_str());
   }

   return m_strCache.c_str();
}

void passthruStore::depopulatePackage(const char *pPackageName)
{
   m_strCache = predictPackagePath(pPackageName);
   m_pLog->writeLn("[passthru] depopulating %s",m_strCache.c_str());

   tcat::typePtr<file::iFileManager> pFm;
   pFm->deleteFolderAndContents(m_strCache.c_str(),*m_pLog,false); // not really
}

tcatExposeTypeAs(passthruStore,passthruStore);
namespace workaround {
tcatExposeTypeAs(passthruStore,iCurrentStore);
} // namespace workaround

} // namespace store
