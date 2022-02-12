#include "metadata.hpp"
#include "api.hpp"
#include <stdexcept>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace tcatbin {

libTable::~libTable()
{
   std::set<void*>::iterator it = m_libs.begin();
   for(;it!=m_libs.end();++it)
      ::FreeLibrary((HMODULE)*it);
}

void libTable::add(void* pLib)
{
   m_libs.insert(pLib);
}

void catalogMetadata::record(iTypeServer& type)
{
   printf("found server for type '%s'\n",type.getTypeName());
}

libProbe::libProbe(const std::string& filePath)
: m_pLib(NULL)
{
   m_pLib = ::LoadLibrary(filePath.c_str());
}

libProbe::~libProbe()
{
   if(isLoaded())
      ::FreeLibrary((HMODULE)m_pLib);
}

bool libProbe::isLoaded() const
{
   return m_pLib != NULL;
}

iModuleServer *libProbe::getServer() const
{
   typedef tcatbin::iModuleServer& (*getFunc_t)();
   getFunc_t func = (getFunc_t)::GetProcAddress((HMODULE)m_pLib,"_Z15getModuleServerv");
   if(!func)
      return NULL;
   return &func();
}

void libProbe::transfer(libTable& table)
{
   table.add(m_pLib);
   m_pLib = NULL;
}

catalogBuilder::catalogBuilder(catalogMetadata& data, libTable& libs)
: m_meta(data), m_libs(libs)
{
}

void catalogBuilder::reflectFile(const std::string& candidatePath)
{
   libProbe probe(candidatePath);
   if(!probe.isLoaded())
      return;

   iModuleServer *pSvr = probe.getServer();
   if(!pSvr)
      return;

   size_t n = pSvr->getNumTypes();
   for(size_t i=0;i<n;i++)
      m_meta.record(pSvr->getIthType(i));

   probe.transfer(m_libs);
}

folderReflector::folderReflector(catalogBuilder& builder)
: m_builder(builder)
{
}

void folderReflector::reflect(const std::string& folder)
{
   std::string pattern = folder;
   pattern += "\\*";

   WIN32_FIND_DATA fData;
   HANDLE hFind = ::FindFirstFileA(pattern.c_str(),&fData);
   if(hFind == INVALID_HANDLE_VALUE)
      throw std::runtime_error("error building catalog");
   do
   {
      m_builder.reflectFile(folder + "\\" + fData.cFileName);
   }
   while (::FindNextFile(hFind,&fData));
   ::FindClose(hFind);
}

} // namespace tcatbin
