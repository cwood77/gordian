#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include "compress.hpp"
#include <memory>
#include <stdexcept>

namespace archive {

compressor::~compressor()
{
   ::CloseCompressor(han);
}

void compressor::run(cmn::autoCFilePtr& src, cmn::autoCFilePtr& dest)
{
   auto inSize = src.calculateFileSizeFromHere();
   std::unique_ptr<char[]> pInBytes(new char[inSize]);
   ::fread(pInBytes.get(),inSize,1,src.fp);

   SIZE_T outSize = 0;
   ::Compress(
      han,
      pInBytes.get(),
      inSize,
      NULL,
      0,
      &outSize);

   std::unique_ptr<char[]> pOutBytes(new char[outSize]);
   ::Compress(
      han,
      pInBytes.get(),
      inSize,
      pOutBytes.get(),
      outSize,
      &outSize);

   ::fwrite(pOutBytes.get(),outSize,1,dest.fp);
}

decompressor::~decompressor()
{
   ::CloseDecompressor(han);
}

void decompressor::run(cmn::autoCFilePtr& src, cmn::autoCFilePtr& dest)
{
   auto inSize = src.calculateFileSizeFromHere();
   std::unique_ptr<char[]> pInBytes(new char[inSize]);
   ::fread(pInBytes.get(),inSize,1,src.fp);

   SIZE_T outSize = 0;
   ::Decompress(
      han,
      pInBytes.get(),
      inSize,
      NULL,
      0,
      &outSize);

   std::unique_ptr<char[]> pOutBytes(new char[outSize]);
   ::Decompress(
      han,
      pInBytes.get(),
      inSize,
      pOutBytes.get(),
      outSize,
      &outSize);

   ::fwrite(pOutBytes.get(),outSize,1,dest.fp);
}

compressSetting::~compressSetting()
{
   delete [] pValue;
}

void compressSetting::load(cmn::autoCFilePtr& f)
{
   flag = f.read<unsigned long>();
   valueSize = f.read<unsigned long>();
   pValue = new char[valueSize];
   ::fread(pValue,valueSize,1,f.fp);
}

void compressSetting::save(cmn::autoCFilePtr& f)
{
   f.write<unsigned long>(flag);
   f.write<unsigned long>(valueSize);
   ::fwrite(pValue,valueSize,1,f.fp);
}

compressHeader::~compressHeader()
{
   for(auto it=settings.begin();it!=settings.end();++it)
      delete *it;
}

void compressHeader::load(cmn::autoCFilePtr& f)
{
   alg = f.read<unsigned long>();

   auto n = f.read<unsigned long>();
   for(unsigned long i=0;i<n;i++)
   {
      auto s = new compressSetting();
      s->load(f);
      settings.push_back(s);
   }
}

void compressHeader::save(cmn::autoCFilePtr& f)
{
   f.write<unsigned long>(alg);

   f.write<unsigned long>(settings.size());
   for(auto it=settings.begin();it!=settings.end();++it)
      (*it)->save(f);
}

compressor *compressHeader::createCompressor()
{
   auto c = new compressor();

   BOOL success = ::CreateCompressor(
      alg,
      NULL,
      &c->han);
   if(!success)
      throw std::runtime_error("faied to create compressor");

   for(auto it=settings.begin();it!=settings.end();++it)
      ::SetCompressorInformation(
         c->han,
         static_cast<COMPRESS_INFORMATION_CLASS>((*it)->flag),
         (*it)->pValue,
         (*it)->valueSize
      );

   return c;
}

decompressor *compressHeader::createDecompressor()
{
   auto c = new decompressor();

   BOOL success = ::CreateDecompressor(
      alg,
      NULL,
      &c->han);
   if(!success)
      throw std::runtime_error("faied to create decompressor");

   for(auto it=settings.begin();it!=settings.end();++it)
      ::SetDecompressorInformation(
         c->han,
         static_cast<COMPRESS_INFORMATION_CLASS>((*it)->flag),
         (*it)->pValue,
         (*it)->valueSize
      );

   return c;
}

const char *compressPackager::pack(const char *pPath)
{
   m_stringCache = pPath;
   m_stringCache += ".z";

   m_pLog->writeLn("[compress] compressing '%s' -> '%s'",pPath,m_stringCache.c_str());

   cmn::autoCFilePtr out(m_stringCache,"wb");
   ::fprintf(out.fp,"cdwe comp");
   out.write<unsigned long>(0);

   compressHeader hdr;
   hdr.alg = COMPRESS_ALGORITHM_MSZIP;
   hdr.save(out);

   {
      cmn::autoCFilePtr in(pPath,"rb");
      std::unique_ptr<compressor> c(hdr.createCompressor());
      c->run(in,out);
   }

   return m_stringCache.c_str();
}

const char *compressPackager::unpack(const char *pPath)
{
   const size_t nPath = ::strlen(pPath);
   if(nPath <= 2)
      throw std::runtime_error("bad extension on compress 1");
   if(::strcmp(pPath+nPath-2,".z")!=0)
      throw std::runtime_error("bad extension on compress 2");
   std::string outPath(pPath,nPath-2);
   m_pLog->writeLn("[compress] uncompressing '%s' -> '%s'",pPath,outPath.c_str());

   cmn::autoCFilePtr in(pPath,"rb");
   {
      cmn::block<9> thumbprint;
      if(in.readBlock(thumbprint) != 9)
         throw std::runtime_error("invalid compress header 1");
      if(::strncmp("cdwe comp",thumbprint.b,9)!=0)
         throw std::runtime_error("invalid compress header 2");
      if(in.read<unsigned long>() != 0)
         throw std::runtime_error("invalid compress version");
   }

   compressHeader hdr;
   hdr.load(in);

   {
      cmn::autoCFilePtr out(outPath,"wb");
      std::unique_ptr<decompressor> d(hdr.createDecompressor());
      d->run(in,out);
   }

   m_stringCache = outPath;
   return m_stringCache.c_str();
}

tcatExposeTypeAs(compressPackager,file::iPackagerSlice);

} // namespace archive
