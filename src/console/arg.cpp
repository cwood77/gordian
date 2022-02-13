#include "arg.hpp"
#include <stdexcept>

namespace console {

commandLineParser& commandLineParser::addVerb(iArgPattern& v)
{
   m_patterns.push_back(&v);
   return *this;
}

iCommand *commandLineParser::parse(int argc, const char *argv[])
{
   iArgPattern *pLast = NULL;
   for(int i=1;i<argc;i++)
   {
      bool matched = false;

      std::list<iArgPattern*>::iterator it = m_patterns.begin();
      for(;it!=m_patterns.end()&&!matched;++it)
      {
         if((*it)->tryMatchWord(argv[i]))
         {
            pLast = *it;
            pLast->adjustPatterns(m_patterns);
            matched = true;
         }
      }
   
      if(!matched)
         throw std::runtime_error("bad usage");
   }

   if(pLast)
      return pLast->complete();
   else
      return NULL;
}

stringParameter::stringParameter(size_t offset)
: m_offset(offset)
, m_pCmd(NULL)
, m_pVerb(NULL)
, m_read(false)
{
}

bool stringParameter::tryMatchWord(const std::string& word)
{
   char *pBytes = (char*)m_pCmd;
   std::string& field = *(std::string*)(pBytes + m_offset);
   field = word;
   m_read = true;
   return true;
}

void stringParameter::adjustPatterns(std::list<iArgPattern*>& list)
{
   std::list<iArgPattern*>::iterator it = list.begin();
   for(;it!=list.end();++it)
   {
      if(*it == this)
      {
         list.erase(it);
         return;
      }
   }
}

iCommand *stringParameter::complete()
{
   m_pVerb->verify();
   return m_pCmd;
}

void stringParameter::verify()
{
   if(!m_read)
      throw std::runtime_error("bad usage: missing parameter");
}

void stringParameter::collaborate(iCommand& c, iCommandVerifier& verb)
{
   m_pCmd = &c;
   m_pVerb = &verb;
}

boolOption::boolOption(const std::string& tag, size_t offset)
: m_offset(offset)
, m_pCmd(NULL)
, m_pVerb(NULL)
{
   m_tags.insert(tag);
}

bool boolOption::tryMatchWord(const std::string& word)
{
   return m_tags.find(word) != m_tags.end();
}

void boolOption::adjustPatterns(std::list<iArgPattern*>& list)
{
   char *pBytes = (char*)m_pCmd;
   bool& field = *(bool*)(pBytes + m_offset);
   field = true;

   std::list<iArgPattern*>::iterator it = list.begin();
   for(;it!=list.end();++it)
   {
      if(*it == this)
      {
         list.erase(it);
         return;
      }
   }
}

iCommand *boolOption::complete()
{
   m_pVerb->verify();
   return m_pCmd;
}

void boolOption::collaborate(iCommand& c, iCommandVerifier& verb)
{
   m_pCmd = &c;
   m_pVerb = &verb;
}

iOption& boolOption::addTag(const std::string& tag)
{
   m_tags.insert(tag);
   return *this;
}

verbBase::~verbBase()
{
   std::list<iCommandConfig*>::iterator it = m_options.begin();
   for(;it!=m_options.end();++it)
      delete *it;
   delete m_pParam;
   delete m_pCmd;
}

verbBase& verbBase::addParameter(iCommandConfig& c)
{
   m_pParam = &c;
   m_pParam->collaborate(*m_pCmd,*this);
   return *this;
}

iOption& verbBase::addOption(iOption& o)
{
   m_options.push_back(&o);
   o.collaborate(*m_pCmd,*this);
   return o;
}

bool verbBase::tryMatchWord(const std::string& word)
{
   return word == m_tag;
}

void verbBase::adjustPatterns(std::list<iArgPattern*>& list)
{
   list.clear();

   std::list<iCommandConfig*>::iterator it = m_options.begin();
   for(;it!=m_options.end();++it)
      list.push_back(*it);

   if(m_pParam)
      list.push_back(m_pParam);
}

iCommand *verbBase::complete()
{
   verify();
   iCommand *pRval = m_pCmd;
   m_pCmd = NULL;
   return pRval;
}

void verbBase::verify()
{
   if(m_pParam)
      m_pParam->verify();
}

verbBase::verbBase(iCommand& c, const std::string& tag)
: m_pCmd(&c)
, m_tag(tag)
, m_pParam(NULL)
{
}

} // namespace console
