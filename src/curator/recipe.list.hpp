#ifndef ___curator_recipe_list___
#define ___curator_recipe_list___

#include "api.hpp"

namespace curator {

class directory;

class listRecipe : public iRecipe {
public:
   explicit listRecipe(
      const directory& d
   )
   : m_d(d)
   {}

   virtual void release() { delete this; }

   virtual void execute();

private:
   const directory& m_d;
};

} // namespace curator

#endif // ___curator_recipe_list___
