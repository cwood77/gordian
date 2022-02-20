#include "../console/log.hpp"
#include "directory.hpp"
#include "recipes.hpp"

namespace curator {

void listRecipe::execute()
{
   m_d.log().writeLn("");
   m_d.log().writeLn("%d packages available",m_d.dictsByGuid.size());

   auto it = m_d.availableGuidsSorted.begin();
   for(;it!=m_d.availableGuidsSorted.end();++it)
   {
      auto& versions = it->second;
      auto jit = versions.begin();
      for(;jit!=versions.end();++jit)
      {
         m_d.log().writeLn("    %s v%d",it->first.c_str(),*jit);
      }
   }

   m_d.log().writeLn("");
}

} // namespace curator
