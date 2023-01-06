#include "../cmn/autoPtr.hpp"
#include "../cmn/win32.hpp"
#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "../curator/api.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../store/api.hpp"
#include "../tcatlib/api.hpp"
#include <memory>
#include <sstream>
#include <stdexcept>

namespace {

class upgradeCommand : public console::iCommand {
public:
   std::string oPid;
   std::string oVersion;
   std::string oExtra;

   virtual void run(console::iLog& l);
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<upgradeCommand>("--_upgrade"));

      (*v)
         .addParameter(
            console::stringParameter::required(offsetof(upgradeCommand,oPid)))
         .addParameter(
            console::stringParameter::required(offsetof(upgradeCommand,oVersion)))
         .addParameter(
            console::stringParameter::required(offsetof(upgradeCommand,oExtra)))
      ;

      return v.release();
   }
} gVerb;

void upgradeCommand::run(console::iLog& l)
{
   std::stringstream evtName;
   evtName << "_gordianUpgrade_" << oPid;
   cmn::osEvent evt(evtName.str());

   DWORD pid;
   ::sscanf(oPid.c_str(),"%lu",&pid);
   HANDLE hOldGordian = ::OpenProcess(SYNCHRONIZE,FALSE,pid);
   if(!hOldGordian)
      throw std::runtime_error("failed to open handle to old gordian");

   // release the old gordian and wait for it to exit
   // it is either about to be overwritten by a copy, or deleted
   // by an unfetch
   evt.set();
   ::WaitForSingleObject(hOldGordian,INFINITE);
   ::CloseHandle(hOldGordian);

   std::string pattern = "gordian:" + oVersion;

   // typical command boilerplate

   tcat::typePtr<file::iFileManager> fMan;
   cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
      file::iFileManager::kUserData,
      "config.sst"
   ));
   pFile->tie(l);
   if(!pFile->existed())
      throw std::runtime_error("config file not found");

   tcat::typePtr<store::iStore> pStore(
      pFile->dict()["store-protocol"].as<sst::str>().get());
   pStore->loadConfiguration(pFile->dict(),l);
   store::iStore *pUpdatedStore = pStore->upgradeIf();
   if(pUpdatedStore)
      pStore.reset(pUpdatedStore);

   std::string manifestFolder = pStore->populateManifests();

   tcat::typePtr<curator::iCurator> pCur;
   pCur->tie(l,pFile->dict(),*pStore);
   curator::request r(curator::iRequest::kList,pattern,true);
   if(oExtra == "unpacked")
      // copy the files and update the SST (install recipe)
      // call the installed version with 'installed' flag
      r = curator::request(curator::iRequest::kUpgrade_InstallAndInvoke,pattern,true);
   else if(oExtra == "installed")
      // unfetch
      // do another install if _another_ version of gordian is available
      r = curator::request(curator::iRequest::kUpgrade_UnfetchAndRecurse,pattern,true);
   else
      throw std::runtime_error("unsupported argument to upgrade: " + oExtra);
   cmn::autoReleasePtr<curator::iRecipe> pRec(pCur->compile(manifestFolder.c_str(),r));
   pRec->execute();

   pFile->scheduleFor(file::iFileManager::kSaveOnClose);
}

} // anonymous namespace
