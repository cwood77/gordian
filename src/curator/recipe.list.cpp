#include "../console/log.hpp"
#include "directory.hpp"
#include "recipes.hpp"

namespace curator {

void listRecipe::execute()
{
   m_d.log().writeLn("");
   m_d.log().writeLn("%d packages available",m_d.all.size());

   auto it = m_d.all.begin();
   for(;it!=m_d.all.end();++it)
   {
      auto& versions = it->second;
      auto jit = versions.begin();
      for(;jit!=versions.end();++jit)
      {
         m_d.log().writeLn("    %s v%d",it->first.c_str(),jit->first);
      }
   }

   m_d.log().writeLn("");
}

} // namespace curator
