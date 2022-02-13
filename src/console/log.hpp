#ifndef ___console_log___
#define ___console_log___

#include <stdio.h>
#include <string>

namespace console {

class iLogFactory;

class iLogSink {
public:
   virtual void writeWords(const std::string& words) = 0;
};

class cStdOutLogSink : public iLogSink {
public:
   virtual void writeWords(const std::string& words) { printf(words.c_str()); }
};

class iLog {
public:
   virtual ~iLog() {}
   virtual void release() = 0;
   virtual void writeLn(const std::string& fmt, ...) = 0;
   virtual void writeWords(const std::string& fmt, ...) = 0;
};

class iLogFactory {
public:
   virtual ~iLogFactory() {}
   virtual iLog& createLog(iLogSink& sink) = 0;
};

} // namespace console

#endif // ___console_log___
