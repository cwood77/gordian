#include "../cmn/autoPtr.hpp"
#include "../tcatlib/api.hpp"
#include "packager.hpp"
#include <list>
#include <map>
#include <memory>
#include <stdexcept>

namespace file {

class packagerFacade : public iPackager {
public:
   virtual void release() { delete this; }

   virtual const char *pack(const char *pPath)
   {
      m_stringCache = pPath;
      for(auto it=m_packStack.begin();it!=m_packStack.end();++it)
         m_stringCache = (*it)->pack(m_stringCache.c_str());
      return m_stringCache.c_str();
   }

   virtual const char *unpack(const char *pPath)
   {
      m_stringCache = pPath;
      for(auto it=m_packStack.rbegin();it!=m_packStack.rend();++it)
         m_stringCache = (*it)->unpack(m_stringCache.c_str());
      return m_stringCache.c_str();
   }

   std::list<iPackagerSlice*> m_packStack;
   std::string m_stringCache;
};

class packagerDirectory {
public:
   void catalog(tcat::typeSet<iPackagerSlice>& slices)
   {
      for(size_t i=0;i<slices.size();i++)
      {
         auto pri = slices[i]->getPackPriority();
         auto flag = slices[i]->getFlag();
         addToMapUnique(m_byPriority,pri,slices[i]);
         addToMapUnique(m_byFlag,flag,slices[i]);
      }
   }

   void compose(size_t flags, std::list<iPackagerSlice*>& packStack)
   {
      for(auto it=m_byPriority.begin();it!=m_byPriority.end();++it)
         if(it->second->getFlag() & flags)
            packStack.push_back(it->second);
   }

private:
   void addToMapUnique(std::map<size_t,iPackagerSlice*>& m, size_t k, iPackagerSlice *v)
   {
      if(m.find(k)!=m.end())
         throw std::runtime_error("duplicate packager slice");
      m[k] = v;
   }

   std::map<size_t,iPackagerSlice*> m_byPriority;
   std::map<size_t,iPackagerSlice*> m_byFlag;
};

class packagerFactoryImpl {
public:
   void tie(console::iLog& l, sst::dict& config)
   {
      for(size_t i=0;i<slices.size();i++)
         slices[i]->tie(l,config);

      dir.catalog(slices);
   }

   tcat::typeSet<iPackagerSlice> slices;
   packagerDirectory dir;
};

class packagerFactory : public iPackagerFactory {
public:
   virtual void tie(console::iLog& l, sst::dict& config)
   {
      m_pImpl.reset(new packagerFactoryImpl());
      m_pImpl->tie(l,config);
   }

   virtual iPackager& compose(size_t flags)
   {
      cmn::autoReleasePtr<packagerFacade> pPackager(new packagerFacade());
      m_pImpl->dir.compose(flags,pPackager->m_packStack);
      return *pPackager.abdicate();
   }

private:
   std::unique_ptr<packagerFactoryImpl> m_pImpl;
};

tcatExposeTypeAs(packagerFactory,iPackagerFactory);

} // namespace file
