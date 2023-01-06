#include "../console/arg.hpp"
#include "curatorCommand.hpp"
#include "install.verb.hpp"
#include <memory>

namespace {

class listCommand : public console::iCommand {
public:
   virtual void run(console::iLog& l);
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<listCommand>("--list"));

      return v.release();
   }
} gVerb;

void listCommand::run(console::iLog& l)
{
   gordian::curatorCommand::compileRecipeAndSave(l,[&]()
      {
         return curator::request(curator::iRequest::kList,"*",true);
      });
}

} // anonymous namespace
