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
   virtual iRecipe *compile(directory& d, const iRequest& r)
   {
      if(r.getType() != iRequest::kInstall)
         return NULL;

      d.categorizeInstalled();

      std::string n,v;
      d.parsePattern(r,n,v);

      auto *pGordianUpgrade = checkForSelfUpgrade(d,n,v);
      if(pGordianUpgrade)
      {
         inflatingVisitor inflater;
         pGordianUpgrade->acceptVisitor(inflater);
         return pGordianUpgrade;
      }

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
         if(d.isMatch(*it->second,n,v))
         {
            // ignore if obsolete
            if(!it->second->getOpt<sst::tf>("discontinued",false))
            {
               // ignore if already installed
               if(d.isInstalled(*it->second))
                  continue;

               iCount++;

               // schedule download
               pDownloads->children.push_back(
                  new fetchRecipe(d,*it->second));

               // schedule install
               pInstalls->children.push_back(
                  new installRecipe(d,*it->second));
            }

            // schedule uninstall of existing versions
            auto& prevVers
               = d.installedGuidsByProdName[(*it->second)["name"].as<sst::str>().get()];
            for(auto jit=prevVers.begin();jit!=prevVers.end();++jit)
               toUninstall.push_back(d.dictsByGuid[*jit]);
         }
      }

      // manage: defer-delete

      // for toUninstall
      for(auto it=toUninstall.begin();it!=toUninstall.end();++it)
      {
         iCount++;

         // push back: uninstall
         pUninstalls->children.push_back(
            new uninstallRecipe(d,**it));

         // push back: remove
         pRemoves->children.push_back(
            new unfetchRecipe(d,**it));
      }

      if(iCount == 0)
         d.log().writeLn("nothing to do");

      inflatingVisitor inflater;
      pMainR->acceptVisitor(inflater);

      return pMainR.abdicate();
   }

private:
   // Gordian is special in that it IS the installer.
   // To allow for changes in format, only let the immediately
   // previous gordian install the next gordian (i.e. have
   // gordian v2 install gordian v3 not gordian v1, so that
   // v3's install script can take advantage of new features in v2
   //
   // If a newer gordian is available, require that it's in the pattern
   // and do it first, alone.  Delegate further installs to this new
   // instance.
   //
   // This prohibits downgrading gordian, ensuring that only the
   // newest known gordian is the one that's running.  Because of that,
   // this problem doesn't apply to uninstall (though uninstall should
   // halt upon detecting a newer gordian)
   compositeRecipe *checkForSelfUpgrade(directory& d, const std::string& n, const std::string& v)
   {
      auto *pDictToInstall = subCuratorHelper::findNextGordian(d);
      if(!pDictToInstall)
         return NULL;

      if(!d.isNameMatch(*pDictToInstall,n) || v != "*")
         // this would be against the user's wishes, so let's not install but complain
         throw std::runtime_error(
            "a newer version of gordian is available; please install it first");

      d.log().writeLn("a newer gordian is available; upgrading that first");
      cmn::autoReleasePtr<compositeRecipe> pMainR(new compositeRecipe());
      pMainR->children.push_back(new fetchRecipe(d,*pDictToInstall));
      pMainR->children.push_back(new installRecipe(d,*pDictToInstall));
      pMainR->children.push_back(new delegateInstallRecipe(d,*pDictToInstall,n,v));

      return pMainR.abdicate();
   }
};

tcatExposeTypeAs(installCurator,iSubCurator);

} // namespace curator
