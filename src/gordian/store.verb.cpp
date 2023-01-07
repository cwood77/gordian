#include "../cmn/autoPtr.hpp"
#include "../console/arg.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../store/api.hpp"
#include "../tcatlib/api.hpp"
#include <memory>
#include <stdexcept>

namespace {

class storeCommand : public console::iCommand {
public:
   std::string oArg1;
   std::string oArg2;
   std::string oArg3;

   virtual void run(console::iLog& l);
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<storeCommand>("--store"));

      (*v)
         .addParameter(
            console::stringParameter::optional(offsetof(storeCommand,oArg1)))
         .addParameter(
            console::stringParameter::optional(offsetof(storeCommand,oArg2)))
         .addParameter(
            console::stringParameter::optional(offsetof(storeCommand,oArg3)))
      ;

      return v.release();
   }
} gVerb;

void storeCommand::run(console::iLog& l)
{
   tcat::typePtr<file::iFileManager> fMan;
   cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
      file::iFileManager::kUserData,
      "config.sst"
   ));
   pFile->tie(l);
   if(!pFile->existed())
      throw std::runtime_error("config file not found");

   if(oArg1 == "activate" && oArg3.empty())
   {
      std::set<std::string> ans;
      tcat::typeSet<store::iCurrentStore> pCurrentStores;
      bool done = false;
      for(size_t i=0;i<pCurrentStores.size()&&!done;i++)
         done = pCurrentStores[i]->tryActivate(pFile->dict(),oArg2,ans);
      if(!done)
      {
         std::stringstream stream;
         stream << "store '" << oArg2 << "' not found; valid names are";
         for(auto n : ans)
            stream << " " << n;
         throw std::runtime_error(stream.str());
      }
   }
   else
   {
      std::vector<std::string> args;
      if(!oArg1.empty())
         args.push_back(oArg1);
      if(!oArg2.empty())
         args.push_back(oArg2);
      if(!oArg3.empty())
         args.push_back(oArg3);

      tcat::typePtr<store::iStore> pStore(
         pFile->dict()["store-protocol"].as<sst::str>().get());
      pStore->loadConfiguration(pFile->dict(),l);
      // intentially don't update the store...

      pStore->command(args);
   }

   pFile->scheduleFor(file::iFileManager::kSaveOnClose);
}

} // anonymous namespace
