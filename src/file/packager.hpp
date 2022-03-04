#ifndef ___file_packager___
#define ___file_packager___

#include <cstddef>

namespace console { class iLog; }
namespace sst { class dict; }

namespace file {

class iPackagerSlice {
public:
   virtual ~iPackagerSlice() {}

   virtual size_t getPackPriority() const = 0;
   virtual size_t getFlag() const = 0;
   virtual void tie(console::iLog& l, sst::dict& config) = 0;

   virtual const char *pack(const char *pPath) = 0;
   virtual const char *unpack(const char *pPath) = 0;
};

class iPackager {
public:
   virtual ~iPackager() {}

   virtual void release() = 0;

   virtual const char *pack(const char *pPath) = 0;
   virtual const char *unpack(const char *pPath) = 0;
};

class iPackagerFactory {
public:
   virtual ~iPackagerFactory() {}

   virtual void tie(console::iLog& l, sst::dict& config) = 0;

   virtual iPackager& compose(size_t flags) = 0;
};

} // namespace file

#endif // ___file_packager___
