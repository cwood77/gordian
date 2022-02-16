#ifndef ___file_manager___
#define ___file_manager___

namespace console { class iLog; }
namespace sst { class dict; }

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

   template<class T>
   T& bindFile(pathRoots& root, const char *pathSuffix, closeTypes onClose = kDiscardOnClose);

protected:
   virtual iFile& _bindFile(const char *fileType, pathRoots& root, const char *pathSuffix, closeTypes onClose) = 0;
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
