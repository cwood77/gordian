#ifndef ___curator__directory___
#define ___curator__directory___

#include <list>
#include <map>

namespace console { class iLog; }
namespace sst { class dict; }
namespace store { class iStore; }

namespace curator {

struct directory {
public:
   directory() : m_pLog(NULL), m_pDict(NULL), m_pStore(NULL) {}
   ~directory();

   void tie(console::iLog& l, sst::dict& config, store::iStore& s);

   std::string calcBuild(sst::dict& package);

   console::iLog& log() const { return *m_pLog; }
   sst::dict& config() { return *m_pDict; }

   std::map<std::string,std::map<size_t,sst::dict*> > all;
   std::list<sst::dict*> flat;
   std::map<std::string,std::list<sst::dict*> > categories;

private:
   mutable console::iLog *m_pLog;
   sst::dict *m_pDict;
   store::iStore *m_pStore;
};

} // namespace curator

#endif // ___curator__directory___
