#include "impl.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace tcatbin {

iCatalog& iCatalog::create()
{
   return catalogRef::get().addref();
}

void iCatalog::destroy()
{
   catalogRef::get().release();
}

catalog::catalog()
{
   catalogBuilder builder(m_metadata,m_libs);
   folderReflector walker(builder);

   char path[MAX_PATH];
   ::GetModuleFileNameA(NULL,path,MAX_PATH);
   std::string folderPath = path;
   folderPath += "\\..";
   walker.reflect(folderPath);
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

catalogRef& catalogRef::get()
{
   static catalogRef the;
   return the;
}

iCatalog& catalogRef::addref()
{
   if(::InterlockedIncrement((LONG*)&m_refCnt) == 1)
      m_pInstance = new catalog();

   return *m_pInstance;
}

void catalogRef::release()
{
   if(::InterlockedDecrement((LONG*)&m_refCnt) == 0)
   {
      delete m_pInstance;
      m_pInstance = NULL;
   }
}

catalogRef::catalogRef()
: m_refCnt(0)
, m_pInstance(NULL)
{
}

} // namespace tcatbin

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
