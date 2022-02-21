//class compositeInstr;
//class copyTreeInstr;
//class deleteTreeInstr;
class addToPathInstr;
class removeFromPathInstr;
class batchFileInstallInstr;
class batchFileUninstallInstr;

class instrBase : public iInstr {
public:
   virtual iInstr *invert() = 0;
};

class iInstrBuilder {
public:
   virtual iInstr *populate(sst::dict& d, bool install) = 0;
};

//  "install-steps": [
//     { "type": "batch", "package-path": "dir\\file.bat" }
//     { "type": "batch", "lib-path": "file.bat" }
//     { "type": "add-to-path" }
//     { "type": "remove-from-path" }
//  ]
