#include "sign.hpp"

namespace archive {

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

   return pPath;
}

const char *signPackager::unpack(const char *pPath)
{
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

   return pPath;
}

} // namespace archive
