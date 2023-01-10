#ifdef cdwTest
#define WIN32_LEAN_AND_MEAN
#include "../console/log.hpp"
#include "../file/manager.hpp"
#include "../http/api.hpp"
#include "../test/api.hpp"
#include "freeweb.hpp"
#include <list>
#include <windows.h>

class fakeHttpReader : public http::iHttpReader {
public:
   virtual void tie(console::iLog& l)
   { throw std::runtime_error("unexpected"); }
   virtual void bind(const std::string& baseUrl, const std::string& baseDest)
   { throw std::runtime_error("unexpected"); }

   std::list<std::string> downloaded;

   virtual void download(const std::string& leafUrl)
   { downloaded.push_back(leafUrl); }
};

void mainhtml_test_helper(const std::string& htmlPath, const std::string& component, size_t nParts, test::iAsserter& a)
{
   using namespace store;
   using namespace store::fweb;

   {
      htmlPage html;
      html.setCatalogInfo(component,nParts);
      html.saveToDisk(htmlPath);
   }
   {
      htmlPage html;
      html.loadFromDisk(htmlPath);
      std::string bn;
      size_t np;
      html.getCatalogInfo(bn,np);
      a.assertTrue(bn == component);
      a.assertTrue(np == nParts);
   }
}

testDefineTest(freeweb_mainhtml_parseroundtrip)
{
   tcat::typePtr<file::iFileManager> fMan;
   const char *pPath = fMan->calculatePath(file::iFileManager::kExeAdjacent,"test.main.html");

   mainhtml_test_helper(pPath,"test-8-win128-retail",0,a);
   mainhtml_test_helper(pPath,"test-8-win128-retail",7,a);

   BOOL success = ::DeleteFileA(pPath);
   if(!success)
      throw std::runtime_error("failed to delete file");
}

testDefineTest(freeweb_multiDownload)
{
   console::nullLog l;

   {
      fakeHttpReader r;
      store::fweb::multiDownload(l,r).download("name",0);
      std::list<std::string> expected;
      expected.push_back("name.ar.z.s.sg0");
      a.assertTrue(expected == r.downloaded);
   }

   {
      fakeHttpReader r;
      store::fweb::multiDownload(l,r).download("name",4);
      std::list<std::string> expected;
      expected.push_back("name.ar.z.s.sg0");
      expected.push_back("name.ar.z.s.sg1");
      expected.push_back("name.ar.z.s.sg2");
      expected.push_back("name.ar.z.s.sg3");
      expected.push_back("name.ar.z.s.sg4");
      a.assertTrue(expected == r.downloaded);
   }
}

#endif // cdwTest
