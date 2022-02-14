#include "parse.hpp"
#include <cstring>

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
   eatWhitespace();
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
   for(;::strchr(" \t\r\n",*m_pThumb);++m_pThumb);
}

void lexor::categorizeThumb()
{
   auto it = m_map.find(*m_pThumb);
   if (it!=m_map.end())
      m_token = it->second;
}

} // namespace sst
