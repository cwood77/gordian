#define WIN32_LEAN_AND_MEAN
#include "../tcatlib/api.hpp"
#include <windows.h>

#ifdef cdwTest
#include "../tcatlib/api.hpp"
#include "../test/api.hpp"
#include "arg.hpp"
#include <stddef.h>

using namespace console;

class installCommand : public iCommand {
public:
   installCommand() : doit(false) {}

   std::string packageName;
   bool doit;
};

class infoCommand : public iCommand {
public:
};

testDefineTest(arg_verbSelection)
{
   verb<installCommand> install("install");
   verb<infoCommand> info("info");

   { // install case
      int argc = 1+1;
      const char *argv[] = { "EXE name", "install" };

      tcat::typePtr<iCommandLineParser> pParser;
      (*pParser)
         .addVerb(install)
         .addVerb(info);
      iCommand *pC = pParser->parse(argc,argv);
      a.assertTrue(dynamic_cast<installCommand*>(pC)!=NULL);
   }

   { // info case
      int argc = 1+1;
      const char *argv[] = { "EXE name", "info" };

      tcat::typePtr<iCommandLineParser> pParser;
      (*pParser)
         .addVerb(install)
         .addVerb(info);
      iCommand *pC = pParser->parse(argc,argv);
      a.assertTrue(dynamic_cast<infoCommand*>(pC)!=NULL);
   }

   { // no args is ok
      int argc = 0+1;
      const char *argv[] = { "EXE name" };

      tcat::typePtr<iCommandLineParser> pParser;
      (*pParser)
         .addVerb(install)
         .addVerb(info);
      iCommand *pC = pParser->parse(argc,argv);
      a.assertTrue(pC==NULL);
   }

   { // bogus arg throws
      int argc = 1+1;
      const char *argv[] = { "EXE name", "unknown" };

      try
      {
         tcat::typePtr<iCommandLineParser> pParser;
         (*pParser)
            .addVerb(install)
            .addVerb(info);
         pParser->parse(argc,argv);
         a.assertTrue(false);
      }
      catch(std::exception&)
      {
         a.assertTrue(true);
      }
   }

   { // multiple verbs throws
      int argc = 2+1;
      const char *argv[] = { "EXE name", "install", "info" };

      try
      {
         tcat::typePtr<iCommandLineParser> pParser;
         (*pParser)
            .addVerb(install)
            .addVerb(info);
         pParser->parse(argc,argv);
         a.assertTrue(false);
      }
      catch(std::exception&)
      {
         a.assertTrue(true);
      }
   }
}

testDefineTest(arg_stringParams)
{
   { // happy path
      verb<installCommand> install("install");
      install.addParameter(
         stringParameter::required(offsetof(installCommand,packageName)));

      int argc = 2+1;
      const char *argv[] = { "EXE name", "install", "foo" };

      tcat::typePtr<iCommandLineParser> pParser;
      (*pParser)
         .addVerb(install);
      installCommand& c = dynamic_cast<installCommand&>(*pParser->parse(argc,argv));
      a.assertTrue(c.packageName == "foo");
   }

   { // missing parameter
      verb<installCommand> install("install");
      install.addParameter(
         stringParameter::required(offsetof(installCommand,packageName)));

      int argc = 1+1;
      const char *argv[] = { "EXE name", "install" };

      try
      {
         tcat::typePtr<iCommandLineParser> pParser;
         (*pParser)
            .addVerb(install);
         pParser->parse(argc,argv);
         a.assertTrue(false);
      }
      catch(std::exception&)
      {
         a.assertTrue(true);
      }
   }
}

testDefineTest(arg_boolOption)
{
   { // happy
      verb<installCommand> install("install");
      install.addParameter(
         stringParameter::required(offsetof(installCommand,packageName)));
      install.addOption(*new boolOption("--yes",offsetof(installCommand,doit))).addTag("-y");

      int argc = 3+1;
      const char *argv[] = { "EXE name", "install", "foo", "-y" };

      tcat::typePtr<iCommandLineParser> pParser;
      (*pParser)
         .addVerb(install);
      installCommand& c = dynamic_cast<installCommand&>(*pParser->parse(argc,argv));
      a.assertTrue(c.packageName == "foo");
      a.assertTrue(c.doit == true);
   }

   { // order don't matter
      verb<installCommand> install("install");
      install.addParameter(
         stringParameter::required(offsetof(installCommand,packageName)));
      install.addOption(*new boolOption("--yes",offsetof(installCommand,doit))).addTag("-y");

      int argc = 3+1;
      const char *argv[] = { "EXE name", "install", "-y", "foo" };

      tcat::typePtr<iCommandLineParser> pParser;
      (*pParser)
         .addVerb(install);
      installCommand& c = dynamic_cast<installCommand&>(*pParser->parse(argc,argv));
      a.assertTrue(c.packageName == "foo");
      a.assertTrue(c.doit == true);
   }

   { // options are optional
      verb<installCommand> install("install");
      install.addParameter(
         stringParameter::required(offsetof(installCommand,packageName)));
      install.addOption(*new boolOption("--yes",offsetof(installCommand,doit))).addTag("-y");

      int argc = 2+1;
      const char *argv[] = { "EXE name", "install", "foo" };

      tcat::typePtr<iCommandLineParser> pParser;
      (*pParser)
         .addVerb(install);
      installCommand& c = dynamic_cast<installCommand&>(*pParser->parse(argc,argv));
      a.assertTrue(c.packageName == "foo");
      a.assertTrue(c.doit == false);
   }
}

#endif // cdwTest

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
