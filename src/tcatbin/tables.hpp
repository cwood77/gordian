#ifndef ___tcatbin_tables___
#define ___tcatbin_tables___

#include <map>
#include <set>
#include <string>

namespace tcatbin {

class iModuleServer;
class iTypeServer;

// holds all server DLLs in memory
class libTable {
public:
   ~libTable();

   void add(void* pLib);

private:
   std::set<void*> m_libs;
};

class instTable {
public:
   void *create(iTypeServer& svr);
   void release(void *pPtr);

private:
   std::map<void*,iTypeServer*> m_ptrs;
};

} // namespace tcatbin

#endif // ___tcatbin_tables___
