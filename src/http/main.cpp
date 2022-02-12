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

tcat::staticTypeServer<iHttpReader,httpReader> _a;

} // namespace http

__declspec(dllexport) tcatbin::iModuleServer *getModuleServer()
{
   return &tcat::staticModuleServer::get();
}

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
