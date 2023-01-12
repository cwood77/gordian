#include "../console/arg.hpp"
#include "../archive/keyExpert.hpp"
#include "../tcatlib/api.hpp"
#include <memory>
#include <stdexcept>

namespace {

class keysCommand : public console::iCommand {
public:
   keysCommand() : oRenew(false), oFree(false) {}

   virtual void run(console::iLog& l);

   bool oRenew;
   bool oFree;
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<keysCommand>("--keys"));

      (*v)
         .addOption(*new console::boolOption("--renew",offsetof(keysCommand,oRenew)))
            .addTag("-r")
      ;
      (*v)
         .addOption(*new console::boolOption("--free",offsetof(keysCommand,oFree)))
            .addTag("-f")
      ;

      return v.release();
   }
} gVerb;

void keysCommand::run(console::iLog& l)
{
   tcat::typePtr<keys::iKeyExpert> pKeys;
   size_t mode = keys::iKeyExpert::kList;
   if(oRenew)
      mode |= (keys::iKeyExpert::kRenew | keys::iKeyExpert::kExport);
   if(oFree)
      mode |= keys::iKeyExpert::kFree;
   pKeys->manageKeys(mode,l);
}

} // anonymous namespace
