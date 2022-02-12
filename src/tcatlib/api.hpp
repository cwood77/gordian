#ifndef ___tcatlib_api___
#define ___tcatlib_api___

#include <stdint.h>
#include <typeinfo>
#include <vector>
#include "../tcatbin/api.hpp"

namespace tcat {

// translate return values into exceptions
class catalogWrapper : public tcatbin::iCatalog {
public:
   catalogWrapper();
   void set(tcatbin::iCatalog *pInner);

   virtual void *createSingleType(const char *pTypeName);
   virtual void *createMultipleTypes(const char *pTypeName, size_t& n);
   virtual void releaseType(void *pPtr);

private:
   tcatbin::iCatalog *m_pInner;
};

// loads/unloads the tcatbin library
class libStub {
public:
   static libStub& get();

   void addref();
   void release();

   tcatbin::iCatalog& getCat() { return m_cat; }

private:
   libStub();

   uint32_t m_refCnt;
   void *m_unloadFunc;
   void *m_dllPtr;
   catalogWrapper m_cat;
};

// refcounts on libStub
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

// a single-type accessor class
template<class T>
class typePtr {
public:
   typePtr() : m_pPtr(NULL)
   {
      m_pPtr = (T*)m_libRef.getCat().createSingleType(typeid(T).name());
   }

   ~typePtr()
   {
      m_libRef.getCat().releaseType(m_pPtr);
   }

   T *operator->() { return m_pPtr; }

private:
   libRef m_libRef;
   T *m_pPtr;

   template<class O> typePtr(const typePtr<O>& source);
   template<class O> typePtr<O>& operator=(const typePtr<O>& source);
};

// simple singleton module server
class staticModuleServer : public tcatbin::iModuleServer {
public:
   static staticModuleServer& get();
   void add(tcatbin::iTypeServer& t);

   virtual size_t getNumTypes() const;
   virtual tcatbin::iTypeServer& getIthType(size_t i);

private:
   std::vector<tcatbin::iTypeServer*> m_types;
};

// simple flyweight type server
template<class I, class T>
class staticTypeServer : public tcatbin::iTypeServer {
public:
   staticTypeServer() { staticModuleServer::get().add(*this); }
   virtual const char *getTypeName() const { return typeid(I).name(); }
   virtual void *createType() { return new T(); }
   virtual void releaseType(void *p) { delete (T*)p; }
};

#define tcatExposeTypeAs(__type__,__intf__) \
   tcat::staticTypeServer<__intf__,__type__> __typeServerIntf##__type__;

#define tcatImplServer() \
   __declspec(dllexport) tcatbin::iModuleServer *getModuleServer() \
   { \
      return &tcat::staticModuleServer::get(); \
   }

} // namespace tcat

#endif // ___tcatlib_api___
