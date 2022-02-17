#ifndef ___store_basic___
#define ___store_basic___

#include "api.hpp"

namespace sst { class dict; }

namespace store {

class basicStore : public iStore {
public:
   basicStore() : m_pRootSettings(NULL), m_pMySettings(NULL) {}

   virtual void setPaths(const char *pManifestFolder, const char *pPackageFolder);
   virtual void loadConfiguration(sst::dict& d);
   virtual iStore *upgradeIf();
   virtual const char *populateManifests();
   virtual const char *populatePackage(const char *pPackageName);

private:
   basicStore(const basicStore& other);

   sst::dict& settings() { return *m_pMySettings; }

   std::string m_manifestFolder;
   std::string m_packageFolder;
   sst::dict *m_pRootSettings;
   sst::dict *m_pMySettings;
};


} // namespace store


#endif // ___store_basic___
