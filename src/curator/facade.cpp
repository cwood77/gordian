#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "directory.hpp"
#include "recipes.hpp"
#include "subcurator.hpp"
#include <stdexcept>
#include <string>
#include <windows.h>

namespace curator {

class facade : public iCurator {
public:
   virtual void tie(console::iLog& l, sst::dict& config, store::iStore& s)
   {
      m_d.tie(l,config,s);
   }

   virtual iRecipe *compile(const char *manifestFolder, const iRequest& r)
   {
      tcat::typeSet<iSubCurator> subs;
      for(size_t i=0;i<subs.size();i++)
      {
         iSubCurator *pS = subs[i];
         iRecipe *pR = pS->compile(m_d,r);
         if(pR)
            return pR;
      }

      throw std::runtime_error("unimpled request");

      /*
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

               checkForPrevVersion.insert(directory::calcFullName(**it));
            }
         }

         std::list<sst::dict*> toUninstall;
         if(m_d.config().has("installed"))
         {
            auto& L = m_d.config()["installed"].as<sst::array>();
            for(size_t i=0;i<L.size();i++)
            {
               auto guid = directory::calcFullName(L[i].as<sst::dict>());
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
         */
   }

private:
   directory m_d;
};

tcatExposeTypeAs(facade,iCurator);

} // namespace curator

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
