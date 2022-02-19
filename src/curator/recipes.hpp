#ifndef ___curator_recipes___
#define ___curator_recipes___

#include "api.hpp"
#include <list>

namespace sst { class dict; }

namespace curator {

class directory;

class compositeRecipe;
class listRecipe;
class fetchRecipe;
class unfetchRecipe;
class installRecipe;
class uninstallRecipe;

class iRecipeVisitor {
public:
   virtual void visit(compositeRecipe& n) = 0;
   virtual void visit(listRecipe& n) = 0;
   virtual void visit(fetchRecipe& n) = 0;
   virtual void visit(unfetchRecipe& n) = 0;
   virtual void visit(installRecipe& n) = 0;
   virtual void visit(uninstallRecipe& n) = 0;
};

class recipeVisitorBase : public iRecipeVisitor {
public:
   virtual void visit(compositeRecipe& n);
};

class recipeBase : public iRecipe {
public:
   virtual void release() { delete this; }

   virtual void acceptVisitor(iRecipeVisitor& v) = 0;

protected:
   recipeBase() {}
};

class compositeRecipe : public recipeBase {
public:
   ~compositeRecipe();

   virtual void release() { delete this; }

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }

   std::list<recipeBase*> children;
};

class directoryRecipe : public recipeBase {
protected:
   explicit directoryRecipe(const directory& d) : m_d(d) {}

   const directory& m_d;
};

class packageRecipe : public directoryRecipe {
protected:
   packageRecipe(const directory& d, sst::dict& p)
   : directoryRecipe(d), m_package(p) {}

   sst::dict& m_package;
};

class listRecipe : public directoryRecipe {
public:
   explicit listRecipe(const directory& d) : directoryRecipe(d) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }
};

class fetchRecipe : public packageRecipe {
public:
   fetchRecipe(const directory& d, sst::dict& p) : packageRecipe(d,p) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }
};

class unfetchRecipe : public packageRecipe {
public:
   unfetchRecipe(const directory& d, sst::dict& p) : packageRecipe(d,p) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }
};

class installRecipe : public packageRecipe {
public:
   installRecipe(const directory& d, sst::dict& p) : packageRecipe(d,p) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }
};

class uninstallRecipe : public packageRecipe {
public:
   uninstallRecipe(const directory& d, sst::dict& p) : packageRecipe(d,p) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }
};

} // namespace curator

#endif // ___curator_recipes___
