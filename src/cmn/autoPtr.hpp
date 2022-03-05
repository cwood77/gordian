#ifndef ___cmn_autoPtr___
#define ___cmn_autoPtr___

#include <cstddef>
#include <stdio.h>
#include <string>

namespace cmn {

template<size_t N = 4096>
class block {
public:
   char b[N];
};

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

   T *abdicate()
   {
      T *pRval = m_pPtr;
      m_pPtr = NULL;
      return pRval;
   }

private:
   T *m_pPtr;

   template<class O> autoReleasePtr(const autoReleasePtr<O>&);
   template<class O> autoReleasePtr<O>& operator=(const autoReleasePtr<O>&);
};

class autoCFilePtr {
public:
   autoCFilePtr(const std::string& path, const std::string& flags)
   : fp(NULL)
   {
      fp = ::fopen(path.c_str(),flags.c_str());
   }

   ~autoCFilePtr()
   {
      ::fclose(fp);
   }

   long calculateFileSizeFromStart()
   {
      ::fseek(fp,0,SEEK_END);
      long size = ::ftell(fp);
      ::fseek(fp,0,SEEK_SET);
      return size;
   }

   long calculateFileSizeFromHere()
   {
      long here = ::ftell(fp);
      ::fseek(fp,0,SEEK_END);
      long size = ::ftell(fp);
      ::fseek(fp,here,SEEK_SET);
      return size;
   }

   template<size_t N>
   size_t readBlock(block<N>& buffer, size_t n = N)
   {
      return ::fread(buffer.b,1,n,fp);
   }

   template<size_t N>
   void writeBlock(block<N>& buffer, size_t n = N)
   {
      ::fwrite(buffer.b,1,n,fp);
   }

   std::string readString()
   {
      auto n = read<unsigned long>();
      char *pStr = new char[n];
      ::fread(pStr,sizeof(char),n,fp);
      std::string rval(pStr,n);
      delete [] pStr;
      return rval;
   }

   void writeString(const std::string& s)
   {
      write<unsigned long>(s.length());
      ::fwrite(s.c_str(),sizeof(char),s.length(),fp);
   }

   template<class D>
   D read(bool *pSuccess = NULL)
   {
      D d;
      auto s = ::fread(&d,sizeof(D),1,fp);
      if(pSuccess)
         *pSuccess = (s != 0);
      return d;
   }

   template<class D>
   void write(const D& v)
   {
      ::fwrite(&v,sizeof(D),1,fp);
   }

   FILE *fp;

private:
   autoCFilePtr(const autoCFilePtr&);
   autoCFilePtr& operator=(const autoCFilePtr&);
};

} // namespace cmn

#endif // ___cmn_autoPtr___
