#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../console/log.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <stdexcept>
#include <stdio.h>
#include <windows.h>
#include <winhttp.h>

namespace {

std::wstring widen(const std::string& nstr)
{
   int len = nstr.size();
   std::wstring wstr(len + 1, 0);
   mbstowcs(&wstr[0], nstr.c_str(), len);
   return wstr;
}

} // anonymous namespace

namespace http {

class httpReader : public iHttpReader {
public:
   virtual void tie(console::iLog& l)
   {
      m_pLog = &l;
   }

   virtual void bind(const std::string& baseUrl, const std::string& baseDest)
   {
      m_baseUrl = baseUrl;
      m_baseDest = baseDest;
   }

   virtual void download(const std::string& leafUrl)
   {
      auto nFullUrl = m_baseUrl + "/" + leafUrl;
      auto nFullDest = m_baseDest + "\\" + leafUrl;
      m_pLog->writeLn("[http] downloading '%s' -> '%s'",
         nFullUrl.c_str(),
         nFullDest.c_str());

      // code below mostly lifed from Microsoft example
      // https://learn.microsoft.com/en-us/windows/win32/
      //    api/winhttp/nf-winhttp-winhttpreaddata

      // Use WinHttpOpen to obtain a session handle.
      HINTERNET hSession = ::WinHttpOpen(
         L"cdwe gordian/1.0",
         WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
         WINHTTP_NO_PROXY_NAME,
         WINHTTP_NO_PROXY_BYPASS,
         0);
      if(!hSession)
         throw std::runtime_error("error from Win32 HTTP library 1");

      // Specify an HTTP server.
      //auto wUrl = widen("cdwe-gordian.infinityfreeapp.com");
      auto wUrl = widen(m_baseUrl);
      HINTERNET hConnect = ::WinHttpConnect(
         hSession,
         wUrl.c_str(),
         INTERNET_DEFAULT_HTTP_PORT,
         0);
      if(!hConnect)
      {
         DWORD e = ::GetLastError();
         m_pLog->writeLn("windows error %ld",e);
         throw std::runtime_error("error from Win32 HTTP library 2");
      }

      // Create an HTTP request handle.
      //auto wObj = widen("gordian-8-win32-rel.ar.z.s.sg0");
      auto wObj = widen(leafUrl);
      const wchar_t *pT1 = L"text/*";
      const wchar_t *pT2 = L"application/javascript";
      const wchar_t *types[] = { pT1, pT2, NULL };
      HINTERNET hRequest = ::WinHttpOpenRequest(
         hConnect,
         L"GET",
         wObj.c_str(),
         L"HTTP/4.0",
         WINHTTP_NO_REFERER,
         types,//WINHTTP_DEFAULT_ACCEPT_TYPES,
         0);//WINHTTP_FLAG_SECURE);
      if(!hRequest)
         throw std::runtime_error("error from Win32 HTTP library 3");

      // Send a request.
      auto bResults = ::WinHttpSendRequest(
         hRequest,
         WINHTTP_NO_ADDITIONAL_HEADERS,
         0,
         WINHTTP_NO_REQUEST_DATA,
         0,
         0,
         0);
      if(!bResults)
      {
         DWORD e = ::GetLastError();
         if(e == 12007)
            throw std::runtime_error("can't connect; please check signal");
         else
         {
            m_pLog->writeLn("windows error %ld",e);
            throw std::runtime_error("error from Win32 HTTP library 4");
         }
      }

      // End the request.
      bResults = ::WinHttpReceiveResponse(hRequest,NULL);
      if(!bResults)
         throw std::runtime_error("error from Win32 HTTP library 5");

      // Keep checking for data until there is nothing left.
      DWORD dwSize = 0;
      {
         tcat::typePtr<file::iFileManager> fMan;
         fMan->createAllFoldersForFile(nFullDest.c_str(),*m_pLog,true);
      }
      cmn::autoCFilePtr out(nFullDest,"wb");
      do
      {
         // Check for available data.
         dwSize = 0;
         if(!::WinHttpQueryDataAvailable(hRequest,&dwSize))
            throw std::runtime_error("error from Win32 HTTP library 6");

         // No more available data.
         if(!dwSize)
            break;

         // Allocate space for the buffer.
         cmn::sizedAlloc block;
         block.realloc(dwSize);

         // Read the Data.
         DWORD dwDownloaded = 0;
         if(!::WinHttpReadData(
            hRequest,
            (LPVOID)block.ptr(),
            dwSize,
            &dwDownloaded))
            throw std::runtime_error("error from Win32 HTTP library 7");
         else
            ::fwrite(block.ptr(),1,block.size(),out.fp);

         // This condition should never be reached since WinHttpQueryDataAvailable
         // reported that there are bits to read.
         if(!dwDownloaded)
            break;
      } while(dwSize);

      // Close any open handles.
      ::WinHttpCloseHandle(hRequest);
      ::WinHttpCloseHandle(hConnect);
      ::WinHttpCloseHandle(hSession);
   }

private:
   console::iLog *m_pLog;
   std::string m_baseUrl;
   std::string m_baseDest;
};

tcatExposeTypeAs(httpReader,iHttpReader);

} // namespace http

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
