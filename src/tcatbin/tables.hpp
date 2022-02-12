#ifndef ___tcatbin_tables___
#define ___tcatbin_tables___

#include <map>
#include <string>

namespace tcatbin {

class iModuleServer;
class iTypeServer;

class binTable {
public:
   iModuleServer& increment(const std::string& binPath);
   void decrement(iModuleServer& m);

private:
   std::map<std::string,iModuleServer*> m_svrs;
   std::map<std::string,void*> m_libs;
};

class instTable {
public:
   void *create(const std::string& binPath, size_t idx, const std::string& typeName);
   void release(void *pPtr);

private:
   binTable *m_pBinTable;
   std::map<void*,iTypeServer*> m_pTySvr;
   std::map<iTypeServer*,iModuleServer*> m_SvrMap;
};

} // namespace tcatbin

#endif // ___tcatbin_tables___
