#include "../cmn/autoPtr.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include "split.hpp"
#include <cstring>
#include <sstream>
#include <stdexcept>

namespace archive {

const char *splitPackager::pack(const char *pPath)
{
   // header size ::
   // "cdwe split" 4+1+5=10
   // <version>         = 4
   // <count>           = 4
   const long headerSize = 10+4+4;
   const long chunkSize = getMaxSizeInBytes() - headerSize;

   // determine number of parts
   cmn::autoCFilePtr in(pPath,"rb");
   long totalSize = in.calculateFileSizeFromStart();
   long nParts = totalSize / chunkSize;
   if(nParts * chunkSize != totalSize)
      nParts++;
   m_pLog->writeLn("[split] splitting '%s' into %ld parts",pPath,nParts);

   cmn::sizedAlloc block;
   long remainingSize = totalSize;
   for(long iPart=0;iPart<nParts;iPart++)
   {
      {
         std::stringstream stream;
         stream << pPath << ".sg" << iPart;
         m_stringCache = stream.str();
      }
      m_pLog->writeLn("[split] splitting '%s' part %ld -> '%s'",
         pPath,iPart,m_stringCache.c_str());

      // use normal size until we're out
      long thisPartSize = chunkSize;
      if(remainingSize < chunkSize)
         thisPartSize = remainingSize;
      if(block.size() != (size_t)thisPartSize)
         block.realloc(thisPartSize);

      // read
      ::fread(block.ptr(),block.size(),1,in.fp);
      remainingSize -= thisPartSize;

      // write
      cmn::autoCFilePtr out(m_stringCache,"wb");
      ::fprintf(out.fp,"cdwe split");
      out.write<unsigned long>(0);
      out.write<long>(nParts);
      ::fwrite(block.ptr(),1,block.size(),out.fp);
   }

   //m_stringCache = pPath;
   //m_stringCache += ".sg0";
   return m_stringCache.c_str();
}

const char *splitPackager::unpack(const char *pPath)
{
   const size_t nPath = ::strlen(pPath);
   if(nPath <= 4)
      throw std::runtime_error("bad extension on split 1");
   if(::strcmp(pPath+nPath-4,".sg0")!=0)
      throw std::runtime_error("bad extension on split 2");
   std::string baseInPath(pPath,nPath-1);
   std::string outPath(pPath,nPath-4);
   m_pLog->writeLn("[split] joining '%s' -> '%s'",pPath,outPath.c_str());

   // determine the number of parts from file 0
   long nParts = 0;
   {
      cmn::autoCFilePtr in(pPath,"rb");
      cmn::block<10> thumbprint;
      if(in.readBlock(thumbprint) != 10)
         throw std::runtime_error("invalid split header 0.1");
      if(::strncmp("cdwe split",thumbprint.b,10)!=0)
         throw std::runtime_error("invalid split header 0.2");
      if(in.read<unsigned long>() != 0)
         throw std::runtime_error("invalid split version");
      nParts = in.read<long>();
      m_pLog->writeLn("[split]         %ld part(s)",nParts);
   }

   cmn::autoCFilePtr out(outPath,"wb");

   for(long iPart=0;iPart<nParts;iPart++)
   {
      std::stringstream inPath;
      inPath << baseInPath << iPart;
      m_pLog->writeLn("[split] reading part %ld from '%s'",iPart,inPath.str().c_str());

      cmn::autoCFilePtr in(inPath.str().c_str(),"rb");
      long remainingSize = in.calculateFileSizeFromStart();
      {
         cmn::block<10> thumbprint;
         if(in.readBlock(thumbprint) != 10)
            throw std::runtime_error("invalid split header 1");
         remainingSize -= 10;
         if(::strncmp("cdwe split",thumbprint.b,10)!=0)
            throw std::runtime_error("invalid split header 2");
         if(in.read<unsigned long>() != 0)
            throw std::runtime_error("invalid split version");
         remainingSize -= sizeof(unsigned long);
         if(in.read<long>() != nParts)
            throw std::runtime_error("invalid split cnt");
         remainingSize -= sizeof(long);
      }

      cmn::sizedAlloc block;
      block.realloc(remainingSize);
      ::fread(block.ptr(),block.size(),1,in.fp);

      ::fwrite(block.ptr(),1,block.size(),out.fp);
   }

   m_stringCache = outPath;
   return m_stringCache.c_str();
}

tcatExposeTypeAs(splitPackager,file::iPackagerSlice);

} // namespace archive
