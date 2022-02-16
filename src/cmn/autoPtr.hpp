#ifndef ___cmn_autoPtr___
#define ___cmn_autoPtr___

#include <cstddef>

namespace cmn {

template<class T>
class autoReleasePtr {
public:
   autoReleasePtr(T *pPtr = NULL)
   : m_pPtr(pPtr)
   {
   }

   ~autoReleasePtr()
   {
      reset();
   }

   void reset(T *pPtr = NULL)
   {
      if(m_pPtr)
         m_pPtr->release();
      m_pPtr = pPtr;
   }

   T& get()
   {
      return *m_pPtr;
   }

   T *operator->()
   {
      return m_pPtr;
   }

   T& operator*()
   {
      return *m_pPtr;
   }

private:
   T *m_pPtr;

   template<class O> autoReleasePtr(const autoReleasePtr<O>&);
   template<class O> autoReleasePtr<O>& operator=(const autoReleasePtr<O>&);
};

} // namespace cmn

#endif // ___cmn_autoPtr___
