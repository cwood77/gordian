#include "../cmn/autoPtr.hpp"
#include "../console/arg.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../file/packager.hpp"
#include "../tcatlib/api.hpp"
#include <memory>
#include <stdexcept>

namespace {

class packCommand : public console::iCommand {
public:
   std::string path;

   virtual void run(console::iLog& l);
};

class myPackVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<packCommand>("--pack"));

      (*v)
         .addParameter(
            console::stringParameter::required(offsetof(packCommand,path)))
      ;

      return v.release();
   }
} gPackVerb;

class unpackCommand : public console::iCommand {
public:
   std::string path;

   virtual void run(console::iLog& l);
};

class myUnpackVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<unpackCommand>("--unpack"));

      (*v)
         .addParameter(
            console::stringParameter::required(offsetof(unpackCommand,path)))
      ;

      return v.release();
   }
} gUnpackVerb;

void packCommand::run(console::iLog& l)
{
   tcat::typePtr<file::iFileManager> fMan;
   cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
      file::iFileManager::kUserData,
      "config.sst"
   ));
   pFile->tie(l);
   if(!pFile->existed())
      throw std::runtime_error("config file not found");

   tcat::typePtr<file::iPackagerFactory> pFac;
   pFac->tie(l,pFile->dict());
   cmn::autoReleasePtr<file::iPackager> pPack(&pFac->compose(0x7));
   pPack->pack(path.c_str());

   pFile->scheduleFor(file::iFileManager::kSaveOnClose);
}

void unpackCommand::run(console::iLog& l)
{
   tcat::typePtr<file::iFileManager> fMan;
   cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
      file::iFileManager::kUserData,
      "config.sst"
   ));
   pFile->tie(l);
   if(!pFile->existed())
      throw std::runtime_error("config file not found");

   tcat::typePtr<file::iPackagerFactory> pFac;
   pFac->tie(l,pFile->dict());
   cmn::autoReleasePtr<file::iPackager> pPack(&pFac->compose(0x7));
   pPack->unpack(path.c_str());

   pFile->scheduleFor(file::iFileManager::kSaveOnClose);
}

} // anonymous namespace
