#include "log.hpp"
#include <cstdio>
#include <stdarg.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../tcatlib/api.hpp"

namespace console {

class stdLog : public iLog {
public:
   explicit stdLog(iLogSink& sink) : m_sink(sink), m_indent(0), m_freshLine(false) {}

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

   virtual void adjustIndent(int i)
   {
      m_indent += i;
   }

private:
   void write(const std::string& text, bool line)
   {
      std::string fulltext;

      if(m_freshLine)
         fulltext += std::string(m_indent,' ');

      fulltext += text;

      if(line)
      {
         fulltext += "\r\n";
         m_freshLine = true;
      }
      else
         m_freshLine = false;

      m_sink.writeWords(fulltext);
   }

   iLogSink& m_sink;
   size_t m_indent;
   bool m_freshLine;
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
