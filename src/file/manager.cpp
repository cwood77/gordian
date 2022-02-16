#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../tcatlib/api.hpp"
#include "manager-i.hpp"
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
   m_pCloseMode->onClose(m_path,*this);
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

   char *buffer = new char[fsize];
   fread(buffer, fsize, 1, f);
   fclose(f);
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
   throw std::runtime_error("save is unimpled");
}

sst::dict& sstFile::dict()
{
   return *m_pDict;
}

fileBase::fileBase()
: m_existed(false)
, m_pCloseMode(new discardOnCloseMode())
, m_pLog(NULL)
{
}

void discardOnCloseMode::onClose(const std::string& path, fileBase& file) const
{
   file.log().writeLn("would have written to '%s'",path.c_str());
}

void saveOnCloseMode::onClose(const std::string& path, fileBase& file) const
{
   file.log().writeLn("save unimpled");
}

void deleteAndTidyOnCloseMode::onClose(const std::string& path, fileBase& file) const
{
   file.log().writeLn("delete unimpled");
}

iFile& fileManager::_bindFile(const char *fileType, pathRoots root, const char *pathSuffix, closeTypes onClose, const sst::iNodeFactory& nf)
{
   if(typeid(iSstFile).name() != std::string(fileType))
      throw std::runtime_error("unknown file type requested");

   cmn::autoReleasePtr<sstFile> pFile(new sstFile(nf));

   std::string path = calculatePath(root,pathSuffix);
   pFile->setPath(path);

   if(exists(path))
      pFile->loadContent();
   else
      pFile->createNewContent();

   return *pFile.abdicate();
}

std::string fileManager::calculatePath(pathRoots root, const char *pathSuffix) const
{
   const char *pPattern = "%APPDATA%\\..\\Local";
   if(root == kAppData)
      ;
   else if(root == kUserData)
      pPattern = "%ProgramData%";
   else
      throw std::runtime_error("unimpl'd root type");

   char buffer[MAX_PATH];
   ::ExpandEnvironmentStringsA(pPattern,buffer,MAX_PATH);
   if(buffer[0] == '%')
      throw std::runtime_error("unable to local path root");
   std::string path = buffer;

   path += "\\cdwe\\gordian\\";

   path += pathSuffix;

   return path;
}

bool fileManager::exists(const std::string& path) const
{
   DWORD dwAttrib = ::GetFileAttributes(path.c_str());

   return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

tcatExposeTypeAs(fileManager,iFileManager);

} // namespace file