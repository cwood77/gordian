#include <exception>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>
#include "../tcatlib/api.hpp"

class iFakeThing {
};

int main(int , char *[])
{
	printf("chris says hi\r\n");
   if(false)
      WinHttpOpen(NULL,0,0,0,0);

   try
   {
      tcat::typePtr<iFakeThing> pFake;
   }
   catch(std::exception& x)
   {
      printf("error: %s\n",x.what());
   }

	return 0;
}
