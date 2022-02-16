#ifndef ___file_manager_i___
#define ___file_manager_i___

#include "api.hpp"
#include "manager.hpp"

namespace file {

class iFileCloseMode;

class fileBase : public virtual iFile {
public:
   virtual ~fileBase() {}

   void setPath(const std::string& path);
   virtual void loadContent() = 0;
   virtual void createNewContent() {}
   virtual void saveTo() = 0;

   virtual void release();
   virtual bool existed() const;
   virtual void scheduleFor(iFileManager::closeTypes onClose);
   virtual void tie(console::iLog& l);

protected:
   fileBase();

   std::string m_path;
   console::iLog& log();

private:
   bool m_existed;
   iFileCloseMode *m_pCloseMode;
   console::nullLog m_nLog;
   console::iLog *m_pLog;
};

class sstFile : public fileBase, public iSstFile {
public:
   virtual void loadContent();
   virtual void saveTo();

   virtual sst::dict& dict();
};

class iFileCloseMode {
public:
   virtual ~iFileCloseMode() {}
   virtual void onClose(const std::string& path, fileBase& file) const = 0;
};

class discardOnCloseMode : public iFileCloseMode {
public:
   virtual void onClose(const std::string& path, fileBase& file) const {}
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
   virtual iFile& _bindFile(const char *fileType, pathRoots& root, const char *pathSuffix, closeTypes onClose);
};

} // namespace file

#endif // ___file_manager_i___
