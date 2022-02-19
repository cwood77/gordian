#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "directory.hpp"
#include "recipe.composite.hpp"
#include "recipe.fetch.hpp"
#include "recipe.install.hpp"
#include "recipe.list.hpp"
#include "recipe.unfetch.hpp"
#include "recipe.uninstall.hpp"
#include <sstream>
#include <stdexcept>
#include <string>
#include <windows.h>

namespace curator {

class curator : public iCurator {
public:
   virtual void tie(console::iLog& l, sst::dict& config, store::iStore& s)
   {
      m_d.tie(l,config,s);
   }

   virtual iRecipe *compile(const char *manifestFolder, const iRequest& r)
   {
      loadAllManifests(manifestFolder);

      if(r.getType() == iRequest::kList)
         return new listRecipe(m_d);
      else if(r.getType() == iRequest::kInstall)
      {
         std::string n,v;
         parsePattern(r,n,v);

         cmn::autoReleasePtr<compositeRecipe> pR(new compositeRecipe());
         std::set<std::string> checkForPrevVersion;

         for(auto it=m_d.flat.begin();it!=m_d.flat.end();++it)
         {
            if(isMatch(**it,n,v))
            {
               // ignore if obsolete
               if((*it)->getOpt<sst::tf>("discontinued",false))
                  continue;

               // ignore if already installed

               // push front: download
               pR->children.push_front(
                  new fetchRecipe(m_d,**it));

               // push back: install
               pR->children.push_back(
                  new installRecipe(m_d,**it));

               checkForPrevVersion.insert(buildGuid(**it));
            }
         }

         std::list<sst::dict*> toUninstall;
         if(m_d.config().has("installed"))
         {
            auto& L = m_d.config()["installed"].as<sst::array>();
            for(size_t i=0;i<L.size();i++)
            {
               auto guid = buildGuid(L[i].as<sst::dict>());
               if(checkForPrevVersion.find(guid)!=checkForPrevVersion.end())
                  toUninstall.push_back(&L[i].as<sst::dict>());
            }
         }

         // manage: defer-delete

         // for toUninstall
         for(auto it=toUninstall.begin();it!=toUninstall.end();++it)
         {
            // push back: uninstall
            pR->children.push_back(
               new uninstallRecipe(m_d,**it));
         }

         // for toUninstall
         for(auto it=toUninstall.begin();it!=toUninstall.end();++it)
         {
            // push back: remove
            pR->children.push_back(
               new unfetchRecipe(m_d,**it));
         }

         return pR.abdicate();
      }
      else
         throw std::runtime_error("unimpled curator request");
   }

private:
   static std::string buildGuid(sst::dict& d)
   {
      auto& name = d["name"].as<sst::str>().get();
      auto& vers = d["version"].as<sst::mint>().get();
      std::stringstream stream;
      stream << name << ":" << vers;
      return stream.str().c_str();
   }

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
      auto pDict = pFile->abdicate();
      m_d.all[name][vers] = pDict;
      m_d.flat.push_back(pDict);
   }

   void parsePattern(const iRequest& r, std::string& nameMatch, std::string& verMatch)
   {
      nameMatch = "*";
      verMatch = "*";
   }

   bool isMatch(sst::dict& c, const std::string& nameMatch, const std::string& verMatch)
   {
      return true;
   }

   directory m_d;
};

tcatExposeTypeAs(curator,iCurator);

} // namespace curator

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
