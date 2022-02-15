#include "../cmn/autoPtr.hpp"
#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include <exception>
#include <stddef.h>

using namespace console;

#if 0
class initCommand : public iCommand {
};

class scrubCommand : public iCommand {
public:
   scrubCommand() : yes(false) {}
   bool yes;
};
#endif

int main(int , char *[])
{
   cStdOutLogSink logSink;
   tcat::typePtr<iLogFactory> pLogFactory;
   cmn::autoReleasePtr<iLog> pLog(&pLogFactory->createLog(logSink));

   try
   {
#if 0
      verb<initCommand> init("--init");
      verb<scrubCommand> scrub("--scrub");
      scrub
         .addOption(*new boolOption("-y",offsetof(scrubCommand,yes)))
         .addTag("--yes");
#endif
   }
   catch(std::exception& x)
   {
      pLog->writeLn("ERROR: %s",x.what());
   }

   return 0;
}
