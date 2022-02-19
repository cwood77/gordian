#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "directory.hpp"
#include <sstream>
#include <windows.h>

namespace curator {

std::string directory::calcFullName(sst::dict& d)
{
   auto& name = d["name"].as<sst::str>().get();
   auto& vers = d["version"].as<sst::mint>().get();
   std::stringstream stream;
   stream << name << ":" << vers;
   return stream.str().c_str();
}

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

void directory::loadAllManifestsIf(const std::string& manifestFolder)
{
   if(flat.size() != 0)
      return;

   WIN32_FIND_DATA fData;
   HANDLE hFind = ::FindFirstFileA((manifestFolder + "\\*").c_str(),&fData);
   do
   {
      if(std::string(".") == fData.cFileName)
         continue;
      if(std::string("..") == fData.cFileName)
         continue;

      if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
         continue;

      if(::strlen(fData.cFileName) <= 4)
         continue;
      if(::strcmp(".sst",fData.cFileName + ::strlen(fData.cFileName)-4)!=0)
         continue;

      loadManifest(manifestFolder + "\\" + fData.cFileName);

   } while(::FindNextFileA(hFind,&fData));
   ::FindClose(hFind);
}

void directory::parsePattern(const iRequest& r, std::string& nameMatch, std::string& verMatch)
{
   nameMatch = "*";
   verMatch = "*";
}

bool directory::isMatch(sst::dict& c, const std::string& nameMatch, const std::string& verMatch)
{
   return true;
}

void directory::loadManifest(const std::string& manifest)
{
   tcat::typePtr<file::iFileManager> pFMan;
   cmn::autoReleasePtr<file::iSstFile> pFile(
      &pFMan->bindFile<file::iSstFile>(manifest.c_str())
   );

   auto& name = pFile->dict()["name"].as<sst::str>().get();
   auto& vers = pFile->dict()["version"].as<sst::mint>().get();
   auto pDict = pFile->abdicate();
   all[name][vers] = pDict;
   flat.push_back(pDict);
}

} // namespace curator
