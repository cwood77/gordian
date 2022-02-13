#ifndef ___test_assert___
#define ___test_assert___

#include "api.hpp"

namespace console { class iLog; }
namespace test {

class asserter : public iAsserter {
public:
   asserter(console::iLog& l, const std::string& testName);
   
   virtual void assertTrue(bool value);

   void complete();

private:
   console::iLog& m_l;
   std::string m_testName;
   int m_cnt;
};

} // namespace test

#endif // ___test_assert___
