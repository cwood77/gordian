#ifndef ___curator_instr___
#define ___curator_instr___

#include <list>
#include <string>

namespace sst { class dict; }

namespace curator {

class directory;
class recipeBase;

class instrBuilder {
public:
   instrBuilder(directory& d, std::list<recipeBase*>& children) : m_d(d), m_children(children) {}

   void populate(sst::dict& p, bool install);

private:
   void populateInstall(sst::dict& p);
   void populateDefaultInstall(sst::dict& p);
   void populateUninstall(sst::dict& p);
   void populateDefaultUninstall(sst::dict& p);
   void invertInstrs();
   void populateInstrs(sst::dict& p, const std::string& key, bool install);

   directory& m_d;
   std::list<recipeBase*>& m_children;
};

} // namespace curator

#endif // ___curator_instr___
