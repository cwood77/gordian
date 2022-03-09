#ifndef ___archive_keyExpert___
#define ___archive_keyExpert___

#include <cstddef>

namespace console { class iLog; }

namespace keys {

class iKeyExpert {
public:
   enum modes {
      kList,
      kRenew,
      kFree
   };

   virtual ~iKeyExpert() {}
   virtual void manageKeys(size_t modes, console::iLog& l) = 0;
};

} // namespace keys

#endif // ___archive_keyExpert___
