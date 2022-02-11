#ifndef ___tcatbin_impl___
#define ___tcatbin_impl___

#include "api.hpp"
#include "metadata.hpp"
#include "tables.hpp"

namespace tcatbin {

class catalog : public iCatalog {
public:
   catalog();

   virtual void *createSingleType(const char *pTypeName);
   virtual void *createMultipleTypes(const char *pTypeName, size_t& n);
   virtual void releaseType(void *pPtr);

private:
   catalogMetadata m_metadata;
   binTable m_bTable;
   instTable m_iTable;
};

} // namespace tcatbin

#endif // ___tcatbin_impl___

