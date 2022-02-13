#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <exception>
#include <windows.h>

class dummy : public test::iAsserter {
public:
   virtual void thing() {}
};

int main(int , char *[])
{
   console::cStdOutLogSink logSink;
   tcat::typePtr<console::iLogFactory> pLogFactory;
   cmn::autoReleasePtr<console::iLog> pLog(&pLogFactory->createLog(logSink));
   pLog->writeLn("looking for tests");

   try
   {
      tcat::typeSet<test::iTest> tests;
      pLog->writeLn("found %lld tests", tests.size());
      for(size_t i=0;i<tests.size();i++)
      {
         test::iTest *pInstance = tests[i];
         dummy d;
         pInstance->run(d);
      }
      pLog->writeLn("done");
   }
   catch(std::exception& x)
   {
      pLog->writeLn("ERROR: %s",x.what());
   }

   return 0;
}
