#include "../cmn/autoPtr.hpp"
#include "../console/log.hpp"
#include "../file/manager.hpp"
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
const char *autoKey::kPubKeyFileName = "s-pub.k";

const char *autoKey::getPubKeyFilePath(file::iFileManager& f)
{
   return f.calculatePath(file::iFileManager::kUserData,kPubKeyFileName);
}

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
   DWORD cbResult = 0;
   SECURITY_STATUS errors = ::NCryptExportKey(
      k,
      0,
      BCRYPT_ECCPUBLIC_BLOB,
      NULL,
      NULL,
      0,
      &cbResult,
      NCRYPT_SILENT_FLAG
   );
   if(errors)
      throw std::runtime_error("error exporting key");

   mem.realloc(cbResult);

   errors = ::NCryptExportKey(
      k,
      0,
      BCRYPT_ECCPUBLIC_BLOB,
      NULL,
      reinterpret_cast<PBYTE>(mem.ptr()),
      mem.size(),
      &cbResult,
      NCRYPT_SILENT_FLAG
   );
   if(errors)
      throw std::runtime_error("error exporting key");
}

void autoKey::importFromBlob(autoKeyStorage& s, cmn::sizedAlloc& mem)
{
   SECURITY_STATUS errors = ::NCryptImportKey(
      s.hProv,
      0,
      BCRYPT_ECCPUBLIC_BLOB,
      NULL,
      &k,
      reinterpret_cast<PBYTE>(mem.ptr()),
      mem.size(),
      NCRYPT_SILENT_FLAG
   );
   if(errors)
      throw std::runtime_error("error importing key");
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

void signature::verifySignature(autoKey& k, cmn::sizedAlloc& block, cmn::sizedAlloc& signature)
{
   SECURITY_STATUS errors = ::NCryptVerifySignature(
      k.k,
      NULL,
      reinterpret_cast<PBYTE>(block.ptr()),
      block.size(),
      reinterpret_cast<PBYTE>(signature.ptr()),
      signature.size(),
      NCRYPT_SILENT_FLAG
   );
   if(errors == NTE_BAD_SIGNATURE)
      throw std::runtime_error("signatures do not match!");
   else if(errors)
      throw std::runtime_error("internal verifying signature");
}

const char *signPackager::pack(const char *pPath)
{
   m_stringCache = pPath;
   m_stringCache += ".s";
   m_pLog->writeLn("[sign] signing '%s' -> '%s'",pPath,m_stringCache.c_str());

   cmn::autoCFilePtr out(m_stringCache,"wb");
   ::fprintf(out.fp,"cdwe sig");
   out.write<unsigned long>(1);

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

   // write the signature
   out.writeBlock(result);

   return m_stringCache.c_str();
}

const char *signPackager::unpack(const char *pPath)
{
   const size_t nPath = ::strlen(pPath);
   if(nPath <= 2)
      throw std::runtime_error("bad extension on sign 1");
   if(::strcmp(pPath+nPath-2,".s")!=0)
      throw std::runtime_error("bad extension on sign 2");
   std::string outPath(pPath,nPath-2);
   m_pLog->writeLn("[sign] validating '%s' -> '%s'",pPath,outPath.c_str());

   cmn::autoCFilePtr in(pPath,"rb");
   {
      cmn::block<8> thumbprint;
      if(in.readBlock(thumbprint) != 8)
         throw std::runtime_error("invalid sign header 1");
      if(::strncmp("cdwe sig",thumbprint.b,8)!=0)
         throw std::runtime_error("invalid sign header 2");
      auto v = in.read<unsigned long>();
      if(v == 0)
         throw std::runtime_error("[sign] OLD SCHEMA!");
      if(v != 1)
         throw std::runtime_error("invalid sign version");
   }

   // don't extract the inner file until we've proven the signature
   auto innerSize = in.read<long>();
   auto innerLoc = ::ftell(in.fp);

   // although, read the file to compute the hash
   cmn::sizedAlloc hash;
   {
      autoAlgorithmProvider hashAlg;
      hashAlg.openForHash();
      autoHash hasher;
      hasher.createHash(hashAlg);

      // hash the entire previous file
      cmn::sizedAlloc b;
      b.realloc(innerSize);
      ::fread(b.ptr(),innerSize,1,in.fp);
      hasher.hashData(b.ptr(),innerSize);

      hasher.finish(hash);
   }

   // read out the later stuff: the signature
   cmn::sizedAlloc signature;
   in.readBlock(signature);

   // import the public key
   autoKeyStorage keyStor;
   autoKey key;
   {
      tcat::typePtr<file::iFileManager> fMan;
      cmn::autoCFilePtr kFile(autoKey::getPubKeyFilePath(*fMan),"rb");
      cmn::sizedAlloc mem;
      kFile.readBlock(mem);
      key.importFromBlob(keyStor,mem);
   }

   // freak out if shadiness is afoot
   signature::verifySignature(key,hash,signature);

   // if the signature matches (and I survived until here), back up and extract the file
   {
      ::fseek(in.fp,innerLoc,SEEK_SET);
      cmn::sizedAlloc b;
      b.realloc(innerSize);
      ::fread(b.ptr(),innerSize,1,in.fp);
      cmn::autoCFilePtr out(outPath,"wb");
      ::fwrite(b.ptr(),b.size(),1,out.fp);
   }

   m_stringCache = outPath;
   return m_stringCache.c_str();
}

tcatExposeTypeAs(signPackager,file::iPackagerSlice);

} // namespace archive
