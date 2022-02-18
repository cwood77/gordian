#ifndef ___curator_api___
#define ___curator_api___

#include <string>

namespace console { class iLog; }
namespace file { class iFileManager; }
namespace sst { class dict; }

namespace curator {

class iRequest {
public:
   enum types {
      kInstall,
      kUpgrade,
      kUninstall,
      kList
   };

   virtual types getType() const = 0;
   virtual const char *getPattern() const = 0;
   virtual bool getReally() const = 0;
};

class request : public iRequest {
public:
   request(types t, const std::string& p, bool r)
   : m_type(t), m_pattern(p), m_really(r) {}

   virtual types getType() const { return m_type; }
   virtual const char *getPattern() const { return m_pattern.c_str(); }
   virtual bool getReally() const { return m_really; }

private:
   types m_type;
   std::string m_pattern;
   bool m_really;
};

class iRecipe {
public:
   virtual ~iRecipe() {}
   virtual void release() = 0;
   virtual void execute() = 0;
};

class iCurator {
public:
   virtual ~iCurator() {}
   virtual void tie(console::iLog& l, sst::dict& config) = 0;
   virtual iRecipe *compile(const char *manifestFolder, const iRequest& r) = 0;
};

} // namespace curator

#endif // ___curator_api___
