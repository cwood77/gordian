#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <map>
#include <stdexcept>
#include <string>
#include <windows.h>

namespace curator {

class curator : public iCurator {
public:
   curator() : m_pLog(NULL), m_pDict(NULL) {}

   ~curator()
   {
      auto it = m_directory.begin();
      for(;it!=m_directory.end();++it)
      {
         auto jit = it->second.begin();
         for(;jit!=it->second.end();++jit)
            delete jit->second;
      }
   }

   virtual void tie(console::iLog& l, sst::dict& config)
   {
      m_pLog = &l;
      m_pDict = &config;
   }

   virtual iRecipe *compile(const char *manifestFolder, const iRequest& r)
   {
      loadAllManifests(manifestFolder);

      if(r.getType() == iRequest::kList)
         throw std::runtime_error("unimpled");
      else
         throw std::runtime_error("unimpled curator request");
   }

private:
   void loadAllManifests(const std::string& manifestFolder)
   {
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

   void loadManifest(const std::string& manifest)
   {
      tcat::typePtr<file::iFileManager> pFMan;
      cmn::autoReleasePtr<file::iSstFile> pFile(
         &pFMan->bindFile<file::iSstFile>(manifest.c_str())
      );

      auto& name = pFile->dict()["name"].as<sst::str>().get();
      auto& vers = pFile->dict()["version"].as<sst::mint>().get();
      m_directory[name][vers] = pFile->abdicate();
   }

   console::iLog *m_pLog;
   sst::dict *m_pDict;
   std::map<std::string,std::map<size_t,sst::dict*> > m_directory;
};

tcatExposeTypeAs(curator,iCurator);

} // namespace curator

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
