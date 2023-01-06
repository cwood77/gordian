#include "../cmn/autoPtr.hpp"
#include "../curator/api.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../store/api.hpp"
#include "../tcatlib/api.hpp"
#include "curatorCommand.hpp"
#include <stdexcept>

namespace gordian {

void curatorCommand::compileRecipeAndSave(console::iLog& l, std::function<curator::request(void)> func)
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
   curator::request r = func();
   cmn::autoReleasePtr<curator::iRecipe> pRec(pCur->compile(manifestFolder.c_str(),r));
   pRec->execute();

   pFile->scheduleFor(file::iFileManager::kSaveOnClose);
}

} // namespace gordian
