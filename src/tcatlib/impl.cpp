#include "api.hpp"
#include <stdexcept>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace tcat {

libStub& libStub::get()
{
   static libStub theStub;
   return theStub;
}

void libStub::addref()
{
   if(::InterlockedIncrement((LONG*)&m_refCnt) == 1)
   {
      char path[MAX_PATH];
      ::GetModuleFileNameA(NULL,path,MAX_PATH);
      std::string tcat = path;
      tcat += "\\..\\tcatbin.dll";
      m_dllPtr = ::LoadLibraryA(tcat.c_str());
      if(!m_dllPtr)
         throw std::runtime_error("tcatbin not found");
      typedef iCatalog& (*createFunc_t)();
      createFunc_t func = (createFunc_t)::GetProcAddress((HMODULE)m_dllPtr,"_ZN7tcatbin8iCatalog6createEv");
      if(!func)
         throw std::runtime_error("tcatbin incompatible");
      m_pCat = &func();
   }
}

void libStub::release()
{
   if(::InterlockedDecrement((LONG*)&m_refCnt) == 0)
   {
      m_pCat = NULL;
      ::FreeLibrary((HMODULE)m_dllPtr);
      m_dllPtr = NULL;
   }
}

libStub::libStub()
: m_refCnt(0)
, m_dllPtr(NULL)
, m_pCat(NULL)
{
}

libRef::libRef()
: m_pPtr(&libStub::get())
{
   m_pPtr->addref();
}

libRef::~libRef()
{
   m_pPtr->release();
}

libRef::libRef(const libRef& source)
: m_pPtr(source.m_pPtr)
{
   m_pPtr->addref();
}

libRef& libRef::operator=(const libRef& source)
{
   source.m_pPtr->addref();
   m_pPtr->release();
   m_pPtr = source.m_pPtr;
   return *this;
}

} // namespace tcat
