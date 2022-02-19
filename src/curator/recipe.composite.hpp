#ifndef ___curator_recipe_composite___
#define ___curator_recipe_composite___

#include "api.hpp"
#include <list>

namespace curator {

class directory;

class compositeRecipe : public iRecipe {
public:
   ~compositeRecipe();

   virtual void release() { delete this; }

   virtual void execute();

   std::list<iRecipe*> children;
};

} // namespace curator

#endif // ___curator_recipe_composite___
