#include "../file/api.hpp"
#include "instr.hpp"
#include "recipes.hpp"
#include <memory>
#include <stdexcept>

namespace curator {

void instrBuilder::populate(sst::dict& p, bool install)
{
   if(install)
      populateInstall(p);
   else
      populateUninstall(p);
}

void instrBuilder::populateInstall(sst::dict& p)
{
   const std::string key = "install-steps";
   if(p.has(key))
      populateInstrs(p,key);
   else
      populateDefaultInstall(p);
}

void instrBuilder::populateDefaultInstall(sst::dict& p)
{
   std::unique_ptr<batchFileInstr> pInstr(new batchFileInstr(m_d,p));

   sst::dict c;
   c.add<sst::str>("path") = "$L/xcopy-deploy";
   pInstr->config(c);

   m_children.push_back(pInstr.release());
}

void instrBuilder::populateInstrs(sst::dict& p, const std::string& key)
{
   auto& L = p[key].as<sst::array>();
   for(size_t i=0;i<L.size();i++)
   {
      std::unique_ptr<instrBase> pInstr;

      auto& config = L[i].as<sst::dict>();
      auto& type = config["type"].as<sst::str>().get();
      if(type == "add-path")
         pInstr.reset(new addToPathInstr(m_d,p));
      else if(type == "remove-path")
         pInstr.reset(new removeFromPathInstr(m_d,p));
      else if(type == "batch")
         pInstr.reset(new batchFileInstr(m_d,p));
      else if(type == "default")
      {
         populateDefaultInstall(p); // INSTALL SPECIFIC
         continue;
      }
      else
         throw std::runtime_error("unsupported instr type name");

      pInstr->config(config);

      m_children.push_back(pInstr.release());
   }
}

} // namespace curator
