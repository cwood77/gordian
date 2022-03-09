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
      L"SHA256",
      NULL,
      0);
   if(failed)
      throw std::runtime_error("failed to open hash algorithm");
}

unsigned long autoAlgorithmProvider::getObjectLength()
{
   DWORD ans = 0;
   ULONG unused = 0;
   NTSTATUS failed = ::BCryptGetProperty(
      h,
      BCRYPT_OBJECT_LENGTH,
      reinterpret_cast<PUCHAR>(&ans),
      sizeof(DWORD),
      &unused,
      0);
   if(failed)
      throw std::runtime_error("failed to query object length");
   return ans;
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

void autoKey::create(autoKeyStorage& s, const wchar_t *pType, const wchar_t *pName)
{
   m_pStor = &s;

   SECURITY_STATUS errors = ::NCryptCreatePersistedKey(
      m_pStor->hProv,
      &k,
      pType,
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

void autoKey::erase()
{
   SECURITY_STATUS errors = ::NCryptDeleteKey(
      k,
      NCRYPT_SILENT_FLAG
   );

   if(errors)
      throw std::runtime_error("failed to delete key");
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
   // TODO why aren't these error symbols available?
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

const char *signPackager::pack(const char *pPath)
{
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

} // namespace archive
