#ifndef ___archive_sign___
#define ___archive_sign___

#include "../file/packager.hpp"
#include <string>

namespace console { class iLog; }
namespace sst { class dict; }

namespace archive {

class splitPackager : public file::iPackagerSlice {
public:
   splitPackager() : m_pLog(NULL), m_pConfig(NULL) {}

   virtual size_t getPackPriority() const { return 4; }
   virtual size_t getFlag() const { return 1<<3; }

   virtual void tie(console::iLog& l, sst::dict& config)
   { m_pLog = &l; m_pConfig = &config; }

   virtual const char *pack(const char *pPath);
   virtual const char *unpack(const char *pPath);

private:
   long getMaxSizeInBytes() const { return 5000 * 1024; } // 5MB

   console::iLog *m_pLog;
   sst::dict *m_pConfig;
   std::string m_stringCache;
};

} // namespace archive

#endif // ___archive_sign___
