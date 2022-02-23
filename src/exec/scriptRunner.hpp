#ifndef ___exec_scriptRunner___
#define ___exec_scriptRunner___

#include "api.hpp"
#include <map>
#include <string>

namespace exec {

class scriptRunner: public iScriptRunner {
public:
   virtual void addVar(const char *pName, const char *pValue) {}
   virtual void execute(const char *path, console::iLog& l);

private:
   static std::string chooseTempPath();

   std::map<std::string,std::string> m_vars;
};

} // namespace exec

#endif // ___exec_scriptRunner___
