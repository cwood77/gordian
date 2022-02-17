#ifndef ___store_api___
#define ___store_api___

namespace store {

class iCurrentStore {
public:
   virtual ~iCurrentStore() {}

   virtual void initConfiguration(sst::dict& d) const = 0;
};

class iStore {
public:
   virtual ~iStore() {}

   virtual void loadConfiguration(sst::dict& d) = 0;
   virtual iStore *upgradeIf() = 0;

   virtual const char *populateManifests() = 0;
   virtual const char *populatePackage(const char *pPackageName) = 0;
};

} // namespace store

#endif // ___store_api___
