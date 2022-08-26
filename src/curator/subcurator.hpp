#ifndef ___curator_subcurator___
#define ___curator_subcurator___

namespace sst { class dict; }

namespace curator {

class directory;
class iRecipe;
class iRequest;

class iSubCurator {
public:
   virtual ~iSubCurator() {}
   virtual iRecipe *compile(directory& d, const iRequest& r) = 0;
};

class subCuratorHelper {
public:
   static sst::dict *findNextGordian(directory& d);
   static void requireLatestGordian(directory& d);
};

} // namespace curator

#endif // ___curator_subcurator___
