#include "impl.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace tcatbin {

iCatalog& iCatalog::create()
{
   static catalog _c;
   return _c;
}

void iCatalog::destroy()
{
}

catalog::catalog()
{
   //folderReflector fr(m_metadata);
}

void *catalog::createSingleType(const char *pTypeName)
{
   return NULL;
}

void *catalog::createMultipleTypes(const char *pTypeName, size_t& n)
{
   return NULL;
}

void catalog::releaseType(void *pPtr)
{
}

} // namespace tcatbin

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
