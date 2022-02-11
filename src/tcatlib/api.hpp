#ifndef ___tcatlib_api___
#define ___tcatlib_api___

#include <stdint.h>

namespace tcat {

class iCatalog;

class libStub {
public:
   static libStub& get();

   void addref();
   void release();

   iCatalog& getCat() { return *m_pCat; }

private:
   libStub();

   uint32_t m_refCnt;
   void *m_dllPtr;
   iCatalog *m_pCat;
};

class libRef {
public:
   libRef();
   libRef(const libRef& source);
   libRef& operator=(const libRef& source);

   iCatalog& getCat() { return m_pPtr->getCat(); }

private:
   libStub *m_pPtr;
};

template<class T>
class typePtr {
public:
   typePtr() : m_pPtr(NULL)
   {
   }

private:
   libRef m_lilbRef;
   T *m_pPtr;
};

} // namespace tcat

#endif // ___tcatlib_api___
