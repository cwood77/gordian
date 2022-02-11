#include "impl.hpp"

namespace tcatbin {

iCatalog& iCatalog::create()
{
   static catalog _c;
   return _c;
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
