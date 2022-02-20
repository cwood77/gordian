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

      // build a look-up map of installed products
      std::map<std::string,sst::dict*> installed;
      std::map<std::string,std::set<std::string> > installedVersions;
      {
         auto& L = d.config()["installed"].as<sst::array>();
         for(size_t i=0;i<L.size();i++)
         {
            auto& dict = L[i].as<sst::dict>();
            auto guid = directory::calcFullName(dict);
            installed[guid] = &dict;
            installedVersions[dict["name"].as<sst::str>().get()].insert(guid);
         }
      }

      // install everything requested
      std::list<sst::dict*> toUninstall;
      for(auto it=d.flat.begin();it!=d.flat.end();++it)
      {
         if(d.isMatch(**it,n,v))
         {
            // ignore if obsolete
            if((*it)->getOpt<sst::tf>("discontinued",false))
               continue;

            // ignore if already installed
            auto guid = directory::calcFullName(**it);
            if(installed.find(guid) != installed.end())
               continue;

            iCount++;

            // schedule download
            pDownloads->children.push_back(
               new fetchRecipe(d,**it));

            // schedule install
            pInstalls->children.push_back(
               new installRecipe(d,**it));

            // schedule uninstall of previous versions
            auto& prevVers = installedVersions[(**it)["name"].as<sst::str>().get()];
            for(auto jit=prevVers.begin();jit!=prevVers.end();++jit)
               toUninstall.push_back(installed[*jit]);
         }
      }

      // manage: defer-delete

      // for toUninstall
      for(auto it=toUninstall.begin();it!=toUninstall.end();++it)
      {
         // push back: uninstall
         pUninstalls->children.push_back(
            new uninstallRecipe(d,**it));

         // push back: remove
         pRemoves->children.push_back(
            new unfetchRecipe(d,**it));
      }

      if(iCount == 0)
         d.log().writeLn("nothing to install");

      return pMainR.abdicate();
   }
};

tcatExposeTypeAs(installCurator,iSubCurator);

} // namespace curator

