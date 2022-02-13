#include "log.hpp"
#include <cstdio>
#include <stdarg.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../tcatlib/api.hpp"

namespace console {

class stdLog : public iLog {
public:
   explicit stdLog(iLogSink& sink) : m_sink(sink) {}

   virtual void release() { delete this; }

   virtual void writeLn(const std::string& fmt, ...)
   {
      char buffer[2000];
      va_list args;
      va_start(args, fmt);
      vsnprintf(buffer,2000,fmt.c_str(),args);
      va_end(args);
      write(buffer,true);
   }

   virtual void writeWords(const std::string& fmt, ...)
   {
      char buffer[2000];
      va_list args;
      va_start(args, fmt);
      vsnprintf(buffer,2000,fmt.c_str(),args);
      va_end(args);
      write(buffer,false);
   }

private:
   void write(const std::string& text, bool line)
   {
      if(line)
         m_sink.writeWords(text + "\r\n");
      else
         m_sink.writeWords(text);
   }

   iLogSink& m_sink;
};

class stdLogFactory : public iLogFactory {
public:
   virtual iLog& createLog(iLogSink& sink)
   {
      return *new stdLog(sink);
   }
};

tcatExposeTypeAs(stdLogFactory,iLogFactory);

} // namespace console

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
