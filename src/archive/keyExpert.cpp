#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include "keyExpert.hpp"
#include "sign.hpp"

namespace keys {

using namespace archive;

class keyExpert : public iKeyExpert {
public:
   virtual void manageKeys(size_t modes, console::iLog& l)
   {
      autoKeyStorage kstor;

      if(modes & modes::kList)
      {
         l.writeLn("existing keys for current user:");
         keyIterator kit;
         kit.start(kstor);
         while(!kit.isDone())
         {
            l.writeLn("   %S",kit.getName().c_str());
            kit.advance();
         }
      }

      if((modes & modes::kFree) || (modes & modes::kRenew))
      {
         autoKey k;
         bool success = k.tryOpen(kstor,autoKey::kSignKeyName);
         if(success)
         {
            l.writeLn("deleting gordian key");
            k.erase();
         }
         else if(modes & modes::kFree)
            l.writeLn("gordian key does not exist");
      }

      if(modes & modes::kRenew)
      {
         l.writeLn("creating gordian key");
         autoKey k;
         k.createForSign(kstor,autoKey::kSignKeyName);
      }
   }
};

tcatExposeTypeAs(keyExpert,keys::iKeyExpert);

} // namespace keys
