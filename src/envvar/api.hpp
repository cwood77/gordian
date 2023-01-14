#ifndef ___envvar_api___
#define ___envvar_api___

#include <string>

namespace console { class iLog; }

namespace envvar {

class iPathVar {
public:
   virtual ~iPathVar() {}

   virtual void tie(console::iLog& l) = 0;

   virtual void addToPath(const std::string& path) = 0;
   virtual void removeFromPath(const std::string& path) = 0;
};

} // namespace envvar

#endif // ___envvar_api___
