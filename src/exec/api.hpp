#ifndef ___exec_api___
#define ___exec_api___

namespace console { class iLog; }

namespace exec {

class iProcessRunner {
public:
   virtual ~iProcessRunner() {}

   virtual void execute(const char *command, console::iLog& l) = 0;
};

class iScriptRunner {
public:
   virtual ~iScriptRunner() {}

   virtual void addVar(const char *pName, const char *pValue) = 0;

   virtual void execute(const char *path, console::iLog& l) = 0;
};

} // namespace exec

#endif // ___exec_api___
