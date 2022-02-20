#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "recipes.hpp"
#include "subcurator.hpp"

namespace curator {

class listCurator : public iSubCurator {
public:
   virtual iRecipe *compile(directory& d, const iRequest& r)
   {
      if(r.getType() != iRequest::kList)
         return NULL;

      return new listRecipe(d);
   }
};

tcatExposeTypeAs(listCurator,iSubCurator);

} // namespace curator
