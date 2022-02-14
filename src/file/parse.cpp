#include "parse.hpp"
#include <cstring>
#include <stdexcept>

namespace sst {

lexor::lexor(const char *pPtr)
: m_pThumb(pPtr)
, m_token(kEOI)
{
   buildMap();
   advance();
}

lexor::tokens lexor::getToken()
{
   return m_token;
}

std::string lexor::getLexeme()
{
   return m_lexeme;
}

void lexor::advance()
{
   const char *pOldThumb = m_pThumb;
   while(true)
   {
      eatWhitespace();
      eatComments();

      if(pOldThumb == m_pThumb)
         break;
      pOldThumb = m_pThumb;
   }

   if(m_pThumb[0] == '"')
      handleString();
   else
      categorizeThumb();

   if(m_token != kEOI)
      ++m_pThumb;
}

void lexor::buildMap()
{
   m_map[ 0 ] = kEOI;
   m_map['{'] = kLBrace;
   m_map['}'] = kRBrace;
   m_map['['] = kLBracket;
   m_map[']'] = kRBracket;
   m_map[':'] = kColon;
   m_map[','] = kComma;
}

void lexor::eatWhitespace()
{
   for(;*m_pThumb&&::strchr(" \t\r\n",*m_pThumb);++m_pThumb);
}

void lexor::eatComments()
{
   if(m_pThumb[0] != '#')
      return;

   for(;*m_pThumb!='\n'&&*m_pThumb;m_pThumb++);
}

void lexor::categorizeThumb()
{
   auto it = m_map.find(*m_pThumb);
   if (it!=m_map.end())
      m_token = it->second;
}

void lexor::handleString()
{
   ++m_pThumb;
   m_token = kQuotedStringLiteral;

   const char *pEnd = m_pThumb;
   for(;*pEnd!='"'&&*pEnd;pEnd++);
   if(pEnd[0] != '"')
      throw std::runtime_error("unterminated string literal");

   m_lexeme = std::string(m_pThumb,pEnd-m_pThumb);
   m_pThumb = pEnd;
}

} // namespace sst
