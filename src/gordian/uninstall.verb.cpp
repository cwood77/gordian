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

class uninstallCommand : public console::iCommand {
public:
   uninstallCommand() : oYes(false), oPattern("*") {}

   bool oYes;
   std::string oPattern;

   virtual void run(console::iLog& l);
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<uninstallCommand>("--uninstall"));

      (*v)
         .addParameter(
            console::stringParameter::required(offsetof(uninstallCommand,oPattern)))
         .addOption(*new console::boolOption("--yes",offsetof(uninstallCommand,oYes)))
            .addTag("-y")
      ;

      return v.release();
   }
} gVerb;

void uninstallCommand::run(console::iLog& l)
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
   curator::request r(curator::iRequest::kUninstall,oPattern,oYes);
   cmn::autoReleasePtr<curator::iRecipe> pRec(pCur->compile(manifestFolder.c_str(),r));
   pRec->execute();

   pFile->scheduleFor(file::iFileManager::kSaveOnClose);
}

} // anonymous namespace
