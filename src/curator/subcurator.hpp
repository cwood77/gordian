#ifndef ___curator_subcurator___
#define ___curator_subcurator___

#include "api.hpp"
#include <list>

namespace sst { class dict; }

namespace curator {

class directory;
class recipeBase;

class iSubCurator {
public:
   enum moreTypes {
      kUpgradeGordian = iRequest::_kLast + 1,
      kDeferDeleteIf
   };

   virtual ~iSubCurator() {}
   virtual void setGordianVersion(size_t v) {}
   virtual void expandRequest(directory& d, std::list<request>& rs) {}
   virtual recipeBase *compile(directory& d, const iRequest& r) = 0;
};

class subCuratorHelper {
public:
   static sst::dict *findNextGordian(directory& d);
   static void requireLatestGordian(directory& d);
};

} // namespace curator

#endif // ___curator_subcurator___
