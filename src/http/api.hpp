#ifndef ___http_api___
#define ___http_api___

namespace console { class iLog; }

namespace http {

class iHttpReader {
public:
   virtual ~iHttpReader() {}

   virtual void tie(console::iLog& l) = 0;

   virtual void bind(const std::string& baseUrl, const std::string& baseDest) = 0;
   virtual void download(const std::string& leafUrl) = 0;
};

} // namespace http

#endif // ___http_api___
