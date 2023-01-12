#ifndef ___archive_keyExpert___
#define ___archive_keyExpert___

#include <cstddef>

namespace console { class iLog; }

namespace keys {

class iKeyExpert {
public:
   enum modes {
      kList   = 0x1,
      kRenew  = 0x2,
      kFree   = 0x4,
      kExport = 0x8
   };

   virtual ~iKeyExpert() {}
   virtual void manageKeys(size_t modes, console::iLog& l) = 0;
};

} // namespace keys

#endif // ___archive_keyExpert___
