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

      tcat::typeSet<iSubCurator> subs;
      for(size_t i=0;i<subs.size();i++)
      {
         iSubCurator *pS = subs[i];
         iRecipe *pR = pS->compile(m_d,r);
         if(pR)
            return pR;
      }

      throw std::runtime_error("unimpled request");
   }

private:
   directory m_d;
};

tcatExposeTypeAs(facade,iCurator);

} // namespace curator

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
