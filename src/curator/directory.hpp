#ifndef ___curator__directory___
#define ___curator__directory___

#include <list>
#include <map>
#include <set>

namespace console { class iLog; }
namespace sst { class dict; }
namespace store { class iStore; }

namespace curator {

class iRecipe;
class iRequest;

struct directory {
public:
   static std::string calcManifestGuid(sst::dict& package);
   static std::string calcManifestGuid(const std::string& name, size_t vers);

   directory() : m_pLog(NULL), m_pDict(NULL), m_pStore(NULL) {}
   ~directory();

   void tie(console::iLog& l, sst::dict& config, store::iStore& s);

   void loadAllManifestsIf(const std::string& manifestFolder);

   void parsePattern(const iRequest& r, std::string& nameMatch, std::string& verMatch);
   bool isNameMatch(sst::dict& c, const std::string& nameMatch);
   bool isMatch(sst::dict& c, const std::string& nameMatch, const std::string& verMatch);
   bool isInstalled(sst::dict& d);
   bool isInstalled(const std::string& name, size_t vers);

   console::iLog& log() const { return *m_pLog; }
   sst::dict& config() { return *m_pDict; }
   store::iStore& store() { return *m_pStore; }

   // guid -> dict all manifests
   // prodName -> versions

   // needs:
   // list:prod/verions in printable order
   // uninstall:all installed manifests
   // install:
   //    - keyword: all manifests
   //    - pattern: all manifests
   //               latest version of ea product
   //    - installed prods by prodName

   std::map<std::string,sst::dict*> dictsByGuid;
   std::map<std::string,std::set<size_t> > availableGuidsSorted;
   std::map<std::string,std::set<std::string> > guidsByCategory;

   void categorizeInstalled();
   std::map<std::string,std::set<size_t> > installedGuidsSorted;
   std::map<std::string,std::set<std::string> > installedGuidsByProdName;

private:
   void loadManifest(const std::string& manifest);

   mutable console::iLog *m_pLog;
   sst::dict *m_pDict;
   store::iStore *m_pStore;
};

} // namespace curator

#endif // ___curator__directory___
