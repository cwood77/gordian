#ifndef ___envvar_key___
#define ___envvar_key___

#define WIN32_LEAN_AND_MEAN
#include <string>
#include <windows.h>

namespace console { class iLog; }

namespace envvar {

class autoPathKey {
public:
   autoPathKey() : m_opened(false), m_pLog(NULL) {}
   ~autoPathKey() { if(m_opened) close(); }

   void tie(console::iLog& l) { m_pLog = &l; }

   void open();
   void close(); // handled automatically

   std::wstring read();
   void write(const std::wstring& v);

private:
   bool m_opened;
   console::iLog *m_pLog;
   HKEY m_k;
};

} // namespace envvar

#endif // ___envvar_key___
