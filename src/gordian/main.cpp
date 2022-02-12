#include <exception>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../http/api.hpp"
#include "../tcatlib/api.hpp"

int main(int , char *[])
{
	printf("chris says hi\r\n");

   try
   {
      tcat::typePtr<http::iHttpReader> pReader;
   }
   catch(std::exception& x)
   {
      printf("error: %s\n",x.what());
   }

	return 0;
}
