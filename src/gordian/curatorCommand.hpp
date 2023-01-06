#ifndef ___gordian_curatorCommand___
#define ___gordian_curatorCommand___

#include "../console/log.hpp"
#include "../curator/api.hpp"
#include <functional>

namespace gordian {

class curatorCommand {
public:
   static void compileRecipeAndSave(
      console::iLog& l,
      std::function<curator::request(void)> func);
};

} // namespace gordian

#endif // ___gordian_curatorCommand___
