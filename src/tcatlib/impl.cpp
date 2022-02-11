#include "api.hpp"

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
      path += "\\tcat.dll";
      m_dllPtr = ::LoadLibraryA(path.c_str());
      if(!m_dllPtr)
         throw std::runtime_error("tcatbin not found");
      typedef iCatalog& (*createFunc_t)();
      createFunc_t func = (createFunc_t)::GetProcAddress(m_dllPtr,"");
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
      ::FreeLibrary(m_dllPtr);
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
}

} // namespace tcat
