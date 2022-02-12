#include "api.hpp"
#include "../tcatlib/api.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace http {

class httpReader : public iHttpReader {
public:
   virtual void read()
   {
   }
};

tcatExposeTypeAs(httpReader,iHttpReader);

} // namespace http

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
