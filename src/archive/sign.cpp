#include "../cmn/autoPtr.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include "sign.hpp"
#include <sstream>
#include <stdexcept>

namespace archive {

autoAlgorithmProvider::~autoAlgorithmProvider()
{
   ::BCryptCloseAlgorithmProvider(h,0);
}

void autoAlgorithmProvider::openForHash()
{
   NTSTATUS failed = ::BCryptOpenAlgorithmProvider(
      &h,
      BCRYPT_SHA256_ALGORITHM,
      NULL,
      0);
   if(failed)
      throw std::runtime_error("failed to open hash algorithm");
}

unsigned long autoAlgorithmProvider::getObjectLength()
{
   return getDword(BCRYPT_OBJECT_LENGTH);
}

unsigned long autoAlgorithmProvider::getHashLength()
{
   return getDword(BCRYPT_HASH_LENGTH);
}

unsigned long autoAlgorithmProvider::getDword(const wchar_t *pProperty)
{
   DWORD ans = 0;
   ULONG outSize = 0;

   NTSTATUS failed = ::BCryptGetProperty(
      h,
      pProperty,
      reinterpret_cast<PUCHAR>(&ans),
      sizeof(DWORD),
      &outSize,
      0);

   if(failed)
      throw std::runtime_error("failed to query property");
   if(outSize != sizeof(DWORD))
      throw std::runtime_error("unexpected size of property");

   return ans;
}

autoHash::~autoHash()
{
   ::BCryptDestroyHash(h);
}

void autoHash::createHash(autoAlgorithmProvider& p)
{
   m_pAlgProv = &p;
   m_internalHashMem.realloc(m_pAlgProv->getObjectLength());

   auto errors = ::BCryptCreateHash(
      m_pAlgProv->h,
      &h,
      reinterpret_cast<PUCHAR>(m_internalHashMem.ptr()),
      m_internalHashMem.size(),
      NULL,
      0,
      0);
   if(errors)
      throw std::runtime_error("error creating hash");
}

void autoHash::hashData(char *pBlock, size_t blockSize)
{
   auto errors = ::BCryptHashData(
      h,
      reinterpret_cast<PUCHAR>(pBlock),
      blockSize,
      0);
   if(errors)
      throw std::runtime_error("error performing hash");
}

void autoHash::finish(cmn::sizedAlloc& result)
{
   result.realloc(m_pAlgProv->getHashLength());

   auto errors = ::BCryptFinishHash(
      h,
      reinterpret_cast<PUCHAR>(result.ptr()),
      result.size(),
      0);
   if(errors)
      throw std::runtime_error("failed to finalize hash");
}

autoKeyStorage::autoKeyStorage()
{
   SECURITY_STATUS errors = ::NCryptOpenStorageProvider(
      &hProv,
      NULL,
      0);
   if(errors)
      throw std::runtime_error("failed to open default key storage");
}

autoKeyStorage::~autoKeyStorage()
{
   ::NCryptFreeObject(hProv);
}

const wchar_t *autoKey::kSignKeyName = L"cdwe gordian package signature";

autoKey::~autoKey()
{
   ::NCryptFreeObject(k);
}

void autoKey::createForSign(autoKeyStorage& s, const wchar_t *pName)
{
   m_pStor = &s;

   SECURITY_STATUS errors = ::NCryptCreatePersistedKey(
      m_pStor->hProv,
      &k,
      NCRYPT_ECDSA_P256_ALGORITHM,
      pName,
      0,
      0
   );

   if(errors)
      throw std::runtime_error("error creating persisted key 1");

   errors = ::NCryptFinalizeKey(
      k,
      NCRYPT_SILENT_FLAG
   );

   if(errors)
      throw std::runtime_error("error creating persisted key 2");
}

bool autoKey::tryOpen(autoKeyStorage& s, const wchar_t *pName)
{
   m_pStor = &s;

   SECURITY_STATUS errors = ::NCryptOpenKey(
      m_pStor->hProv,
      &k,
      pName,
      0,
      NCRYPT_SILENT_FLAG
   );

   if(errors == NTE_BAD_KEYSET)
      return false;
   else if(errors == 0)
      ;
   else
      throw std::runtime_error("error opening key");

   return true;
}

void autoKey::open(autoKeyStorage& s, const wchar_t *pName)
{
   bool success = tryOpen(s,pName);
   if(!success)
      throw std::runtime_error("failed to open key");
}

void autoKey::erase()
{
   SECURITY_STATUS errors = ::NCryptDeleteKey(
      k,
      NCRYPT_SILENT_FLAG
   );

   if(errors)
      throw std::runtime_error("failed to delete key");
}

void autoKey::exportToBlob(cmn::sizedAlloc& mem)
{
   throw std::runtime_error("unimpled");

   std::unique_ptr<BCRYPT_DSA_KEY_BLOB> pBlob(new BCRYPT_DSA_KEY_BLOB);

   DWORD cbResult = 0;
   SECURITY_STATUS errors = ::NCryptExportKey(
      k,
      0,
      BCRYPT_DSA_PUBLIC_BLOB,
      NULL,
      reinterpret_cast<PBYTE>(pBlob.get()),
      sizeof(BCRYPT_DSA_KEY_BLOB),
      &cbResult,
      NCRYPT_SILENT_FLAG
   );
   if(errors)
      throw std::runtime_error("error exporting key");

   mem.commandeer(pBlob);
}

keyIterator::keyIterator()
: m_pStorage(NULL), m_pCurrent(NULL), m_pState(NULL)
{
}

keyIterator::~keyIterator()
{
   finish();
}

void keyIterator::start(autoKeyStorage& s)
{
   finish();
   m_pStorage = &s;
   advance();
}

bool keyIterator::isDone()
{
   return m_pStorage == NULL;
}

void keyIterator::advance()
{
   SECURITY_STATUS error = ::NCryptEnumKeys(
      m_pStorage->hProv,
      NULL,
      &m_pCurrent,
      &m_pState,
      NCRYPT_SILENT_FLAG
   );
   if(error == /*NTE_NO_MORE_ITEMS*/(SECURITY_STATUS)0x8009002A)
      finish();
   else if(error == ERROR_SUCCESS)
      ;
   else
      throw std::runtime_error("error enumerating keys");
}

std::wstring& keyIterator::getName()
{
   std::wstringstream stream;
   stream
      << m_pCurrent->pszName
      << L" (alg:"
      << m_pCurrent->pszAlgid
      << L")"
   ;

   DWORD flags = m_pCurrent->dwLegacyKeySpec;
   if(flags & AT_KEYEXCHANGE)
   {
      flags &= ~AT_KEYEXCHANGE;
      stream << L" [AT_KEYEXCHANGE]";
   }
   if(flags & AT_SIGNATURE)
   {
      flags &= ~AT_SIGNATURE;
      stream << L" [AT_SIGNATURE]";
   }
   if(flags)
   {
      stream << L" [unknown legacy key spec?: " << flags << L"]";
   }

   flags = m_pCurrent->dwFlags;
   if(flags & NCRYPT_MACHINE_KEY_FLAG)
   {
      flags &= ~NCRYPT_MACHINE_KEY_FLAG;
      stream << L" [NCRYPT_MACHINE_KEY_FLAG]";
   }
   if(flags)
   {
      stream << L" [unknown flags?: " << flags << L"]";
   }

   m_nameCache = stream.str();
   return m_nameCache;
}

void keyIterator::finish()
{
   if(!m_pStorage)
      return;

   ::NCryptFreeBuffer(m_pCurrent);
   ::NCryptFreeBuffer(m_pState);

   m_pStorage = NULL;
   m_pCurrent = NULL;
   m_pState = NULL;
}

void signature::signBlock(autoKey& k, cmn::sizedAlloc& block, cmn::sizedAlloc& result)
{
   DWORD sizeToAlloc = 0;
   SECURITY_STATUS errors = ::NCryptSignHash(
      k.k,
      NULL,
      reinterpret_cast<PBYTE>(block.ptr()),
      block.size(),
      NULL,
      0,
      &sizeToAlloc,
      NCRYPT_SILENT_FLAG
   );
   if(errors)
      throw std::runtime_error("failed to determine sign size");

   result.realloc(sizeToAlloc);

   sizeToAlloc = 0;
   errors = ::NCryptSignHash(
      k.k,
      NULL,
      reinterpret_cast<PBYTE>(block.ptr()),
      block.size(),
      reinterpret_cast<PBYTE>(result.ptr()),
      result.size(),
      &sizeToAlloc,
      NCRYPT_SILENT_FLAG
   );
   if(errors)
      throw std::runtime_error("failed to sign");
   if(sizeToAlloc != result.size())
      throw std::runtime_error("sign size insane");
}

const char *signPackager::pack(const char *pPath)
{
   m_stringCache = pPath;
   m_stringCache += ".s";
   m_pLog->writeLn("[sign] signing '%s' -> '%s'",pPath,m_stringCache.c_str());

   cmn::autoCFilePtr out(m_stringCache,"wb");
   ::fprintf(out.fp,"cdwe sig");
   out.write<unsigned long>(0);

   cmn::sizedAlloc hash;

   // write file and calculate hash
   {
      autoAlgorithmProvider hashAlg;
      hashAlg.openForHash();
      autoHash hasher;
      hasher.createHash(hashAlg);

      // include the entire previous file
      cmn::block<> b;
      cmn::autoCFilePtr other(pPath,"rb");
      long size = other.calculateFileSizeFromStart();
      out.write(size);
      while(true)
      {
         size_t r = other.readBlock(b);
         if(!r)
            break;
         hasher.hashData(b.b,r);
         out.writeBlock(b,r);
      }

      hasher.finish(hash);
   }

   // retrieve the key
   autoKeyStorage keyStor;
   autoKey key;
   key.open(keyStor,autoKey::kSignKeyName);

   // sign it
   cmn::sizedAlloc result;
   signature::signBlock(key,hash,result);

   // write the hash and result
   out.writeBlock(result);
   key.exportToBlob(result);
   out.writeBlock(result);

#if 0
   // hash
   {
      BCRYPT_ALG_HANDLE h;
      /*NTSTATUS s = */::BCryptOpenAlgorithmProvider(
         &h,
         L"SHA256",
         NULL,
         0);
      ::BCryptGetProperty(
         h,
         BCRYPT_OBJECT_LENGTH,
         NULL, // buffer
         0,
         NULL, // actually copied
         0);
      BCRYPT_HASH_HANDLE hh;
      ::BCryptCreateHash(
         h,
         &hh,
         NULL, // user alloc
         0,
         NULL, // unused
         0,
         0);
      ::BCryptHashData( // multiple times ok
         hh,
         NULL, // data to hash
         0,
         0);
      ::BCryptGetProperty(
         h,
         BCRYPT_HASH_LENGTH,
         NULL, // buffer
         0,
         NULL, // actually copied
         0);
      ::BCryptFinishHash(
         hh,
         NULL, // user alloc #2
         0,
         0);
      ::BCryptDestroyHash(
         hh);
      // free alloc #2
      ::BCryptCloseAlgorithmProvider(
         h,
         0);
      // free alloc #1
   }

   // acquire key
   {
      NCRYPT_PROV_HANDLE hProv;
      ::NCryptOpenStorageProvider(
         &hProv,
         NULL,
         0);
      NCRYPT_KEY_HANDLE k;
      ::NCryptOpenKey(
         hProv,
         &k,
         L"gordian-key",
         0,
         NCRYPT_SILENT_FLAG
      );
      ::NCryptFreeObject(
         k
      );
      ::NCryptFreeObject(
         hProv
      );
   }

#if 0
   // sign
   {
      SECURITY_STATUS NCryptSignHash(
         [in]           NCRYPT_KEY_HANDLE hKey,
         [in, optional] VOID              *pPaddingInfo,
         [in]           PBYTE             pbHashValue,
         [in]           DWORD             cbHashValue,
         [out]          PBYTE             pbSignature,
         [in]           DWORD             cbSignature,
         [out]          DWORD             *pcbResult,
         [in]           DWORD             dwFlags
      );
   }

   // export public key
   {
      SECURITY_STATUS NCryptExportKey(
         [in]            NCRYPT_KEY_HANDLE hKey,
         [in, optional]  NCRYPT_KEY_HANDLE hExportKey,
         [in]            LPCWSTR           pszBlobType,
         [in, optional]  NCryptBufferDesc  *pParameterList,
         [out, optional] PBYTE             pbOutput,
         [in]            DWORD             cbOutput,
         [out]           DWORD             *pcbResult,
         [in]            DWORD             dwFlags
      );
   }

   // combine
#endif
#endif

   return pPath;
}

const char *signPackager::unpack(const char *pPath)
{
#if 0
   // split

   // hash

   // import key
   {
      SECURITY_STATUS NCryptImportKey(
         [in]           NCRYPT_PROV_HANDLE hProvider,
         [in, optional] NCRYPT_KEY_HANDLE  hImportKey,
         [in]           LPCWSTR            pszBlobType,
         [in, optional] NCryptBufferDesc   *pParameterList,
         [out]          NCRYPT_KEY_HANDLE  *phKey,
         [in]           PBYTE              pbData,
         [in]           DWORD              cbData,
         [in]           DWORD              dwFlags
      );
   }

   // verify
   {
      SECURITY_STATUS NCryptVerifySignature(
         [in]           NCRYPT_KEY_HANDLE hKey,
         [in, optional] VOID              *pPaddingInfo,
         [in]           PBYTE             pbHashValue,
         [in]           DWORD             cbHashValue,
         [in]           PBYTE             pbSignature,
         [in]           DWORD             cbSignature,
         [in]           DWORD             dwFlags
      );
   }
#endif

   return pPath;
}

tcatExposeTypeAs(signPackager,file::iPackagerSlice);

} // namespace archive
