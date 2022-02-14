inline str& str::operator=(const std::string& value)
{
   set(value);
   return *this;
}

inline void str::set(const std::string& value)
{
   m_value = value;
}

inline const std::string& str::get()
{
   return m_value;
}

inline dict::~dict()
{
   auto it = m_value.begin();
   for(;it!=m_value.end();++it)
      delete it->second;
}

inline node& dict::operator[](const std::string& key)
{
   return *m_value[key];
}

inline void dict::replace(const std::string& key, node *pValue)
{
   node *pOld = m_value[key];
   if(pOld)
      delete pOld;
   m_value[key] = pValue;
}

inline array::~array()
{
   auto it = m_value.begin();
   for(;it!=m_value.end();++it)
      delete *it;
}

inline size_t array::size() const
{
   return m_value.size();
}

inline node& array::operator[](size_t index)
{
   return *m_value[index];
}

inline void array::replace(size_t index, node *pValue)
{
   node *pOld = m_value[index];
   if(pOld)
      delete pOld;
   m_value[index] = pValue;
}

inline defNodeFactory::defNodeFactory()
: m_pRoot(NULL)
{
}

inline defNodeFactory::~defNodeFactory()
{
   delete m_pRoot;
}

inline void *defNodeFactory::createRootDictNode() const
{
   m_pRoot = new dict();
}

inline void defNodeFactory::releaseRootDictNode() const
{
}

inline void *defNodeFactory::dict_add(types t, const std::string& key) const
{
   return NULL;
}

inline void *defNodeFactory::array_append(types t) const
{
   return NULL;
}

inline void defNodeFactory::str_set(void *pNode, const std::string& value) const
{
}
