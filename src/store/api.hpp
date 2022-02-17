#ifndef ___store_api___
#define ___store_api___

namespace store {

class iStore {
public:
   virtual ~iStore() {}
   virtual void setPaths(const char *pManifestFolder, const char *pPackageFolder) = 0;
   virtual void loadConfiguration(sst::dict& d) = 0;
   virtual iStore *upgradeIf() = 0;

   virtual void populateManifests() = 0;
   virtual void populatePackage(const char *pPackageName) = 0;

   virtual void saveConfiguration(sst::dict& d) = 0;
};

} // namespace store

#endif // ___store_api___
