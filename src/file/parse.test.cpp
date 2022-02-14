#ifdef cdwTest
#include "../test/api.hpp"
#include "parse.hpp"
#include <exception>

using namespace sst;

testDefineTest(sst_lexor)
{
   const char *text = " {,\t\r\n} \"bleh \r \t\":[ # foo } \r\n ] ";

   lexor l(text);
   a.assertTrue(l.getToken() == lexor::kLBrace);
   a.assertTrue(l.getToken() == lexor::kLBrace);
   l.advance();
   a.assertTrue(l.getToken() == lexor::kComma);
   l.advance();
   a.assertTrue(l.getToken() == lexor::kRBrace);
   a.assertTrue(l.getLexeme() == "");
   l.advance();
   a.assertTrue(l.getToken() == lexor::kQuotedStringLiteral);
   a.assertTrue(l.getLexeme() == "bleh \r \t");
   l.advance();
   a.assertTrue(l.getToken() == lexor::kColon);
   l.advance();
   a.assertTrue(l.getToken() == lexor::kLBracket);
   l.advance();
   a.assertTrue(l.getToken() == lexor::kRBracket);
   l.advance();
   a.assertTrue(l.getToken() == lexor::kEOI);
}

testDefineTest(sst_unterminatedStrLit)
{
   const char *text = "  [ \"foo   ";

   lexor l(text);
   a.assertTrue(l.getToken() == lexor::kLBracket);
   try
   {
      l.advance();
      a.assertTrue(false);
   }
   catch(std::exception&)
   {
      a.assertTrue(true);
   }
}

testDefineTest(sst_consecutiveComments)
{
   const char *text = "  [  #\r\n#\n# bleh\n}";

   lexor l(text);
   a.assertTrue(l.getToken() == lexor::kLBracket);
   l.advance();
   a.assertTrue(l.getToken() == lexor::kRBrace);
   l.advance();
   a.assertTrue(l.getToken() == lexor::kEOI);
}

testDefineTest(parse_tdd)
{
   const char *text = "";
   defNodeFactory f;

   lexor l(text);
   parser p(l,f);
}

#endif // cdwTest
