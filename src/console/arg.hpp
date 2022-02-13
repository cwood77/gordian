#ifndef ___console_arg___
#define ___console_arg___

#include <list>
#include <set>
#include <string>

namespace console {

// supported command-line formats
//
// gordian
// gordian init
// gordian info
// gordian install -y <package>
// gordian install --yes <package>
// gordian seturl <localhost>
// gordian upgrade <package>
// gordian purge

// result of parsing - user defined
class iCommand {
public:
   virtual ~iCommand() {}
};

// participates in parsing
class iArgPattern {
public:
   virtual ~iArgPattern() {}
   virtual bool tryMatchWord(const std::string& word) = 0;
   virtual void adjustPatterns(std::list<iArgPattern*>& list) = 0;
   virtual iCommand *complete() = 0;
};

// the top-level parsing object
class commandLineParser {
public:
   commandLineParser& addVerb(iArgPattern& v);

   iCommand *parse(int argc, const char *argv[]);

private:
   std::list<iArgPattern*> m_patterns;
};

// participates in post-parse validation
class iCommandVerifier : public iArgPattern {
public:
   virtual void verify() = 0;
};

// an element that acquires state based on the parse
class iCommandConfig : public iCommandVerifier {
public:
   virtual void collaborate(iCommand& c, iCommandVerifier& verb) = 0;
};

// an input
class stringParameter : public iCommandConfig {
public:
   explicit stringParameter(size_t offset);

   virtual bool tryMatchWord(const std::string& word);
   virtual void adjustPatterns(std::list<iArgPattern*>& list);
   virtual iCommand *complete();
   virtual void verify();
   virtual void collaborate(iCommand& c, iCommandVerifier& verb);

private:
   size_t m_offset;
   iCommand *m_pCmd;
   iCommandVerifier *m_pVerb;
   bool m_read;
};

// a switch
class iOption : public iCommandConfig {
public:
   virtual iOption& addTag(const std::string& tag) = 0;
};

// a boolean switch
class boolOption : public iOption {
public:
   boolOption(const std::string& tag, size_t offset);

   virtual bool tryMatchWord(const std::string& word);
   virtual void adjustPatterns(std::list<iArgPattern*>& list);
   virtual iCommand *complete();
   virtual void verify() {}
   virtual void collaborate(iCommand& c, iCommandVerifier& verb);
   virtual iOption& addTag(const std::string& tag);

private:
   std::set<std::string> m_tags;
   size_t m_offset;
   iCommand *m_pCmd;
   iCommandVerifier *m_pVerb;
};

// verbs own options, paramters, and commands, but nobody owns verbs
class verbBase : public iCommandVerifier {
public:
   virtual ~verbBase();

   verbBase& addParameter(iCommandConfig& c);
   iOption& addOption(iOption& o);

   virtual bool tryMatchWord(const std::string& word);
   virtual void adjustPatterns(std::list<iArgPattern*>& list);
   virtual iCommand *complete();
   virtual void verify();

protected:
   verbBase(iCommand& c, const std::string& tag);

private:
   iCommand *m_pCmd;
   std::string m_tag;
   iCommandConfig *m_pParam;
   std::list<iCommandConfig*> m_options;
};

template<class T>
class verb : public verbBase {
public:
   verb(const std::string& tag) : verbBase(*new T(), tag) {}
};

} // namespace console

#endif // ___console_arg___
