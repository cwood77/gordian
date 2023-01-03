#include "../cmn/win32.hpp"
#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "install.verb.hpp"
#include <memory>
#include <sstream>

namespace {

class upgradeCommand : public console::iCommand {
public:
   std::string oPid;
   std::string oName;
   std::string oVersion;

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
            console::stringParameter::required(offsetof(upgradeCommand,oName)))
         .addParameter(
            console::stringParameter::required(offsetof(upgradeCommand,oVersion)))
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

   evt.set();
   ::WaitForSingleObject(hOldGordian,INFINITE);
   ::CloseHandle(hOldGordian);
   l.writeLn("old gordian exited; now I will kill him!");
   l.writeLn("<unimplemented>");

   gordian::installCommand ic;
   ic.oYes = true;
   ic.oPattern = oName + ":" + oVersion;
   ic.run(l);
}

} // anonymous namespace
