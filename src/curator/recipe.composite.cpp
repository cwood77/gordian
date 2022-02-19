#include "recipe.composite.hpp"

namespace curator {

compositeRecipe::~compositeRecipe()
{
   auto it = children.begin();
   for(;it!=children.end();++it)
      delete *it;
}

void compositeRecipe::execute()
{
   auto it = children.begin();
   for(;it!=children.end();++it)
      (*it)->execute();
}

} // namespace curator
