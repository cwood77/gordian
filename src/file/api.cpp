#include "../console/log.hpp"
#include "api.hpp"

namespace sst {

str& str::operator=(const std::string& value)
{
   set(value);
   return *this;
}

void str::set(const std::string& value)
{
   m_value = value;
}

const std::string& str::get()
{
   return m_value;
}

void str::write(console::iLog& l) const
{
   l.writeLn("\"%s\"",m_value.c_str());
}

dict::~dict()
{
   auto it = m_value.begin();
   for(;it!=m_value.end();++it)
      delete it->second;
}

node& dict::operator[](const std::string& key)
{
   return *m_value[key];
}

void dict::write(console::iLog& l) const
{
   l.writeLn("{");
   {
      console::autoIndent _i(l);
      auto it = m_value.begin();
      for(;it!=m_value.end();++it)
      {
         l.writeWords("\"%s\": ",it->first.c_str());
         it->second->write(l);
      }
   }
   l.writeLn("}");
}

void dict::replace(const std::string& key, node *pValue)
{
   node *pOld = m_value[key];
   if(pOld)
      delete pOld;
   m_value[key] = pValue;
}

array::~array()
{
   auto it = m_value.begin();
   for(;it!=m_value.end();++it)
      delete *it;
}

size_t array::size() const
{
   return m_value.size();
}

node& array::operator[](size_t index)
{
   return *m_value[index];
}

void array::write(console::iLog& l) const
{
   l.writeLn("[");
   {
      console::autoIndent _i(l);
      auto it = m_value.begin();
      for(;it!=m_value.end();++it)
      {
         (*it)->write(l);
      }
   }
   l.writeLn("]");
}

void array::replace(size_t index, node *pValue)
{
   node *pOld = m_value[index];
   if(pOld)
      delete pOld;
   m_value[index] = pValue;
}

} // namespace sst
