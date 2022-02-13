#include "../console/log.hpp"
#include "assert.hpp"

namespace test {

asserter::asserter(console::iLog& l, const std::string& testName)
: m_l(l)
, m_testName(testName)
, m_cnt(0)
{
}

void asserter::assertTrue(bool value)
{
   m_cnt++;
   if(!value)
   {
      m_l.writeLn("test FAILED in function '%s' on assert #%d",m_testName.c_str(),m_cnt);
   }
}

void asserter::complete()
{
   if(m_cnt == 0)
   {
      m_l.writeLn("tets FAILED by not asserting anything? - function '%s'",m_testName.c_str());
   }
}

} // namespace test
