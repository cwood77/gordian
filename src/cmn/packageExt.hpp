#ifndef ___cmn_packageExt___
#define ___cmn_packageExt___

#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include <sstream>
#include <string>

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

inline std::string buildPackageTargetPath(sst::dict& manifest)
{
   tcat::typePtr<file::iFileManager> pFm;
   file::iFileManager::pathRoots bitness = file::iFileManager::kProgramFiles32Bit;
   auto packageBitness = manifest.getOpt<sst::str>("bitness","32");
   if(packageBitness == "64")
      bitness = file::iFileManager::kProgramFiles64Bit;
   return pFm->calculatePath(bitness,manifest["name"].as<sst::str>().get().c_str());
}

} // namespace cmn

#endif // ___cmn_packageExt___
