#define WIN32_LEAN_AND_MEAN
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
   }

   void loadManifest(const std::string& manifest)
   {
   }

   console::iLog *m_pLog;
   sst::dict *m_pDict;
   std::map<std::string,size_t> m_latest;
   std::map<std::string,size_t> m_current;
   std::map<std::string,std::map<size_t,sst::dict*> > m_directory;
};

tcatExposeTypeAs(curator,iCurator);

} // namespace curator

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
