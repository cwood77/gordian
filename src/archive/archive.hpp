#ifndef ___archive_archive___
#define ___archive_archive___

#include "../cmn/autoPtr.hpp"
#include <string>

namespace console { class iLog; }
namespace sst { class dict; }

namespace archive {

class archiveWriter {
public:
   archiveWriter(
      const std::string& contentsBasePath,
      const std::string& archivePath,
      console::iLog& l);

   void addFolder(const std::string& path);
   void addFile(const std::string& path);

private:
   cmn::autoCFilePtr m_archive;
   std::string m_contentsBasePath;
   console::iLog& m_log;
};

class archiveReader {
public:
   archiveReader(
      const std::string& contentsBasePath,
      console::iLog& l)
   : m_contentsBasePath(contentsBasePath)
   , m_log(l) {}

   void unpack(cmn::autoCFilePtr& f);

private:
   void readHeader(cmn::autoCFilePtr& f);
   void readEntity(cmn::autoCFilePtr& f);
   void readFolder(cmn::autoCFilePtr& f);
   void readFile(cmn::autoCFilePtr& f);

   const std::string m_contentsBasePath;
   console::iLog& m_log;
};

class folderReflector {
public:
   explicit folderReflector(archiveWriter& w) : m_w(w) {}

   void reflectFolder(const std::string& folder);

private:
   archiveWriter& m_w;
};

class archivePackager : public file::iPackagerSlice {
public:
   archivePackager() : m_pLog(NULL), m_pConfig(NULL) {}

   virtual size_t getPackPriority() const { return 1; }
   virtual size_t getFlag() const { return 1; }

   virtual void tie(console::iLog& l, sst::dict& config)
   { m_pLog = &l; m_pConfig = &config; }

   virtual const char *pack(const char *pPath);
   virtual const char *unpack(const char *pPath);

private:
   console::iLog *m_pLog;
   sst::dict *m_pConfig;
   std::string m_stringCache;
};

} // namespace archive

#endif // ___archive_archive___
