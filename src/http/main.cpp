#include <stdio.h>
#include "api.hpp"
#include "../tcatlib/api.hpp"
#ifdef cdwTest
#include "../test/api.hpp"
#endif // cdwTest
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace http {

class httpReader : public iHttpReader {
public:
   virtual void read()
   {
      printf("reading\n");
   }
};

tcatExposeTypeAs(httpReader,iHttpReader);

#ifdef cdwTest
testDefineTest(httpSanity)
{
   printf("running an http test\n");
}
#endif // cdwTest

} // namespace http

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
