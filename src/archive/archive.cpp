#define WIN32_LEAN_AND_MEAN
#include "../console/log.hpp"
#include "../file/api.hpp"
#include "../file/packager.hpp"
#include "../tcatlib/api.hpp"
#include "archive.hpp"
#include <windows.h>

namespace archive {

archiveWriter::archiveWriter(const std::string& contentsBasePath, const std::string& archivePath, console::iLog& l)
: m_archive(archivePath,"wb")
, m_contentsBasePath(contentsBasePath)
, m_log(l)
{
   m_log.writeLn("[archive] opening file for write - %s",archivePath.c_str());

   // thumbprint is special as no size is saved
   ::fprintf(m_archive.fp,"cdwe archive");

   // version
   m_archive.write<unsigned long>(0);
}

void archiveWriter::addFolder(const std::string& path)
{
   std::string relPath = path.c_str() + m_contentsBasePath.length() + 1;
   m_log.writeLn("[archive] adding folder %s",relPath.c_str());

   m_archive.write<unsigned char>(0); // type (1==folder)

   m_archive.writeString(relPath);
}

void archiveWriter::addFile(const std::string& path)
{
   std::string relPath = path.c_str() + m_contentsBasePath.length() + 1;
   m_log.writeLn("[archive] adding file %s",relPath.c_str());

   m_archive.write<unsigned char>(1); // type (1==file)

   m_archive.writeString(relPath);

   cmn::block<> b;
   cmn::autoCFilePtr other(path,"rb");
   long size = other.calculateFileSizeFromStart();
   m_archive.write(size);
   while(true)
   {
      size_t r = other.readBlock(b);
      if(!r)
         break;
      m_archive.writeBlock(b,r);
   }
}

void archiveReader::unpack(cmn::autoCFilePtr& f)
{
   readHeader(f);
   readEntity(f);
}

void archiveReader::readHeader(cmn::autoCFilePtr& f)
{
   cmn::block<12> thumbprint;
   if(f.readBlock(thumbprint) != 12)
      throw std::runtime_error("invalid archive header 1");
   if(::strncmp("cdwe archive",thumbprint.b,12)!=0)
      throw std::runtime_error("invalid archive header 2");

   if(f.read<unsigned long>() != 0)
      throw std::runtime_error("invalid archive version");

   BOOL success = ::CreateDirectoryA(m_contentsBasePath.c_str(),NULL);
   if(!success)
      throw std::runtime_error("failure creating folder");
}

void archiveReader::readEntity(cmn::autoCFilePtr& f)
{
   bool success = false;
   auto type = f.read<unsigned char>(&success);
   if(!success)
      return;

   if(type == 0)
      readFolder(f);
   else if(type == 1)
      readFile(f);
   else
      throw std::runtime_error("invalid type in archive");

   readEntity(f);
}

void archiveReader::readFolder(cmn::autoCFilePtr& f)
{
   std::string relPath = f.readString();
   std::string fullPath = m_contentsBasePath + "\\" + relPath;

   m_log.writeLn("[archive] reading folder %s",fullPath.c_str());

   BOOL success = ::CreateDirectoryA(fullPath.c_str(),NULL);
   if(!success)
      throw std::runtime_error("failure creating folder");
}

void archiveReader::readFile(cmn::autoCFilePtr& f)
{
   std::string relPath = f.readString();
   std::string fullPath = m_contentsBasePath + "\\" + relPath;

   m_log.writeLn("[archive] reading file %s",fullPath.c_str());

   cmn::block<> b;
   cmn::autoCFilePtr noob(fullPath,"wb");
   auto sizeLeft = f.read<long>();
   while(true)
   {
      if(sizeLeft == 0)
         break;
      size_t blockSize = 4096 < sizeLeft ? 4096 : sizeLeft;
      f.readBlock(b,blockSize);
      noob.writeBlock(b,blockSize);
      sizeLeft -= blockSize;
   }
}

void folderReflector::reflectFolder(const std::string& folder)
{
   std::string pattern = folder;
   pattern += "\\*";

   WIN32_FIND_DATA fData;
   HANDLE hFind = ::FindFirstFileA(pattern.c_str(),&fData);
   if(hFind == INVALID_HANDLE_VALUE)
      throw std::runtime_error("error building catalog");
   do
   {
      if(std::string(".") == fData.cFileName)
         continue;
      if(std::string("..") == fData.cFileName)
         continue;

      std::string fullPath = folder + "\\" + fData.cFileName;

      if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         m_w.addFolder(fullPath);
         reflectFolder(fullPath);
      }
      else
         m_w.addFile(fullPath);
   }
   while (::FindNextFile(hFind,&fData));
   ::FindClose(hFind);
}

const char *archivePackager::pack(const char *pPath)
{
   m_stringCache = pPath;
   m_stringCache += ".ar";

   archiveWriter writer(pPath,m_stringCache,*m_pLog);
   folderReflector r(writer);
   r.reflectFolder(pPath);

   return m_stringCache.c_str();
}

const char *archivePackager::unpack(const char *pPath)
{
   const size_t nPath = ::strlen(pPath);
   if(nPath <= 3)
      throw std::runtime_error("bad extension on archive 1");
   if(::strcmp(pPath+nPath-3,".ar")!=0)
      throw std::runtime_error("bad extension on archive 2");

   std::string dirPath(pPath,nPath-3);
   if(::GetFileAttributes(dirPath.c_str())!=INVALID_FILE_ATTRIBUTES)
      throw std::runtime_error("unpack destination already exists");

   cmn::autoCFilePtr ar(pPath,"rb");

   archiveReader r(dirPath,*m_pLog);
   r.unpack(ar);

   m_stringCache = dirPath;
   return m_stringCache.c_str();
}

tcatExposeTypeAs(archivePackager,file::iPackagerSlice);

} // namespace archive

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
