#include <exception>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "api.hpp"
#include "../tcatlib/api.hpp"

class dummy : public test::iAsserter {
public:
   virtual void thing() {}
};

int main(int , char *[])
{
   printf("looking for tests\r\n");

   try
   {
      tcat::typeSet<test::iTest> tests;
      printf("found %lld\r\n", tests.size());
      for(size_t i=0;i<tests.size();i++)
      {
         test::iTest *pInstance = tests[i];
         dummy d;
         pInstance->run(d);
      }
      printf("done\r\n");
   }
   catch(std::exception& x)
   {
      printf("error: %s\n",x.what());
   }

   return 0;
}
