#ifdef cdwTest
#include "../test/api.hpp"
#include "api.hpp"
#include <string.h>

testDefineTest(sst_write)
{
   sst::dict config;
   auto& installed = config.add<sst::array>("installed");
   installed.append<sst::dict>().add<sst::str>("name") = "some product";
   installed.append<sst::dict>().add<sst::str>("name") = "some other product";

   tcat::typePtr<sst::iSerializer> pWriter;
   std::string buffer = pWriter->write(config);

   const char *expected = "{\n   \"installed\": [\n      {\n         \"name\": \"some product\"\n      },\n      {\n         \"name\": \"some other product\"\n      }\n   ]\n}";
   a.assertTrue(buffer == expected);
   a.assertTrue(buffer.length() == ::strlen(expected));
}

#endif // cdwTest
