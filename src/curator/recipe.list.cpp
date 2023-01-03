#include "../console/log.hpp"
#include "../file/api.hpp"
#include "directory.hpp"
#include "recipes.hpp"

namespace curator {

void listRecipe::execute()
{
   m_d.categorizeInstalled();

   m_d.log().writeLn("");
   m_d.log().writeLn("%d packages available",m_d.dictsByGuid.size());

   auto it = m_d.availableGuidsSorted.begin();
   for(;it!=m_d.availableGuidsSorted.end();++it)
   {
      auto& versions = it->second;
      auto jit = versions.begin();
      for(;jit!=versions.end();++jit)
      {
         bool isInstalled = m_d.isInstalled(it->first,*jit);
         bool isDead = m_d.dictsByGuid[directory::calcManifestGuid(it->first,*jit)]
            ->getOpt<sst::tf>("discontinued",false);

         m_d.log().writeLn(
            " %s  %s v%d %s",
            (isInstalled ? "*" : " "),
            it->first.c_str(),
            *jit,
            (isDead ? "(discontinued)" : "")
         );
      }
   }

   m_d.log().writeLn("");
}

} // namespace curator
