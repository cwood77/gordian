#ifndef ___curator_instr___
#define ___curator_instr___

#include <list>

namespace sst { class dict; }

namespace curator {

class recipeBase;

class instrBuilder {
public:
   explicit instrBuilder(std::list<recipeBase*>& children) : m_children(children) {}

   void populate(sst::dict& d, bool install);

private:
   std::list<recipeBase*>& m_children;
};

} // namespace curator

#endif // ___curator_instr___
