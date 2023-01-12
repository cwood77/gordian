#ifndef ___archive_sign___
#define ___archive_sign___

// some crypto functionality requires the full enchilada
// #define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../file/packager.hpp"
#include <list>
#include <string>
#include <windows.h>
#include <ncrypt.h>

namespace console { class iLog; }
namespace file { class iFileManager; }
namespace sst { class dict; }

namespace archive {

class autoAlgorithmProvider {
public:
   autoAlgorithmProvider() : h(0) {}
   ~autoAlgorithmProvider();

   void openForHash();
   void openForSign();

   unsigned long getObjectLength();
   unsigned long getHashLength();

   BCRYPT_ALG_HANDLE h;

private:
   unsigned long getDword(const wchar_t *pProperty);
};

class autoHash {
public:
   autoHash() : h(0), m_pAlgProv(NULL) {}
   ~autoHash();

   void createHash(autoAlgorithmProvider& p);
   void hashData(char *pBlock, size_t blockSize);
   void finish(cmn::sizedAlloc& result);

   BCRYPT_HASH_HANDLE h;

private:
   cmn::sizedAlloc m_internalHashMem;
   autoAlgorithmProvider *m_pAlgProv;
};

class autoKeyStorage {
public:
   autoKeyStorage();
   ~autoKeyStorage();

   NCRYPT_PROV_HANDLE hProv;
};

class autoKey {
public:
   static const wchar_t *kSignKeyName;
   static const char *kPubKeyFileName;

   static const char *getPubKeyFilePath(file::iFileManager& f);

   autoKey() : m_pStor(NULL), k(0) {}
   ~autoKey();

   void createForSign(autoKeyStorage& s, const wchar_t *pName);
   bool tryOpen(autoKeyStorage& s, const wchar_t *pName);
   void open(autoKeyStorage& s, const wchar_t *pName);
   void erase();
   void exportToBlob(cmn::sizedAlloc& mem);
   void importFromBlob(autoKeyStorage& s, cmn::sizedAlloc& mem);

   autoKeyStorage *m_pStor;
   NCRYPT_KEY_HANDLE k;
};

class keyIterator {
public:
   keyIterator();
   ~keyIterator();

   void start(autoKeyStorage& s);
   bool isDone();
   void advance();
   std::wstring& getName();

private:
   void finish();

   autoKeyStorage *m_pStorage;
   NCryptKeyName *m_pCurrent;
   void *m_pState;
   std::wstring m_nameCache;
};

class signature {
public:
   static void signBlock(
      autoKey& k,
      cmn::sizedAlloc& block,
      cmn::sizedAlloc& result);

   static void verifySignature(
      autoKey& k,
      cmn::sizedAlloc& block,
      cmn::sizedAlloc& signature);
};

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
