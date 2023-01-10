#ifndef ___store_freeweb___
#define ___store_freeweb___

#include "api.hpp"

namespace http { class iHttpReader; }
namespace sst { class dict; }

namespace store {
namespace fweb {

class multiDownload {
public:
   multiDownload(console::iLog& l, http::iHttpReader& http) : m_log(l), m_http(http) {}

   void download(const std::string& path, size_t nParts);

private:
   console::iLog& m_log;
   http::iHttpReader& m_http;
};

class multiPackage {
public:
   multiPackage(console::iLog& l, sst::dict& config) : m_log(l), m_config(config) {}

   void split(std::string& path, size_t& nParts);
   void join(std::string& path, const std::string& baseName, size_t nParts);

private:
   console::iLog& m_log;
   sst::dict& m_config;
};

class htmlPage {
public:
   void loadFromDisk(const std::string& path);
   void saveToDisk(const std::string& path);

   void getCatalogInfo(std::string& baseName, size_t& nParts)
   { baseName = m_baseName; nParts = m_nParts; }
   void setCatalogInfo(const std::string& baseName, size_t nParts)
   { m_baseName = baseName; m_nParts = nParts; }

private:
   std::string m_baseName;
   size_t m_nParts;
};

} // namespace fweb

// a free web store is an HTTP web store, but is limited by file size restrictions
class freewebStore : public iStore, public iCurrentStore {
public:
   freewebStore()
   : m_pLog(NULL), m_pRootSettings(NULL), m_pMySettings(NULL), m_catDownloaded(false) {}

   virtual void initConfiguration(sst::dict& d) const;
   virtual bool tryActivate(sst::dict& d, const std::string& name, std::set<std::string>& ans) const;

   virtual void loadConfiguration(sst::dict& d, console::iLog& l);
   virtual iStore *upgradeIf();
   virtual const char *predictPackagePath(const char *pPackageName);
   virtual const char *populateManifests();
   virtual void depopulateManifests();
   virtual const char *populatePackage(const char *pPackageName);
   virtual void depopulatePackage(const char *pPackageName);
   virtual void command(const std::vector<std::string>& args);

private:
   void downloadCatalogIf();
   sst::dict& settings() { return *m_pMySettings; }

   console::iLog *m_pLog;
   sst::dict *m_pRootSettings;
   sst::dict *m_pMySettings;

   bool m_catDownloaded;
   std::string m_catalogBaseName;

   std::string m_strCache;
};


} // namespace store


#endif // ___store_freeweb___
