#ifndef ___cmn_win32___
#define ___cmn_win32___

#define WIN32_LEAN_AND_MEAN
#include <stdexcept>
#include <string>
#include <windows.h>

namespace cmn {

class osEvent {
public:
   explicit osEvent(const std::string& name)
   {
      m_hEvent = ::CreateEventA(NULL,FALSE,FALSE,name.c_str());
   }

   ~osEvent()
   {
      ::CloseHandle(m_hEvent);
   }

   void set()
   {
      ::SetEvent(m_hEvent);
   }

   void wait(DWORD timeout)
   {
      DWORD r = ::WaitForSingleObject(m_hEvent,timeout);
      if(r != WAIT_OBJECT_0)
         throw std::runtime_error("timeout waiting for signal");
   }

private:
   HANDLE m_hEvent;
};

} // namespace cmn

#endif // ___cmn_win32___
