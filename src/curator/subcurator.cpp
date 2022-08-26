#include "subcurator.hpp"
#include "../file/api.hpp"
#include "directory.hpp"

namespace curator {

sst::dict *subCuratorHelper::findNextGordian(directory& d)
{
   auto currGordianGuid = *(d.installedGuidsByProdName["gordian"].begin());
   auto& currGordianVers = (*d.dictsByGuid[currGordianGuid])["version"]
      .as<sst::mint>().get();

   std::map<sst::mint::pod_t,sst::dict*> newerVersions;
   for(auto it=d.dictsByGuid.begin();it!=d.dictsByGuid.end();++it)
   {
      if(!d.isNameMatch(*it->second,"gordian"))
         continue;

      auto& version = (*it->second)["version"].as<sst::mint>().get();
      if(version > currGordianVers)
         newerVersions[version] = it->second;
   }

   if(newerVersions.size() == 0) return NULL;
   return newerVersions.begin()->second;
}

void subCuratorHelper::requireLatestGordian(directory& d)
{
   if(findNextGordian(d))
      throw std::runtime_error(
         "a newer version of gordian is available; please install it first");
}

} // namespace curator
