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
protected:
   virtual iFile& _bindFile(
      const char *fileType,
      pathRoots root,
      const char *pathSuffix,
      closeTypes onClose,
      const sst::iNodeFactory& f);

private:
   std::string calculatePath(pathRoots root, const char *pathSuffix) const;
   bool exists(const std::string& path) const;
};

} // namespace file

#endif // ___file_manager_i___
