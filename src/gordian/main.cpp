#include "../cmn/autoPtr.hpp"
#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include <exception>
#include <stddef.h>

class initCommand {};

class scrubCommand {
public:
   scrubCommand() : yes(false) {}
   bool yes;
};

int main(int , char *[])
{
   using namespace console;
   cStdOutLogSink logSink;
   tcat::typePtr<iLogFactory> pLogFactory;
   cmn::autoReleasePtr<iLog> pLog(&pLogFactory->createLog(logSink));

   try
   {
      verb<initCommand> init("--init");
      verb<scrubCommand> scrub("--scrub");
      scrub
         .addOption(*new boolOption("-y",offsetof(scrubCommand,yes)))
         .addTag("--yes");
   }
   catch(std::exception& x)
   {
      pLog->writeLn("ERROR: %s",x.what());
   }

   return 0;
}
