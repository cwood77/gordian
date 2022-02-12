// clients should go through tcatlib rather than this interface

#ifndef ___tcatbin_api___
#define ___tcatbin_api___

#include <cstddef>

namespace tcatbin {

class iCatalog {
public:
   static __declspec(dllexport) iCatalog& create();

   virtual ~iCatalog() {}
   virtual void *createSingleType(const char *pTypeName) = 0;
   virtual void *createMultipleTypes(const char *pTypeName, size_t& n) = 0;
   virtual void releaseType(void *pPtr) = 0;
};

class iTypeServer {
public:
   virtual ~iTypeServer() {}
   virtual const char *getTypeName() const = 0;
   virtual void *createType() = 0;
   virtual void releaseType(void *) = 0;
};

class iModuleServer {
public:
   virtual ~iModuleServer() {}
   virtual size_t getNumTypes() const = 0;
   virtual iTypeServer *getIthType(size_t i) = 0;
};

} // namespace tcatbin

#endif // ___tcatbin_api___
