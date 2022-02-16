#include "../cmn/autoPtr.hpp"
#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include <exception>

int main(int argc, const char *argv[])
{
   console::cStdOutLogSink logSink;
   tcat::typePtr<console::iLogFactory> pLogFactory;
   cmn::autoReleasePtr<console::iLog> pLog(&pLogFactory->createLog(logSink));

   try
   {
      tcat::typePtr<console::iCommandLineParser> pParser;

      console::autoVerbs v;
      v.program(*pParser);

      console::iCommand *pCmd = pParser->parse(argc,argv);
      if(!pCmd)
         throw std::runtime_error("bad usage");

      pCmd->run(*pLog);
   }
   catch(std::exception& x)
   {
      pLog->writeLn("ERROR: %s",x.what());
   }

   return 0;
}
