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

parser::parser(lexor& l, const iNodeFactory& f)
: m_l(l)
, m_f(f)
{
}

void *parser::parseConfig()
{
   demand(lexor::kLBrace);

   void *pDict = m_f.createRootDictNode();
   parseDictionary(pDict);

   m_f.releaseRootDictNode();
   return pDict;
}

void parser::parseNode(void *pNode, iNodeFactory::types ty)
{
   if(ty == iNodeFactory::kDict)
      parseDictionary(pNode);
   else if(ty == iNodeFactory::kArray)
      parseArray(pNode);
   else if(ty == iNodeFactory::kStr)
   {
      std::string value = m_l.getLexeme();
      m_l.advance();
      m_f.str_set(pNode,value);
   }
}

void parser::parseDictionary(void *pNode)
{
   m_l.advance();
   do
   {
      lexor::tokens t = m_l.getToken();

      if(t == lexor::kQuotedStringLiteral)
      {
         std::string key = m_l.getLexeme();
         m_l.advance();

         demandAndEat(lexor::kColon);

         iNodeFactory::types ty = determineNodeType();
         void *pSubNode = m_f.dict_add(pNode,ty,key);
         parseNode(pSubNode,ty);
      }
      else if(t == lexor::kRBrace)
      {
         m_l.advance();
         return;
      }
      else if(t == lexor::kComma)
         m_l.advance();
      else
         throw std::runtime_error("unexpected element in sst dictionary");
   } while(true);
}

void parser::parseArray(void *pNode)
{
   m_l.advance();
   do
   {
      lexor::tokens t = m_l.getToken();

      if(t == lexor::kRBracket)
      {
         m_l.advance();
         return;
      }
      else if(t == lexor::kComma)
         m_l.advance();
      else
      {
         iNodeFactory::types ty = determineNodeType();
         void *pSubNode = m_f.array_append(pNode,ty);
         parseNode(pSubNode,ty);
      }
   } while(true);
}

iNodeFactory::types parser::determineNodeType()
{
   if(m_l.getToken() == lexor::kLBrace)
      return iNodeFactory::kDict;
   else if(m_l.getToken() == lexor::kLBracket)
      return iNodeFactory::kArray;
   else if(m_l.getToken() == lexor::kQuotedStringLiteral)
      return iNodeFactory::kStr;
   else
      throw std::runtime_error("unexpected node creation in sst parser");
}

void parser::demand(lexor::tokens t)
{
   if(m_l.getToken() != t)
      throw std::runtime_error("expected token absent in sst parse");
}

void parser::demandAndEat(lexor::tokens t)
{
   demand(t);
   m_l.advance();
}

} // namespace sst
