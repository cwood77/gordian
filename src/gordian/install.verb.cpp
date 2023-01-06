#include "../console/arg.hpp"
#include "curatorCommand.hpp"
#include "install.verb.hpp"
#include <memory>

namespace {

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<gordian::installCommand>("--install"));

      (*v)
         .addParameter(
            console::stringParameter::optional(offsetof(gordian::installCommand,oPattern)))
         .addOption(*new console::boolOption("--yes",offsetof(gordian::installCommand,oYes)))
            .addTag("-y")
      ;

      return v.release();
   }
} gVerb;

} // anonymous namespace
namespace gordian {

void installCommand::run(console::iLog& l)
{
   curatorCommand::compileRecipeAndSave(l,[&]()
      {
         return curator::request(curator::iRequest::kInstall,oPattern,oYes);
      });
}

} // namespace gordian
