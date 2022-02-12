#ifndef ___http_api___
#define ___http_api___

namespace http {

class iHttpReader {
public:
   virtual ~iHttpReader() {}
   virtual void read() = 0;
};

} // namespace http

#endif // ___http_api___
