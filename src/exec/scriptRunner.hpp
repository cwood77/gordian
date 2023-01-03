#ifndef ___exec_scriptRunner___
#define ___exec_scriptRunner___

#include "api.hpp"
#include <list>
#include <map>
#include <string>

namespace exec {

class processRunner : public iProcessRunner {
public:
   virtual void execute(const char *command, console::iLog& l, bool wait);
};

class autoDeleteFile {
public:
   autoDeleteFile(const std::string& path, bool armed);
   ~autoDeleteFile();

   const std::string& path() const;

   void enableDelete();
   void cancelDelete();

private:
   const std::string m_path;
   bool m_shouldDelete;
};

class debugArtifact {
public:
   explicit debugArtifact(console::iLog& l);
   ~debugArtifact();

   void add(autoDeleteFile& f, const std::string& desc);
   void enableDelete();

private:
   std::list<autoDeleteFile*> m_files;
   std::map<autoDeleteFile*,std::string> m_names;
   console::iLog& m_log;
   bool m_armed;
};

class scriptRunner: public iScriptRunner {
public:
   virtual void addVar(const char *pName, const char *pValue);
   virtual void execute(const char *path, console::iLog& l);

private:
   static const char *kSuccessSentinel;
   static const char *kErrorSentinel;

   static std::string chooseTempPath();
   std::string startLogFile(const std::string& scriptPath);
   std::string generateWrapperFile(const std::string& scriptPath, const std::string& logPath);
   void runWrapper(const std::string& wrapperPath, console::iLog& l);
   void checkLog(const std::string& logPath);

   std::map<std::string,std::string> m_vars;
};

} // namespace exec

#endif // ___exec_scriptRunner___
