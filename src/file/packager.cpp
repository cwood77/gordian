#include "../tcatlib/api.hpp"
#include "packager.hpp"
#include <list>
#include <map>
#include <memory>

namespace file {

class packagerFacade : public iPackager {
public:
   virtual void release();
   virtual const char *pack(const char *pPath);
   virtual const char *unpack(const char *pPath);

   std::list<iPackagerSlice*> m_packStack;
};

class packagerDirectory {
public:
   void catalog(tcat::typeSet<iPackagerSlice>& slices);

   void compose(size_t flags, std::list<iPackagerSlice*>& packStack);

private:
   std::map<size_t,iPackagerSlice*> m_byPriority;
   std::map<size_t,iPackagerSlice*> m_byFlag;
};

class packagerFactoryImpl {
public:
   void tie(console::iLog& l, sst::dict& config);

   tcat::typeSet<iPackagerSlice> slices;
   packagerDirectory dir;
};

class packagerFactory : public iPackagerFactory {
public:
   virtual void tie(console::iLog& l, sst::dict& config) {}
   virtual iPackager& compose(size_t flags) { throw 0; }

private:
   std::unique_ptr<packagerFactoryImpl> m_pImpl;
};

tcatExposeTypeAs(packagerFactory,iPackagerFactory);

} // namespace file
