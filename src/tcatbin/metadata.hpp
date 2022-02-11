#ifndef ___tcatbin_metadata___
#define ___tcatbin_metadata___

#include <list>
#include <map>
#include <string>

namespace tcatbin {

class instTable;

class typeServerMetadata {
public:
   typeServerMetadata(const std::string& binPath, size_t idx);

   void *instantiate(instTable& t, const std::string& typeName);

private:
   std::string m_binPath;
   size_t m_typeServerIdx;
};

class catalogMetadata {
public:
   void record(const std::string& typeName, const std::string& binPath, size_t idx);

   typeServerMetadata& demandOne(const std::string& typeName);
   std::list<typeServerMetadata>& demandMany(const std::string& typeName);

private:
   std::map<std::string,std::list<typeServerMetadata> > m_data;
};

class moduleReflector {
public:
   moduleReflector(const std::string& filePath);

   void reflect(catalogMetadata& meta);

private:
   std::string m_filePath;
   void *m_pLib;
};

class folderReflector {
public:
   folderReflector(catalogMetadata& meta);

   void reflect(const std::string& folder);

private:
   catalogMetadata& m_meta;
};

} // namespace tcatbin

#endif // ___tcatbin_metadata___
