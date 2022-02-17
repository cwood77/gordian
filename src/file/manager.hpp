#ifndef ___file_manager___
#define ___file_manager___

#include "api.hpp"
#include <typeinfo>

namespace console { class iLog; }

namespace file {

class iFile;

class iFileManager {
public:
   enum pathRoots {
      kAppData,
      kUserData
   };

   enum closeTypes {
      kDiscardOnClose,
      kSaveOnClose,
      kDeleteAndTidyOnClose
   };

   virtual ~iFileManager() {}

   virtual const char *calculatePath(pathRoots root, const char *pathSuffix) = 0;
   virtual void createAllFoldersForFile(const char *path, console::iLog& l, bool really) const = 0;

   template<class T>
   T& bindFile(pathRoots root,
      const char *pathSuffix,
      const sst::iNodeFactory& f = sst::defNodeFactory(),
      closeTypes onClose = kDiscardOnClose)
   {
      return dynamic_cast<T&>(_bindFile(typeid(T).name(),root,pathSuffix,onClose,f));
   }

protected:
   virtual iFile& _bindFile(
      const char *fileType,
      pathRoots root,
      const char *pathSuffix,
      closeTypes onClose,
      const sst::iNodeFactory& f) = 0;
};

class iFile {
public:
   virtual ~iFile() {}
   virtual void release() = 0;

   virtual bool existed() const = 0;
   virtual void scheduleFor(iFileManager::closeTypes onClose) = 0;

   virtual void tie(console::iLog& l) = 0;
};

class iSstFile : public virtual iFile {
public:
   virtual sst::dict& dict() = 0;
};

} // namespace file

#endif // ___file_manager___
