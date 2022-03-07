#ifndef ___archive_sign___
#define ___archive_sign___

#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../file/packager.hpp"
#include <list>
#include <string>
#include <windows.h>
#include <ncrypt.h>

namespace console { class iLog; }
namespace sst { class dict; }

namespace archive {

class signPackager : public file::iPackagerSlice {
public:
   signPackager() : m_pLog(NULL), m_pConfig(NULL) {}

   virtual size_t getPackPriority() const { return 3; }
   virtual size_t getFlag() const { return 1<<2; }

   virtual void tie(console::iLog& l, sst::dict& config)
   { m_pLog = &l; m_pConfig = &config; }

   virtual const char *pack(const char *pPath);
   virtual const char *unpack(const char *pPath);

private:
   console::iLog *m_pLog;
   sst::dict *m_pConfig;
   std::string m_stringCache;
};

} // namespace archive

#endif // ___archive_sign___
