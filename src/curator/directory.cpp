#include "../file/api.hpp"
#include "directory.hpp"

namespace curator {

directory::~directory()
{
   auto it = all.begin();
   for(;it!=all.end();++it)
   {
      auto jit = it->second.begin();
      for(;jit!=it->second.end();++jit)
         delete jit->second;
   }
}

void directory::tie(console::iLog& l, sst::dict& config, store::iStore& s)
{
   m_pLog = &l;
   m_pDict = &config;
   m_pStore = &s;
}

} // namespace curator
