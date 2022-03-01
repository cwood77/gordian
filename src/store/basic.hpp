#ifndef ___store_basic___
#define ___store_basic___

#include "api.hpp"

namespace sst { class dict; }

namespace store {

class basicStore : public iStore, public iCurrentStore {
public:
   basicStore() : m_pLog(NULL), m_pRootSettings(NULL), m_pMySettings(NULL) {}

   virtual void initConfiguration(sst::dict& d) const;

   virtual void loadConfiguration(sst::dict& d, console::iLog& l);
   virtual iStore *upgradeIf();
   virtual const char *populateManifests();
   virtual const char *populatePackage(const char *pPackageName);

private:
   basicStore(const basicStore& other);

   sst::dict& settings() { return *m_pMySettings; }

   console::iLog *m_pLog;
   sst::dict *m_pRootSettings;
   sst::dict *m_pMySettings;

   std::string m_strCache;
};


} // namespace store


#endif // ___store_basic___
