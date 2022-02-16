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
   virtual void adjustIndent(int i) = 0;
};

class nullLog : public iLog {
public:
   virtual void release() {}
   virtual void writeLn(const std::string& fmt, ...) {}
   virtual void writeWords(const std::string& fmt, ...) {}
   virtual void adjustIndent(int i) {}
};

class autoIndent {
public:
   autoIndent(iLog& l, int offset = 3)
   : m_l(l) , m_offset(offset * -1)
   { l.adjustIndent(offset); }

   ~autoIndent()
   { m_l.adjustIndent(m_offset); }

private:
   iLog& m_l;
   int m_offset;

   autoIndent(const autoIndent&);
   autoIndent& operator=(const autoIndent&);
};

class iLogFactory {
public:
   virtual ~iLogFactory() {}
   virtual iLog& createLog(iLogSink& sink) = 0;
};

} // namespace console

#endif // ___console_log___
