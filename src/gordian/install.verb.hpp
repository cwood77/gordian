#ifndef ___gordian_install_verb___
#define ___gordian_install_verb___

#include "../console/arg.hpp"
#include "../console/log.hpp"

namespace gordian {

class installCommand : public console::iCommand {
public:
   installCommand() : oYes(false), oPattern("*") {}

   bool oYes;
   std::string oPattern;

   virtual void run(console::iLog& l);
};

} // namespace gordian

#endif // ___gordian_install_verb___
