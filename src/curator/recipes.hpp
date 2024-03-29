#ifndef ___curator_recipes___
#define ___curator_recipes___

#include "../exec/api.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <list>

namespace sst { class dict; }

namespace curator {

class directory;

class compositeRecipe;
class listRecipe;
class fetchRecipe;
class unfetchRecipe;
class inflatableRecipe;
class installRecipe;
class uninstallRecipe;
class scheduleUninstallRecipe;
class upgradeCallUnpackedRecipe;
class upgradeCallInstalledRecipe;
class addToPathInstr;
class removeFromPathInstr;
class batchFileInstr;

class iRecipeVisitor {
public:
   virtual void visit(compositeRecipe& n) = 0;
   virtual void visit(listRecipe& n) = 0;
   virtual void visit(fetchRecipe& n) = 0;
   virtual void visit(unfetchRecipe& n) = 0;
   virtual void visit(inflatableRecipe& n) = 0;
   virtual void visit(installRecipe& n) = 0;
   virtual void visit(uninstallRecipe& n) = 0;
   virtual void visit(scheduleUninstallRecipe& n) = 0;
   virtual void visit(upgradeCallUnpackedRecipe& n) = 0;
   virtual void visit(upgradeCallInstalledRecipe& n) = 0;
   virtual void visit(addToPathInstr& n) = 0;
   virtual void visit(removeFromPathInstr& n) = 0;
   virtual void visit(batchFileInstr& n) = 0;
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

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }

   std::list<recipeBase*> children;
};

class directoryRecipe : public recipeBase {
protected:
   explicit directoryRecipe(directory& d) : m_d(d) {}

   directory& m_d;
};

class packageRecipe : public directoryRecipe {
protected:
   packageRecipe(directory& d, sst::dict& p)
   : directoryRecipe(d), m_package(p) {}

   sst::dict& m_package;
};

class listRecipe : public directoryRecipe {
public:
   explicit listRecipe(directory& d) : directoryRecipe(d) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }
};

class fetchRecipe : public packageRecipe {
public:
   fetchRecipe(directory& d, sst::dict& p) : packageRecipe(d,p) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }
};

class unfetchRecipe : public packageRecipe {
public:
   unfetchRecipe(directory& d, sst::dict& p) : packageRecipe(d,p) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }
};

class inflatableRecipe : public packageRecipe {
public:
   ~inflatableRecipe();

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }

   virtual void inflate() = 0;

   std::list<recipeBase*> children;

protected:
   inflatableRecipe(directory& d, sst::dict& p) : packageRecipe(d,p) {}
};

class installRecipe : public inflatableRecipe {
public:
   installRecipe(directory& d, sst::dict& p) : inflatableRecipe(d,p) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }

   virtual void inflate();
};

class uninstallRecipe : public inflatableRecipe {
public:
   uninstallRecipe(directory& d, sst::dict& p) : inflatableRecipe(d,p) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }

   virtual void inflate();
};

class scheduleUninstallRecipe : public packageRecipe {
public:
   scheduleUninstallRecipe(directory& d, sst::dict& p) : packageRecipe(d,p) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }
};

class upgradeCallingRecipe : public packageRecipe {
public:
   upgradeCallingRecipe(
      directory& d,
      sst::dict& p
   ) : packageRecipe(d,p) {}

   virtual void execute();

protected:
   virtual std::string getCalleePath() = 0;
   virtual std::string getExtraArg() = 0;
};

class upgradeCallUnpackedRecipe : public upgradeCallingRecipe {
public:
   upgradeCallUnpackedRecipe(
      directory& d,
      sst::dict& p
   ) : upgradeCallingRecipe(d,p) {}

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }

protected:
   virtual std::string getCalleePath();
   virtual std::string getExtraArg() { return "unpacked"; }
};

class upgradeCallInstalledRecipe : public upgradeCallingRecipe {
public:
   upgradeCallInstalledRecipe(
      directory& d,
      sst::dict& p
   ) : upgradeCallingRecipe(d,p) {}

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }

protected:
   virtual std::string getCalleePath();
   virtual std::string getExtraArg() { return "installed"; }
};

class instrBase : public packageRecipe {
public:
   virtual void config(sst::dict& c) {}
   virtual instrBase *invert() = 0;

protected:
   instrBase(directory& d, sst::dict& p) : packageRecipe(d,p) {}
};

class addToPathInstr : public instrBase {
public:
   addToPathInstr(directory& d, sst::dict& p) : instrBase(d,p) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }

   virtual void config(sst::dict& c);
   virtual instrBase *invert();

   // this is public for inversion
   std::string m_path;
};

class removeFromPathInstr : public instrBase {
public:
   removeFromPathInstr(directory& d, sst::dict& p) : instrBase(d,p) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }

   virtual instrBase *invert();

   // this is public for inversion
   std::string m_path;
};

class batchFileInstr : public instrBase {
public:
   batchFileInstr(directory& d, sst::dict& p) : instrBase(d,p), m_install(true) {}

   virtual void execute();

   virtual void acceptVisitor(iRecipeVisitor& v) { v.visit(*this); }

   virtual void config(sst::dict& c);

   virtual instrBase *invert();

private:
   tcat::typePtr<exec::iScriptRunner> m_pScript;
   bool m_install;
   std::string m_scriptPath;
};

class hierRecipeVisitor : public recipeVisitorBase {
public:
   virtual void visit(listRecipe& n) {}
   virtual void visit(fetchRecipe& n) {}
   virtual void visit(unfetchRecipe& n) {}
   virtual void visit(inflatableRecipe& n) {}
   virtual void visit(installRecipe& n) { visit(static_cast<inflatableRecipe&>(n)); }
   virtual void visit(uninstallRecipe& n) { visit(static_cast<inflatableRecipe&>(n)); }
   virtual void visit(scheduleUninstallRecipe& n) {}
   virtual void visit(upgradeCallUnpackedRecipe& n) {}
   virtual void visit(upgradeCallInstalledRecipe& n) {}
   virtual void visit(addToPathInstr& n) {}
   virtual void visit(removeFromPathInstr& n) {}
   virtual void visit(batchFileInstr& n) {}
};

class inflatingVisitor : public hierRecipeVisitor {
public:
   virtual void visit(inflatableRecipe& n) { n.inflate(); }
};

} // namespace curator

#endif // ___curator_recipes___
