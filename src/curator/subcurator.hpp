#ifndef ___curator_subcurator___
#define ___curator_subcurator___

namespace curator {

class directory;
class iRecipe;
class iRequest;

class iSubCurator {
public:
   virtual ~iSubCurator() {}
   virtual iRecipe *compile(directory& d, const iRequest& r) = 0;
};

} // namespace curator

#endif // ___curator_subcurator___
