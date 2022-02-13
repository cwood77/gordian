#define WIN32_LEAN_AND_MEAN
#include "../tcatlib/api.hpp"
#include <windows.h>

#ifdef cdwTest
#include "../cmn/autoPtr.hpp"
#include "../console/log.hpp"
#include "../test/api.hpp"
#include "api.hpp"

testDefineTest(sst_write)
{
   sst::dict config;
   auto& installed = config.add<sst::array>("installed");
   auto& prod = installed.append<sst::dict>();
   prod.add<sst::str>("name") = "some product";

   console::cStdOutLogSink logSink;
   tcat::typePtr<console::iLogFactory> pLogFactory;
   cmn::autoReleasePtr<console::iLog> pLog(&pLogFactory->createLog(logSink));
   pLog->writeLn("inside test");
   config.write(pLog.get());
}

#endif // cdwTest

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
