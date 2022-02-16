#include "../console/arg.hpp"
#include "../console/log.hpp"
#include <memory>

namespace {

class initCommand : public console::iCommand {
public:
   initCommand() : yes(false) {}

   bool yes;

   virtual void run(console::iLog& l);
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<initCommand>("--init"));

      v->addOption(*new console::boolOption("--yes",offsetof(initCommand,yes)))
         .addTag("-y");

      return v.release();
   }
} gVerb;

void initCommand::run(console::iLog& l)
{
   l.writeLn("inside command");
}

} // anonymous namespace
