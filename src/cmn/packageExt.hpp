#ifndef ___cmn_packageExt___
#define ___cmn_packageExt___

#include "../file/api.hpp"
#include <string>
#include <sstream>

namespace cmn {

inline std::string buildPackageFullName(sst::dict& manifest)
{
   std::stringstream stream;
   stream
      << manifest["name"].as<sst::str>().get()
      << "-"
      << manifest["version"].as<sst::mint>().get()
      << "-"
      << manifest.getOpt<sst::str>("platform","win32")
      << "-"
      << manifest.getOpt<sst::str>("config","rel")
   ;
   return stream.str();
}

} // namespace cmn

#endif // ___cmn_packageExt___
