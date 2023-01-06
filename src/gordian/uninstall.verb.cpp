#include "../console/arg.hpp"
#include "curatorCommand.hpp"
#include "install.verb.hpp"
#include <memory>

namespace {

class uninstallCommand : public console::iCommand {
public:
   uninstallCommand() : oYes(false), oPattern("*") {}

   bool oYes;
   std::string oPattern;

   virtual void run(console::iLog& l);
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<uninstallCommand>("--uninstall"));

      (*v)
         .addParameter(
            console::stringParameter::required(offsetof(uninstallCommand,oPattern)))
         .addOption(*new console::boolOption("--yes",offsetof(uninstallCommand,oYes)))
            .addTag("-y")
      ;

      return v.release();
   }
} gVerb;

void uninstallCommand::run(console::iLog& l)
{
   gordian::curatorCommand::compileRecipeAndSave(l,[&]()
      {
         return curator::request(curator::iRequest::kUninstall,oPattern,oYes);
      });
}

} // anonymous namespace
