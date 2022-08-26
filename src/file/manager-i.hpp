#ifndef ___file_manager_i___
#define ___file_manager_i___

#include "../console/log.hpp"
#include "api.hpp"
#include "manager.hpp"
#include <memory>
#include <string>

namespace file {

class iFileCloseMode;

class fileBase : public virtual iFile {
public:
   virtual ~fileBase() {}

   void setPath(const std::string& path);
   virtual void loadContent();
   virtual void createNewContent();
   virtual void saveTo() = 0;

   virtual void release();
   virtual bool existed() const;
   virtual void scheduleFor(iFileManager::closeTypes onClose);
   virtual void tie(console::iLog& l);

   console::iLog& log();

protected:
   fileBase();

   std::string m_path;

private:
   bool m_existed;
   iFileCloseMode *m_pCloseMode;
   console::nullLog m_nLog;
   console::iLog *m_pLog;
};

class sstFile : public fileBase, public iSstFile {
public:
   explicit sstFile(const sst::iNodeFactory& nf);

   virtual void loadContent();
   virtual void createNewContent();
   virtual void saveTo();

   virtual sst::dict& dict();
   virtual sst::dict *abdicate();

private:
   const sst::iNodeFactory& m_nf;
   std::unique_ptr<sst::dict> m_pDict;
};

class iFileCloseMode {
public:
   virtual ~iFileCloseMode() {}
   virtual void onClose(const std::string& path, fileBase& file) const = 0;
};

class discardOnCloseMode : public iFileCloseMode {
public:
   virtual void onClose(const std::string& path, fileBase& file) const;
};

class saveOnCloseMode : public iFileCloseMode {
public:
   virtual void onClose(const std::string& path, fileBase& file) const;
};

class deleteAndTidyOnCloseMode : public iFileCloseMode {
public:
   virtual void onClose(const std::string& path, fileBase& file) const;
};

class fileManager : public iFileManager {
public:
   static std::string splitLast(const std::string& path);
   static bool fileExists(const std::string& path);
   static bool folderExists(const std::string& path);
   static void deleteFile(const std::string& path, console::iLog& l, bool really);
   static bool isFolderEmpty(const std::string& path, const std::set<std::string>& scheduledToDelete);
   static void createAllFoldersForFile(const std::string& path, console::iLog& l, bool really);
   static void createAllFoldersForFolder(const std::string& path, console::iLog& l, bool really);
   static void deleteEmptyFoldersForFile(const std::string& path, console::iLog& l, bool really);

   virtual const char *calculatePath(pathRoots root, const char *pathSuffix) const;
   virtual void createAllFoldersForFile(const char *path, console::iLog& l, bool really) const;
   virtual void createAllFoldersForFolder(const char *path, console::iLog& l, bool really) const;
   virtual bool isFolder(const char *path) const;

protected:
   virtual iFile& _bindFile(
      const char *fileType,
      const char *path,
      closeTypes onClose,
      const sst::iNodeFactory& f);

private:
   mutable std::string m_pathCache;
};

} // namespace file

#endif // ___file_manager_i___
