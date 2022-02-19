#ifndef ___curator_recipe_fetch___
#define ___curator_recipe_fetch___

#include "api.hpp"

#include "../console/log.hpp"
#include "../file/api.hpp"
#include "directory.hpp"

namespace curator {

class directory;

class fetchRecipe : public iRecipe {
public:
   fetchRecipe(
      const directory& d,
      sst::dict& p
   )
   : m_d(d)
   , m_package(p)
   {}

   virtual void release() { delete this; }

   virtual void execute()
   {
      m_d.log().writeLn(
         "running fetch %s:%d",
         m_package["name"].as<sst::str>().get().c_str(),
         m_package["version"].as<sst::mint>().get());
   }

private:
   const directory& m_d;
   sst::dict& m_package;
};

} // namespace curator

#endif // ___curator_recipe_fetch___
