#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../tcatlib/api.hpp"
#include "manager-i.hpp"
#include <list>
#include <stdio.h>
#include <windows.h>

namespace file {

void fileBase::setPath(const std::string& path)
{
   m_path = path;
}

void fileBase::loadContent()
{
   m_existed = true;
}

void fileBase::createNewContent()
{
   m_existed = false;
}

void fileBase::release()
{
   try
   {
      m_pCloseMode->onClose(m_path,*this);
   }
   catch(std::exception& x)
   {
      log().writeLn("ERROR: %s",x.what());
   }
   delete m_pCloseMode;
   m_pCloseMode = NULL;
   delete this;
}

bool fileBase::existed() const
{
   return m_existed;
}

void fileBase::scheduleFor(iFileManager::closeTypes onClose)
{
   delete m_pCloseMode;
   m_pCloseMode = NULL;

   switch(onClose)
   {
      case iFileManager::kDiscardOnClose:
         m_pCloseMode = new discardOnCloseMode();
         break;

      case iFileManager::kSaveOnClose:
         m_pCloseMode = new saveOnCloseMode();
         break;

      case iFileManager::kDeleteAndTidyOnClose:
         m_pCloseMode = new deleteAndTidyOnCloseMode();
         break;

      default:
         throw std::runtime_error("unimpled close mode");
   }
}

void fileBase::tie(console::iLog& l)
{
   m_pLog = &l;
}

console::iLog& fileBase::log()
{
   return m_pLog ? *m_pLog : m_nLog;
}

sstFile::sstFile(const sst::iNodeFactory& nf)
: m_nf(nf)
{
}

void sstFile::loadContent()
{
   fileBase::loadContent();

   FILE *f = ::fopen(m_path.c_str(),"r");
   ::fseek(f, 0, SEEK_END);
   long fsize = ::ftell(f);
   ::fseek(f, 0, SEEK_SET);

   char *buffer = new char[fsize+1];
   ::fread(buffer,fsize,1,f);
   ::fclose(f);
   buffer[fsize] = 0;
   std::string str = buffer;
   delete [] buffer;

   tcat::typePtr<sst::iDeserializer> pD;
   m_pDict.reset(pD->parse(str.c_str(),m_nf));
}

void sstFile::createNewContent()
{
   fileBase::createNewContent();

   m_pDict.reset(new sst::dict());
}

void sstFile::saveTo()
{
   log().writeLn("writing to '%s'",m_path.c_str());
   tcat::typePtr<sst::iSerializer> pS;
   const char *pBuffer = pS->write(dict());
   FILE *f = ::fopen(m_path.c_str(),"w");
   ::fwrite(pBuffer,::strlen(pBuffer),1,f);
   ::fclose(f);
}

sst::dict& sstFile::dict()
{
   return *m_pDict;
}

sst::dict *sstFile::abdicate()
{
   return m_pDict.release();
}

fileBase::fileBase()
: m_existed(false)
, m_pCloseMode(new discardOnCloseMode())
, m_pLog(NULL)
{
}

void discardOnCloseMode::onClose(const std::string& path, fileBase& file) const
{
   file.log().writeLn("discarding changes to '%s'",path.c_str());
}

void saveOnCloseMode::onClose(const std::string& path, fileBase& file) const
{
   fileManager::createAllFoldersForFile(path,file.log(),true);
   file.saveTo();
}

void deleteAndTidyOnCloseMode::onClose(const std::string& path, fileBase& file) const
{
   fileManager::deleteFile(path,file.log(),true);
   fileManager::deleteEmptyFoldersForFile(path,file.log(),true);
}

std::string fileManager::splitLast(const std::string& path)
{
   const char *pSlash = ::strrchr(path.c_str(),'\\');
   return std::string(path.c_str(),pSlash - path.c_str());
}

bool fileManager::fileExists(const std::string& path)
{
   DWORD dwAttrib = ::GetFileAttributes(path.c_str());

   return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool fileManager::folderExists(const std::string& path)
{
   DWORD dwAttrib = ::GetFileAttributes(path.c_str());

   return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void fileManager::deleteFile(const std::string& path, console::iLog& l, bool really)
{
   if(!really)
   {
      if(fileManager::fileExists(path))
         l.writeLn("would have deleted %s",path.c_str());
      return;
   }

   l.writeLn("deleting %s",path.c_str());
   BOOL success = ::DeleteFileA(path.c_str());
   if(!success)
      throw std::runtime_error("failed to delete file");
}

bool fileManager::isFolderEmpty(const std::string& path, const std::set<std::string>& scheduledToDelete)
{
   WIN32_FIND_DATA fData;
   HANDLE hFind = ::FindFirstFileA((path + "\\*").c_str(),&fData);
   do
   {
      if(std::string(".") == fData.cFileName)
         continue;
      if(std::string("..") == fData.cFileName)
         continue;

      std::string fullPath = path + "\\" + fData.cFileName;
      if(scheduledToDelete.find(fullPath) == scheduledToDelete.end())
      {
         ::FindClose(hFind);
         return false;
      }

   } while(::FindNextFileA(hFind,&fData));
   ::FindClose(hFind);
   return true;
}

void fileManager::createAllFoldersForFile(const std::string& path, console::iLog& l, bool really)
{
   createAllFoldersForFolder(splitLast(path),l,really);
}

void fileManager::createAllFoldersForFolder(const std::string& path, console::iLog& l, bool really)
{
   std::list<std::string> missingFolders;
   std::string workingPath = path;
   do
   {
      if(!fileManager::folderExists(workingPath))
      {
         missingFolders.push_front(workingPath);
         workingPath = splitLast(workingPath);
      }
      else
         break;
   } while(true);

   for(auto it=missingFolders.begin();it!=missingFolders.end();++it)
   {
      if(really)
      {
         l.writeLn("creating folder %s",it->c_str());
         ::CreateDirectoryA(it->c_str(),NULL);
      }
      else
         l.writeLn("would have created folder %s",it->c_str());
   }
}

void fileManager::deleteEmptyFoldersForFile(const std::string& path, console::iLog& l, bool really)
{
   std::set<std::string> emptyFolders;
   std::string workingPath = path;
   do
   {
      std::string folder = splitLast(workingPath);
      if(fileManager::isFolderEmpty(folder,emptyFolders))
      {
         emptyFolders.insert(folder);
         workingPath = folder;
      }
      else
         break;
   } while(true);

   for(auto it=emptyFolders.rbegin();it!=emptyFolders.rend();++it)
   {
      if(really)
      {
         l.writeLn("deleting folder %s",it->c_str());
         BOOL success = ::RemoveDirectoryA(it->c_str());
         if(!success)
            throw std::runtime_error("failed to remove folder");
      }
      else
         l.writeLn("would have deleted folder %s",it->c_str());
   }
}

const char *fileManager::calculatePath(pathRoots root, const char *pathSuffix) const
{
   std::string path;
   if(root == kExeAdjacent)
   {
      char buffer[MAX_PATH];
      ::GetModuleFileName(NULL,buffer,MAX_PATH);
      path = buffer;
      path += "\\..\\";
   }
   else
   {
      // handle envvars
      const char *pPattern = "%APPDATA%\\..\\Local";
      if(root == kAppData)
         ;
      else if(root == kUserData)
         pPattern = "%ProgramData%";
      else if(root == kProgramFiles32Bit)
         pPattern = "%ProgramFiles(x86)%";
      else if(root == kProgramFiles64Bit)
         pPattern = "%ProgramFiles%";
      else
         throw std::runtime_error("unimpl'd root type");

      char buffer[MAX_PATH];
      ::ExpandEnvironmentStringsA(pPattern,buffer,MAX_PATH);
      if(buffer[0] == '%')
         throw std::runtime_error("unable to local path root");
      path = buffer;

      path += "\\cdwe\\";

      if(root == kUserData || root == kAppData)
         path += "gordian\\";
   }

   path += pathSuffix;

   m_pathCache = path;
   return m_pathCache.c_str();
}

void fileManager::createAllFoldersForFile(const char *path, console::iLog& l, bool really) const
{
   fileManager::createAllFoldersForFile(std::string(path),l,really);
}

void fileManager::createAllFoldersForFolder(const char *path, console::iLog& l, bool really) const
{
   fileManager::createAllFoldersForFolder(std::string(path),l,really);
}

iFile& fileManager::_bindFile(const char *fileType, const char *path, closeTypes onClose, const sst::iNodeFactory& nf)
{
   if(typeid(iSstFile).name() != std::string(fileType))
      throw std::runtime_error("unknown file type requested");

   cmn::autoReleasePtr<sstFile> pFile(new sstFile(nf));

   pFile->setPath(path);

   if(fileExists(path))
      pFile->loadContent();
   else
      pFile->createNewContent();

   return *pFile.abdicate();
}

tcatExposeTypeAs(fileManager,iFileManager);

} // namespace file
