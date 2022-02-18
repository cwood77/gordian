#include "../cmn/autoPtr.hpp"
#include "../console/arg.hpp"
#include "../curator/api.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../store/api.hpp"
#include "../tcatlib/api.hpp"
#include <memory>
#include <stdexcept>

namespace {

class listCommand : public console::iCommand {
public:
   virtual void run(console::iLog& l);
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<listCommand>("--list"));

      return v.release();
   }
} gVerb;

void listCommand::run(console::iLog& l)
{
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
   pCur->tie(l,pFile->dict());
   curator::request r(curator::iRequest::kList,"*",true);
   cmn::autoReleasePtr<curator::iRecipe> pRec(pCur->compile(manifestFolder.c_str(),r));

   pFile->scheduleFor(file::iFileManager::kSaveOnClose);
}

} // anonymous namespace
