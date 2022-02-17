#include "../cmn/autoPtr.hpp"
#include "../console/arg.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include <memory>
#include <stdexcept>

namespace {

class scrubCommand : public console::iCommand {
public:
   scrubCommand() : oYes(false) {}

   bool oYes;

   virtual void run(console::iLog& l);
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<scrubCommand>("--scrub"));

      v->addOption(*new console::boolOption("--yes",offsetof(scrubCommand,oYes)))
         .addTag("-y");

      return v.release();
   }
} gVerb;

void scrubCommand::run(console::iLog& l)
{
   tcat::typePtr<file::iFileManager> fMan;
   cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
      file::iFileManager::kUserData,
      "config.sst"
   ));
   pFile->tie(l);

   if(oYes)
      pFile->scheduleFor(file::iFileManager::kDeleteAndTidyOnClose);
}

} // anonymous namespace
