#include <exception>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../http/api.hpp"
#include "../tcatlib/api.hpp"

int main(int , char *[])
{
   printf("looking for tests\r\n");

   try
   {
      tcat::typeSet<http::iHttpReader> tests;
      for(size_t i=0;i<tests.size();i++)
      {
         http::iHttpReader *pInstance = tests[i];
         pInstance->read();
      }
   }
   catch(std::exception& x)
   {
      printf("error: %s\n",x.what());
   }

   return 0;
}
