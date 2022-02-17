#error just sketching

class expert {
public:
   enum commands {
      kInstall,
      kUpdate,
      kUninstall
   };

   virtual void tie(console::iLog& l) = 0;

   virtual void loadManifests(const char *path) = 0;

   virtual void list() = 0;

   iRecipe *generate(commands c, const char *wildcard) = 0;

private:
   std::map<std::string,size_t> latest;
   std::map<std::string,size_t> current;
   std::map<std::string,std::map<size_t,sst::dict*> > directory;
};

class iRecipe {
public:
   virtual void release() = 0;
   virtual void execute(file::iFileManager& f, pack::iPacker& p, console::iLog& l) = 0;
};

class iFileManager {
public:
   virtual bool folderExists(const char *) = 0;
   virtual bool createPath(const char *) = 0;
   virtual bool destroyPath(const char *) = 0;
};

class iPacker {
public:
   virtual const std::string& unpack(const char *path, console::iLog& l) = 0;
};
