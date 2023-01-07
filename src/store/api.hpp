#ifndef ___store_api___
#define ___store_api___

namespace sst { class dict; }
namespace console { class iLog; }

namespace store {

class iCurrentStore {
public:
   virtual ~iCurrentStore() {}

   virtual void initConfiguration(sst::dict& d) const = 0;

   virtual bool tryActivate(
      sst::dict& d,
      const std::string& name,
      std::set<std::string>& ans) const = 0;
};

class iStore {
public:
   virtual ~iStore() {}

   virtual void loadConfiguration(sst::dict& d, console::iLog& l) = 0;
   virtual iStore *upgradeIf() = 0;

   // populate may actually download, unzip, etc., but prediction just
   // does path computation

   virtual const char *predictPackagePath(const char *pPackageName) = 0;

   virtual const char *populateManifests() = 0;
   virtual void depopulateManifests() = 0;
   virtual const char *populatePackage(const char *pPackageName) = 0;
   virtual void depopulatePackage(const char *pPackageName) = 0;

   virtual void command(const std::vector<std::string>& args) = 0;
};

} // namespace store

#endif // ___store_api___
