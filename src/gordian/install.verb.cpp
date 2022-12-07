#include "../cmn/autoPtr.hpp"
#include "../console/arg.hpp"
#include "../curator/api.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../store/api.hpp"
#include "../tcatlib/api.hpp"
#include "install.verb.hpp"
#include <memory>
#include <stdexcept>

namespace {

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<gordian::installCommand>("--install"));

      (*v)
         .addParameter(
            console::stringParameter::optional(offsetof(gordian::installCommand,oPattern)))
         .addOption(*new console::boolOption("--yes",offsetof(gordian::installCommand,oYes)))
            .addTag("-y")
      ;

      return v.release();
   }
} gVerb;

} // anonymous namespace
namespace gordian {

void installCommand::run(console::iLog& l)
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
   pCur->tie(l,pFile->dict(),*pStore);
   curator::request r(curator::iRequest::kInstall,oPattern,oYes);
   cmn::autoReleasePtr<curator::iRecipe> pRec(pCur->compile(manifestFolder.c_str(),r));
   pRec->execute();

   pFile->scheduleFor(file::iFileManager::kSaveOnClose);
}

} // namespace gordian
