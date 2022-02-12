#ifndef ___tcatlib_api___
#define ___tcatlib_api___

#include <stdint.h>
#include <typeinfo>
#include <vector>
#include "../tcatbin/api.hpp"

namespace tcat {

class libStub {
public:
   static libStub& get();

   void addref();
   void release();

   tcatbin::iCatalog& getCat() { return *m_pCat; }

private:
   libStub();

   uint32_t m_refCnt;
   void *m_dllPtr;
   tcatbin::iCatalog *m_pCat;
};

class libRef {
public:
   libRef();
   ~libRef();
   libRef(const libRef& source);
   libRef& operator=(const libRef& source);

   tcatbin::iCatalog& getCat() { return m_pPtr->getCat(); }

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

class staticModuleServer : public tcatbin::iModuleServer {
public:
   static staticModuleServer& get();
   void add(tcatbin::iTypeServer& t);

   virtual size_t getNumTypes() const;
   virtual tcatbin::iTypeServer *getIthType(size_t i);

private:
   std::vector<tcatbin::iTypeServer*> m_types;
};

template<class I, class T>
class staticTypeServer : public tcatbin::iTypeServer {
public:
   staticTypeServer() { staticModuleServer::get().add(*this); }
   virtual const char *getTypeName() const { return typeid(I).name(); }
   virtual void *createType() { return new T(); }
   virtual void releaseType(void *p) { delete (T*)p; }
};

#define tcatImplTypeServer(__type__,__intf__) ;

#define tcatImplModuleServer() ;

} // namespace tcat

#endif // ___tcatlib_api___
