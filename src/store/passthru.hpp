#ifndef ___store_passthru___
#define ___store_passthru___

#include "api.hpp"

namespace sst { class dict; }

namespace store {

class passthruStore : public iStore, public iCurrentStore {
public:
   passthruStore() : m_pLog(NULL), m_pRootSettings(NULL), m_pMySettings(NULL) {}

   virtual void initConfiguration(sst::dict& d) const;

   virtual void loadConfiguration(sst::dict& d, console::iLog& l);
   virtual iStore *upgradeIf();
   virtual const char *predictPackagePath(const char *pPackageName);
   virtual const char *populateManifests();
   virtual void depopulateManifests() {}
   virtual const char *populatePackage(const char *pPackageName);
   virtual void depopulatePackage(const char *pPackageName) {}

private:
   sst::dict& settings() { return *m_pMySettings; }

   console::iLog *m_pLog;
   sst::dict *m_pRootSettings;
   sst::dict *m_pMySettings;

   std::string m_strCache;
};


} // namespace store


#endif // ___store_passthru___
