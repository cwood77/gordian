#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "assert.hpp"
#include <exception>
#include <stddef.h>
#include <windows.h>

int main(int argc, const char *argv[])
{
   console::cStdOutLogSink logSink;
   tcat::typePtr<console::iLogFactory> pLogFactory;
   cmn::autoReleasePtr<console::iLog> pLog(&pLogFactory->createLog(logSink));
   pLog->writeLn("looking for tests");

   try
   {
      tcat::typeSet<test::iTest> tests;
      pLog->writeLn("found %lld tests",tests.size());
      for(size_t i=0;i<tests.size();i++)
      {
         test::iTest *pTest = tests[i];
         pLog->writeLn("running %s",pTest->getName());
         test::asserter a(pLog.get(),pTest->getName());
         pTest->run(a);
         a.complete();
      }
      pLog->writeLn("done");
   }
   catch(std::exception& x)
   {
      pLog->writeLn("ERROR: %s",x.what());
   }

   return 0;
}
