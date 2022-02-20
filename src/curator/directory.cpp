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
   auto vers = d["version"].as<sst::mint>().get();

   return calcFullName(name,vers);
}

std::string directory::calcFullName(const std::string& name, size_t vers)
{
   std::stringstream stream;
   stream << name << ":" << vers;
   return stream.str().c_str();
}

directory::~directory()
{
   for(auto it=dictsByGuid.begin();it!=dictsByGuid.end();++it)
      delete it->second;
}

void directory::tie(console::iLog& l, sst::dict& config, store::iStore& s)
{
   m_pLog = &l;
   m_pDict = &config;
   m_pStore = &s;
}

void directory::loadAllManifestsIf(const std::string& manifestFolder)
{
   if(dictsByGuid.size() != 0)
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

   std::string pattern = r.getPattern();
   if(pattern == "*")
      return;

   const char *pColon = ::strchr(pattern.c_str(),':');
   if(!pColon)
   {
      nameMatch = pattern;
      return;
   }

   nameMatch = std::string(pattern.c_str(),pColon-pattern.c_str());
   verMatch = pColon + 1;
}

bool directory::isMatch(sst::dict& c, const std::string& nameMatch, const std::string& verMatch)
{
   bool isMatch = (nameMatch == "*");
   if(!isMatch)
   {
      auto& name = c["name"].as<sst::str>().get();
      isMatch = (nameMatch == name);
   }
   if(!isMatch)
      return false;

   auto& ver = c["version"].as<sst::mint>().get();
   if(verMatch == "*")
   {
      auto& name = c["name"].as<sst::str>().get();
      auto latestVer = *(--availableGuidsSorted[name].end());
      isMatch = (latestVer == ver);
   }
   else
   {
      isMatch = (((size_t)atoi(verMatch.c_str())) == ver);
   }
   if(!isMatch)
      return false;
   else
      return true;
}

bool directory::isInstalled(sst::dict& d)
{
   auto& name = d["name"].as<sst::str>().get();
   auto vers = d["version"].as<sst::mint>().get();

   return isInstalled(name,vers);
}

bool directory::isInstalled(const std::string& name, size_t vers)
{
   auto it = installedGuidsSorted.find(name);
   if(it == installedGuidsSorted.end())
      return false;
   auto jit = it->second.find(vers);
   return jit != it->second.end();
}

void directory::loadManifest(const std::string& manifest)
{
   tcat::typePtr<file::iFileManager> pFMan;
   cmn::autoReleasePtr<file::iSstFile> pFile(
      &pFMan->bindFile<file::iSstFile>(manifest.c_str())
   );

   auto& name = pFile->dict()["name"].as<sst::str>().get();
   auto vers = pFile->dict()["version"].as<sst::mint>().get();
   auto guid = calcFullName(pFile->dict());

   // categorize
   if(pFile->dict().has("categories"))
   {
      auto& keywords = pFile->dict()["categories"].as<sst::array>();
      for(size_t i=0;i<keywords.size();i++)
         guidsByCategory[keywords[i].as<sst::str>().get()].insert(guid);
   }
   auto pDict = pFile->abdicate();
   dictsByGuid[calcFullName(*pDict)] = pDict;
   availableGuidsSorted[name].insert(vers);
}

void directory::categorizeInstalled()
{
   auto& L = config()["installed"].as<sst::array>();
   for(size_t i=0;i<L.size();i++)
   {
      auto& dict = L[i].as<sst::dict>();

      auto& name = dict["name"].as<sst::str>().get();
      auto& vers = dict["version"].as<sst::mint>().get();
      auto guid = calcFullName(dict);

      installedGuidsSorted[name].insert(vers);
      installedGuidsByProdName[name].insert(guid);
   }
}

} // namespace curator
