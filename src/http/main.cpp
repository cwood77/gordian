#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>

int main(int , char *[])
{
	printf("chris says hi\r\n");
	WinHttpOpen(NULL,0,0,0,0);
	return 0;
}
