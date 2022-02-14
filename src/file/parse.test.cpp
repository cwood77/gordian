#ifdef cdwTest
#include "../test/api.hpp"
#include "parse.hpp"

using namespace sst;

testDefineTest(sst_lexor)
{
   const char *text = " {,\t\r\n} \"bleh \r \t\":[ # foo ] \r\n ] ";

   lexor l(text);
   a.assertTrue(l.getToken() == lexor::kLBrace);
   a.assertTrue(l.getToken() == lexor::kLBrace);
   l.advance();
   a.assertTrue(l.getToken() == lexor::kComma);
   l.advance();
   a.assertTrue(l.getToken() == lexor::kRBrace);
}

#endif // cdwTest
