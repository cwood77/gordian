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

void directory::tie(console::iLog& l, sst::dict& config)
{
   m_pLog = &l;
   m_pDict = &config;
}

} // namespace curator
