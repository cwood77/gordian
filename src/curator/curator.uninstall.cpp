#include "../cmn/autoPtr.hpp"
#include "../console/log.hpp"
#include "../file/api.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "directory.hpp"
#include "recipes.hpp"
#include "subcurator.hpp"

namespace curator {

class uninstallCurator : public iSubCurator {
public:
   virtual recipeBase *compile(directory& d, const iRequest& r)
   {
      if(r.getType() != iRequest::kUninstall)
         return NULL;

      subCuratorHelper::requireLatestGordian(d);

      std::string n,v;
      d.parsePattern(r,n,v);
      if(n == "gordian")
         throw std::runtime_error("gordian can't be uninstalled via gordian");

      size_t iCount = 0;
      cmn::autoReleasePtr<compositeRecipe> pMainR(new compositeRecipe());
      auto pUninstalls = new compositeRecipe();
      auto pRemoves = new compositeRecipe();
      pMainR->children.push_back(pUninstalls);
      pMainR->children.push_back(pRemoves);

      // uninstall everything requested
      for(auto it=d.installedGuidsSorted.begin();it!=d.installedGuidsSorted.end();++it)
      {
         for(auto jit=it->second.begin();jit!=it->second.end();++jit)
         {
            auto guid = directory::calcManifestGuid(it->first,*jit);
            sst::dict& dict = *d.dictsByGuid[guid];

            if(!d.isMatch(dict,n,v))
               continue;

            if(d.isNameMatch(dict,"gordian"))
               continue; // skip gordians

            iCount++;

            // schedule uninstall
            pUninstalls->children.push_back(
               new uninstallRecipe(d,dict));

            // schedule remove
            pRemoves->children.push_back(
               new unfetchRecipe(d,dict));
         }
      }

      if(iCount == 0)
         d.log().writeLn("nothing to uninstall");

      inflatingVisitor inflater;
      pMainR->acceptVisitor(inflater);

      return pMainR.abdicate();
   }
};

tcatExposeTypeAs(uninstallCurator,iSubCurator);

} // namespace curator
