#ifndef ___archive_compress___
#define ___archive_compress___

#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../file/packager.hpp"
#include <compressapi.h>
#include <list>
#include <string>
#include <windows.h>

namespace console { class iLog; }
namespace sst { class dict; }

namespace archive {

class compressor {
public:
   compressor() : han(0) {}
   virtual ~compressor();

   void run(cmn::autoCFilePtr& src, cmn::autoCFilePtr& dest);

   COMPRESSOR_HANDLE han;
};

class decompressor {
public:
   decompressor() : han(0) {}
   virtual ~decompressor();

   void run(cmn::autoCFilePtr& src, cmn::autoCFilePtr& dest);

   COMPRESSOR_HANDLE han;
};

class compressSetting {
public:
   virtual ~compressSetting();

   void load(cmn::autoCFilePtr& f);
   void save(cmn::autoCFilePtr& f);

   unsigned long flag;
   unsigned long valueSize;
   char *pValue;
};

class compressHeader {
public:
   ~compressHeader();

   void load(cmn::autoCFilePtr& f);
   void save(cmn::autoCFilePtr& f);

   compressor *createCompressor();
   decompressor *createDecompressor();

   unsigned long alg;
   std::list<compressSetting*> settings;
};

class compressPackager : public file::iPackagerSlice {
public:
   compressPackager() : m_pLog(NULL), m_pConfig(NULL) {}

   virtual size_t getPackPriority() const { return 2; }
   virtual size_t getFlag() const { return 1<<1; }

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

#endif // ___archive_compress___
