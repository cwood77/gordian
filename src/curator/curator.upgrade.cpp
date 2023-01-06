#include "../cmn/autoPtr.hpp"
#include "../console/log.hpp"
#include "../file/api.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "directory.hpp"
#include "recipes.hpp"
#include "subcurator.hpp"

namespace curator {

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
class upgradeCurator : public iSubCurator {
public:
   virtual void setGordianVersion(size_t v) { m_gVersion = v; }

   virtual void expandRequest(directory& d, std::list<request>& rs)
   {
      if(rs.size() == 0)
         return;
      if(rs.begin()->getType() == iRequest::kInstall)
      {
         // cancel install and schedule upgrade if a newer godian is available

         auto *pDictToInstall = subCuratorHelper::findNextGordian(d);
         if(!pDictToInstall)
            return;

         std::string n,v;
         d.parsePattern(*rs.begin(),n,v);
         if(n != "gordian" || v != "*")
            throw std::runtime_error(
               "a newer version of gordian is available; please install it first");

         request upgrade(iSubCurator::kUpgradeGordian,"","");
         rs.clear(); // toss all other requests!
         rs.push_front(upgrade);
      }
   }

   virtual recipeBase *compile(directory& d, const iRequest& r)
   {
      if(r.getType() == iSubCurator::kUpgradeGordian)
         return compile_upgrade(d,r);
      else if(r.getType() == iRequest::kUpgrade_InstallAndInvoke)
         return compile_upgradeAndInvoke(d,r);
      else if(r.getType() == iRequest::kUpgrade_UnfetchAndRecurse)
         return compile_upgradeAndRecurse(d,r);
      else
         return NULL;
   }

   size_t m_gVersion;

private:
   recipeBase *compile_upgrade(directory& d, const iRequest& r)
   {
      auto *pDictToInstall = subCuratorHelper::findNextGordian(d);
      if(!pDictToInstall)
         throw std::runtime_error("ISE");

      cmn::autoReleasePtr<compositeRecipe> pMainR(new compositeRecipe());
      pMainR->children.push_back(new fetchRecipe(d,*pDictToInstall));
      pMainR->children.push_back(new upgradeCallUnpackedRecipe(d,*pDictToInstall));

      inflatingVisitor inflater;
      pMainR->acceptVisitor(inflater);

      return pMainR.abdicate();
   }

   recipeBase *compile_upgradeAndInvoke(directory& d, const iRequest& r)
   {
      auto *pDictToInstall = d.dictsByGuid[r.getPattern()];
      if(!pDictToInstall)
         throw std::runtime_error("ISE");

      cmn::autoReleasePtr<compositeRecipe> pMainR(new compositeRecipe());
      pMainR->children.push_back(new installRecipe(d,*pDictToInstall));
      pMainR->children.push_back(new upgradeCallInstalledRecipe(d,*pDictToInstall));

      inflatingVisitor inflater;
      pMainR->acceptVisitor(inflater);

      return pMainR.abdicate();
   }

   recipeBase *compile_upgradeAndRecurse(directory& d, const iRequest& r)
   {
      auto *pDictToInstall = d.dictsByGuid[r.getPattern()];
      if(!pDictToInstall)
         throw std::runtime_error("ISE");

      cmn::autoReleasePtr<compositeRecipe> pMainR(new compositeRecipe());
      pMainR->children.push_back(new unfetchRecipe(d,*pDictToInstall));

      auto *pEvenMoreToUpgrade = subCuratorHelper::findNextGordian(d);
      if(pEvenMoreToUpgrade)
      {
         pMainR->children.push_back(new fetchRecipe(d,*pEvenMoreToUpgrade));
         pMainR->children.push_back(new upgradeCallUnpackedRecipe(d,*pEvenMoreToUpgrade));
      }

      inflatingVisitor inflater;
      pMainR->acceptVisitor(inflater);

      return pMainR.abdicate();
   }
};

tcatExposeTypeAs(upgradeCurator,iSubCurator);

} // namespace curator
