#include "../cmn/autoPtr.hpp"
#include "../console/arg.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../store/api.hpp"
#include "../tcatlib/api.hpp"
#include "version.hpp"
#include <memory>
#include <stdexcept>

namespace {

class initCommand : public console::iCommand {
public:
   initCommand() : oYes(false) {}

   bool oYes;

   virtual void run(console::iLog& l);
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<initCommand>("--init"));

      v->addOption(*new console::boolOption("--yes",offsetof(initCommand,oYes)))
         .addTag("-y");

      return v.release();
   }
} gVerb;

void initCommand::run(console::iLog& l)
{
   tcat::typePtr<file::iFileManager> fMan;
   cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
      file::iFileManager::kUserData,
      "config.sst"
   ));
   pFile->tie(l);
   if(pFile->existed())
      throw std::runtime_error("config file already exists");

   auto& me = pFile->dict().add<sst::array>("installed").append<sst::dict>();
   me.add<sst::str>("name") = "gordian";
   me.add<sst::mint>("version") = gMyVersion;

   tcat::typeSet<store::iCurrentStore> pCurrentStores;
   for(size_t i=0;i<pCurrentStores.size();i++)
      pCurrentStores[i]->initConfiguration(pFile->dict());

   if(oYes)
      pFile->scheduleFor(file::iFileManager::kSaveOnClose);
}

} // anonymous namespace
