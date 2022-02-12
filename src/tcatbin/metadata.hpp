#ifndef ___tcatbin_metadata___
#define ___tcatbin_metadata___

//#include <list>
//#include <map>
#include <set>
#include <string>

namespace tcatbin {

class iTypeServer;
class iModuleServer;

// holds all server DLLs in memory
class libTable {
public:
   ~libTable();

   void add(void* pLib);

private:
   std::set<void*> m_libs;
};

// lookup table for type to iTypeServer
class catalogMetadata {
public:
   void record(iTypeServer& type);

   //typeServerMetadata& demandOne(const std::string& typeName);
   //std::list<typeServerMetadata>& demandMany(const std::string& typeName);

private:
   //std::map<std::string,std::list<typeServerMetadata> > m_data;
};

// holds a DLL is memory while it's checked
class libProbe {
public:
   explicit libProbe(const std::string& filePath);
   ~libProbe();

   bool isLoaded() const;
   iModuleServer *getServer() const;

   void transfer(libTable& table);

private:
   void *m_pLib;
};

// examines a file for server-ness
class catalogBuilder {
public:
   catalogBuilder(catalogMetadata& data, libTable& libs);

   void reflectFile(const std::string& candidatePath);

private:
   catalogMetadata& m_meta;
   libTable& m_libs;
};

// walks files
class folderReflector {
public:
   explicit folderReflector(catalogBuilder& builder);

   void reflect(const std::string& folder);

private:
   catalogBuilder& m_builder;
};

} // namespace tcatbin

#endif // ___tcatbin_metadata___
