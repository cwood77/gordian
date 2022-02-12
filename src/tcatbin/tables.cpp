#include "tables.hpp"
#include "api.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace tcatbin {

libTable::~libTable()
{
   std::set<void*>::iterator it = m_libs.begin();
   for(;it!=m_libs.end();++it)
      ::FreeLibrary((HMODULE)*it);
}

void libTable::add(void* pLib)
{
   m_libs.insert(pLib);
}

void *instTable::create(iTypeServer& svr)
{
   void *pPtr = svr.createType();
   m_ptrs[pPtr] = &svr;
   return pPtr;
}

void instTable::release(void *pPtr)
{
   m_ptrs[pPtr]->releaseType(pPtr);
}

} // namespace tcatbin
