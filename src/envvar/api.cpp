#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../cmn/string.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "key.hpp"
#include <sstream>
#include <stdexcept>
#include <windows.h>

namespace envvar {

class pathVar : public iPathVar {
public:
   virtual void tie(console::iLog& l) { m_pLog = &l; }

   virtual void addToPath(const std::string& path)
   {
      auto wpath = cmn::widen(path);

      autoPathKey k;
      k.tie(*m_pLog);
      k.open();

      auto before = k.read();
      auto after = process(before,wpath,/*add*/true);
      m_pLog->writeLn("writing to registry <<%S>>",after.c_str());
      k.write(after.c_str());

      broadcast();
   }

   virtual void removeFromPath(const std::string& path)
   {
      auto wpath = cmn::widen(path);

      autoPathKey k;
      k.tie(*m_pLog);
      k.open();

      auto before = k.read();
      auto after = process(before,wpath,/*add*/false);
      m_pLog->writeLn("writing to registry <<%S>>",after.c_str());
      k.write(after.c_str());

      broadcast();
   }

private:
   std::wstring process(const std::wstring& in, const std::wstring& path, bool add)
   {
      // split
      const wchar_t *pThumb = in.c_str();
      const wchar_t *pStart = pThumb;
      std::vector<std::wstring> paths;
      bool already = false;
      for(;;++pThumb)
      {
         bool isEnd = (*pThumb == 0);
         bool isWordEnd = (isEnd || (*pThumb == L';'));
         if(isWordEnd)
         {
            std::wstring word(pStart,pThumb-pStart);
            pStart = pThumb + 1;
            if(word == path)
               already = true;
            if((word != path) || add)
               if(!word.empty())
                  paths.push_back(word);
         }
         if(isEnd)
            break;
      }
      if(!already && add)
         paths.push_back(path);

      // recombine
      std::wstringstream stream;
      bool first = true;
      for(auto p : paths)
      {
         if(first)
            first = false;
         else
            stream << L";";
         stream << p;
      }
      stream << L";";

      return stream.str();
   }

   void broadcast()
   {
      ::SendMessageTimeoutW(
         HWND_BROADCAST,
         WM_SETTINGCHANGE,
         0,
         reinterpret_cast<LPARAM>(L"Environment"),
         SMTO_NORMAL,
         100,
         NULL
      );
   }

   console::iLog *m_pLog;
};

void autoPathKey::open()
{
   auto status = ::RegOpenKeyExW(
      HKEY_LOCAL_MACHINE,
      L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",
      0, // ulOptions
      KEY_QUERY_VALUE | KEY_SET_VALUE,
      &m_k
   );
   if(status != ERROR_SUCCESS)
      throw std::runtime_error("[addToPath] failed to open registry key");
   m_opened = true;
}

void autoPathKey::close()
{
   ::RegCloseKey(m_k);
   m_opened = false;
}

std::wstring autoPathKey::read()
{
   DWORD size = 0;
   auto status = ::RegQueryValueExW(
      m_k,
      L"Path",
      NULL,
      NULL,
      NULL,
      &size
   );
   if(status != ERROR_SUCCESS)
      throw std::runtime_error("[addToPath] failed to open registry key");

   cmn::sizedAllocT<wchar_t> block;
   block.realloc(size / sizeof(wchar_t));
   status = ::RegQueryValueExW(
      m_k,
      L"Path",
      NULL,
      NULL,
      (LPBYTE)block.ptr(),
      &size
   );
   if(status != ERROR_SUCCESS)
      throw std::runtime_error("[addToPath] failed to open registry key");

   return block.ptr();
}

void autoPathKey::write(const std::wstring& v)
{
   auto status = ::RegSetValueExW(
      m_k,
      L"Path",
      0,
      REG_SZ,
      (LPBYTE)v.c_str(),
      (v.length() + 1) * sizeof(wchar_t)
   );
   if(status != ERROR_SUCCESS)
      throw std::runtime_error("[addToPath] failed to open registry key");
}

tcatExposeTypeAs(pathVar,iPathVar);

} // namespace envvar

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
