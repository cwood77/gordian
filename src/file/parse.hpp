#ifndef ___file_parser___
#define ___file_parser___

#include <map>
#include <string>

namespace sst {

class lexor {
public:
   enum tokens {
      kEOI,
      kLBrace,
      kRBrace,
      kLBracket,
      kRBracket,
      kColon,
      kComma,
      kQuotedStringLiteral,
   };

   explicit lexor(const char *pPtr);

   tokens getToken();
   std::string getLexeme();

   void advance();

private:
   void buildMap();
   void eatWhitespace();
   void eatComments();
   void categorizeThumb();
   void handleString();

   const char *m_pThumb;
   tokens m_token;
   std::string m_lexeme;
   std::map<char,tokens> m_map;
};

} // namespace sst

#endif // ___file_parser___
