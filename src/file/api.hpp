#include <map>
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

   virtual void write(console::iLog& l) const = 0;
};

class str : public node {
public:
   str& operator=(const std::string& value);

   void set(const std::string& value);
   const std::string& get();

   virtual void write(console::iLog& l) const;

private:
   std::string m_value;
};

class i32 : public node {
};

class Bool : public node {
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

   virtual void write(console::iLog& l) const;

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

   virtual void write(console::iLog& l) const;

private:
   void replace(size_t index, node *pValue);

   std::vector<node*> m_value;
};

class deserializer {
};

class lexor {
};

class parser {
};

} // namespace sst
