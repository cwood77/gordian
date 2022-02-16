#include "../cmn/autoPtr.hpp"
#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
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

   pFile->dict().add<sst::array>("installed");
   pFile->dict().add<sst::str>("store-url") = "localhost";

   if(oYes)
      pFile->scheduleFor(file::iFileManager::kSaveOnClose);
}

} // anonymous namespace
