#ifndef ___curator__directory___
#define ___curator__directory___

#include <list>
#include <map>

namespace console { class iLog; }
namespace sst { class dict; }

namespace curator {

struct directory {
public:
   directory() : m_pLog(NULL), m_pDict(NULL) {}
   ~directory();

   void tie(console::iLog& l, sst::dict& config);

   console::iLog& log() const { return *m_pLog; }
   sst::dict& config() { return *m_pDict; }

   std::map<std::string,std::map<size_t,sst::dict*> > all;
   std::map<std::string,std::list<sst::dict*> > categories;

private:
   mutable console::iLog *m_pLog;
   sst::dict *m_pDict;
};

} // namespace curator

#endif // ___curator__directory___
