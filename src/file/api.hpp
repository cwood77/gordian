#include <map>
#include <set>
#include <string>
#include <vector>

namespace console { class iLog; }

namespace sst {

// simple structed text

class node {
public:
   virtual ~node() {}

   template<class T>
   T& as() { return dynamic_cast<T&>(*this); }
};

class str : public node {
public:
   str& operator=(const std::string& value);

   void set(const std::string& value);
   const std::string& get();

private:
   std::string m_value;
};

class dict : public node {
public:
   virtual ~dict();

   node& operator[](const std::string& key);

   template<class T>
   T& add(const std::string& key)
   { 
      T *pValue = new T();
      replace(key,pValue);
      return *pValue;
   }

   std::map<std::string,node*>& asMap() { return m_value; }

private:
   void replace(const std::string& key, node *pValue);

   std::map<std::string,node*> m_value;
};

class array : public node {
public:
   virtual ~array();

   size_t size() const;
   node& operator[](size_t index);

   template<class T>
   T& addAt(size_t index)
   { 
      T *pValue = new T();
      replace(index,pValue);
      return *pValue;
   }

   template<class T>
   T& append()
   {
      T *pValue = new T();
      m_value.push_back(pValue);
      return *pValue;
   }

   std::vector<node*>& asVector() { return m_value; }

private:
   void replace(size_t index, node *pValue);

   std::vector<node*> m_value;
};

class iSerializer {
public:
   virtual ~iSerializer() {}
   virtual const char *write(node& n) = 0;
};

// provided by client so allocations happen in client
// memory-space
class iNodeFactory {
public:
   enum types {
      kDict,
      kArray,
      kStr
   };

   virtual void *createRootDictNode() = 0;
   virtual void releaseRootDictNode() = 0;

   virtual void *dict_add(types t, const std::string& key) = 0;
   virtual void *array_append(types t) = 0;

   virtual void str_set(void *pNode, const std::string& value) = 0;
};

class iDeserializer {
public:
   virtual ~iDeserializer() {}
   virtual dict *parse(iNodeFactory& f, const char *pPtr) = 0;
};

class lexor {
public:
   explicit lexor(const char *pPtr);

   int getToken();
   std::string getLexeme();
};

#include "api.ipp"

} // namespace sst
