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
   initCommand() : oYes(false), oForce(false) {}

   bool oYes;
   bool oForce;

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
      v->addOption(*new console::boolOption("--force",offsetof(initCommand,oForce)))
         .addTag("-f");

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

   if(!oForce && pFile->existed())
      throw std::runtime_error("config file already exists");

   pFile->dict().add<sst::array>("installed");
   pFile->dict().add<sst::str>("store") = "basic";
   pFile->dict().add<sst::dict>("basicStore-settings")
      .add<sst::str>("url") = "https://192.168.39.176:8080/_gordian.html";


   if(oYes)
      pFile->scheduleFor(file::iFileManager::kSaveOnClose);
}

} // anonymous namespace
