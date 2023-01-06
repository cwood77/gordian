#include "../cmn/autoPtr.hpp"
#include "../console/log.hpp"
#include "../file/api.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "directory.hpp"
#include "recipes.hpp"
#include "subcurator.hpp"

namespace curator {

class installCurator : public iSubCurator {
public:
   virtual recipeBase *compile(directory& d, const iRequest& r)
   {
      if(r.getType() != iRequest::kInstall)
         return NULL;

      std::string n,v;
      d.parsePattern(r,n,v);

      size_t iCount = 0;
      cmn::autoReleasePtr<compositeRecipe> pMainR(new compositeRecipe());
      auto pDownloads = new compositeRecipe();
      auto pInstalls = new compositeRecipe();
      auto pUninstalls = new compositeRecipe();
      auto pRemoves = new compositeRecipe();
      pMainR->children.push_back(pDownloads);
      pMainR->children.push_back(pInstalls);
      pMainR->children.push_back(pUninstalls);
      pMainR->children.push_back(pRemoves);

      // install everything requested
      std::list<sst::dict*> toUninstall;
      for(auto it=d.dictsByGuid.begin();it!=d.dictsByGuid.end();++it)
      {
         if(d.isMatchOne(*it->second,n,v))
         {
            // ignore if already installed
            if(d.isInstalled(*it->second))
               continue;

            // installing an 'obsolete' version means removing it
            if(it->second->getOpt<sst::tf>("discontinued",false))
            {
               // uninstall all existing versions
               auto& prevVers
                  = d.installedGuidsByProdName[(*it->second)["name"].as<sst::str>().get()];
               for(auto jit=prevVers.begin();jit!=prevVers.end();++jit)
               {
                  iCount++;

                  // uninstall
                  pUninstalls->children.push_back(
                     new uninstallRecipe(d,*d.dictsByGuid[*jit]));
               }
            }
            else
            {
               iCount++;

               // schedule download and cleanup
               pDownloads->children.push_back(
                  new fetchRecipe(d,*it->second));
               pRemoves->children.push_back(
                  new unfetchRecipe(d,*it->second));

               // schedule install
               pInstalls->children.push_back(
                  new installRecipe(d,*it->second));
            }
         }
      }

      if(iCount == 0)
         d.log().writeLn("nothing to do");

      inflatingVisitor inflater;
      pMainR->acceptVisitor(inflater);

      return pMainR.abdicate();
   }
};

tcatExposeTypeAs(installCurator,iSubCurator);

} // namespace curator
