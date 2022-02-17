#ifndef ___curator_api___
#define ___curator_api___

namespace console { class iLog; }
namespace file { class iFileManager; }
namespace sst { class dict; }

namespace curator {

class iRecipe {
public:
   virtual void release() = 0;

   virtual void execute(file::iFileManager& f, console::iLog& l) = 0;
};

class iCurator {
public:
   virtual ~iCurator() {}

   virtual void tie(console::iLog& l, sst::dict& config) = 0;

   iRecipe *compile(const char *manifestFolder) = 0;
};

} // namespace curator

#endif // ___curator_api___
