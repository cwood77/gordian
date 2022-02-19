#ifndef ___curator_recipe_unfetch___
#define ___curator_recipe_unfetch___

#include "api.hpp"

#include "../console/log.hpp"
#include "../file/api.hpp"
#include "directory.hpp"

namespace curator {

class directory;

class unfetchRecipe : public iRecipe {
public:
   unfetchRecipe(
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
         "running unfetch %s:%d",
         m_package["name"].as<sst::str>().get().c_str(),
         m_package["version"].as<sst::mint>().get());
   }

private:
   const directory& m_d;
   sst::dict& m_package;
};

} // namespace curator

#endif // ___curator_recipe_unfetch___
