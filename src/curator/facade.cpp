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
      m_d.loadAllManifestsIf(manifestFolder);
      m_d.categorizeInstalled();
      tcat::typeSet<iSubCurator> subs;

      std::list<request> expanded;
      expanded.push_front(dynamic_cast<const request&>(r));
      for(size_t i=0;i<subs.size();i++)
      {
         iSubCurator *pS = subs[i];
         pS->expandRequest(m_d,expanded);
      }

      cmn::autoReleasePtr<compositeRecipe> pMainR(new compositeRecipe());
      for(auto& r : expanded)
      {
         for(size_t i=0;i<subs.size();i++)
         {
            iSubCurator *pS = subs[i];
            auto *pR = pS->compile(m_d,r);
            pMainR->children.push_back(pR);
         }
         if(pMainR->children.size() == 0)
            throw std::runtime_error("unimpled request");
      }

      return pMainR.abdicate();
   }

private:
   directory m_d;
};

tcatExposeTypeAs(facade,iCurator);

} // namespace curator

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
