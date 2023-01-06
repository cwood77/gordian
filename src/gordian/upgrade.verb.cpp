#include "../cmn/win32.hpp"
#include "../console/arg.hpp"
#include "curatorCommand.hpp"
#include "install.verb.hpp"
#include <memory>
#include <sstream>
#include <stdexcept>

namespace {

class upgradeCommand : public console::iCommand {
public:
   std::string oPid;
   std::string oVersion;
   std::string oExtra;

   virtual void run(console::iLog& l);
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<upgradeCommand>("--_upgrade"));

      (*v)
         .addParameter(
            console::stringParameter::required(offsetof(upgradeCommand,oPid)))
         .addParameter(
            console::stringParameter::required(offsetof(upgradeCommand,oVersion)))
         .addParameter(
            console::stringParameter::required(offsetof(upgradeCommand,oExtra)))
      ;

      return v.release();
   }
} gVerb;

void upgradeCommand::run(console::iLog& l)
{
   std::stringstream evtName;
   evtName << "_gordianUpgrade_" << oPid;
   cmn::osEvent evt(evtName.str());

   DWORD pid;
   ::sscanf(oPid.c_str(),"%lu",&pid);
   HANDLE hOldGordian = ::OpenProcess(SYNCHRONIZE,FALSE,pid);
   if(!hOldGordian)
      throw std::runtime_error("failed to open handle to old gordian");

   // release the old gordian and wait for it to exit
   // it is either about to be overwritten by a copy, or deleted
   // by an unfetch
   evt.set();
   ::WaitForSingleObject(hOldGordian,INFINITE);
   ::CloseHandle(hOldGordian);

   std::string pattern = "gordian:" + oVersion;

   // typical command boilerplate
   gordian::curatorCommand::compileRecipeAndSave(l,[&]()
      {
         if(oExtra == "unpacked")
            // copy the files and update the SST (install recipe)
            // call the installed version with 'installed' flag
            return curator::request(
               curator::iRequest::kUpgrade_InstallAndInvoke,pattern,true);
         else if(oExtra == "installed")
            // unfetch
            // do another install if _another_ version of gordian is available
            return curator::request(
               curator::iRequest::kUpgrade_UnfetchAndRecurse,pattern,true);
         else
            throw std::runtime_error("unsupported argument to upgrade: " + oExtra);
      });
}

} // anonymous namespace
