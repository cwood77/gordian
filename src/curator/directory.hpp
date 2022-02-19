#ifndef ___curator__directory___
#define ___curator__directory___

#include <list>
#include <map>

namespace console { class iLog; }
namespace sst { class dict; }
namespace store { class iStore; }

namespace curator {

class iRecipe;
class iRequest;

struct directory {
public:
   static std::string calcFullName(sst::dict& package);

   directory() : m_pLog(NULL), m_pDict(NULL), m_pStore(NULL) {}
   ~directory();

   void tie(console::iLog& l, sst::dict& config, store::iStore& s);

   void loadAllManifestsIf(const std::string& manifestFolder);

   void parsePattern(const iRequest& r, std::string& nameMatch, std::string& verMatch);
   bool isMatch(sst::dict& c, const std::string& nameMatch, const std::string& verMatch);

   console::iLog& log() const { return *m_pLog; }
   sst::dict& config() { return *m_pDict; }

   std::map<std::string,std::map<size_t,sst::dict*> > all;
   std::list<sst::dict*> flat;
   std::map<std::string,std::list<sst::dict*> > categories;

private:
   void loadManifest(const std::string& manifest);

   mutable console::iLog *m_pLog;
   sst::dict *m_pDict;
   store::iStore *m_pStore;
};

} // namespace curator

#endif // ___curator__directory___
