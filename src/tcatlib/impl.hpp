class catalogProxy : public iCatalog { // static lib throws on error
public:
   virtual void *createSingleType(const char *pTypeName) = 0;
   virtual void *createMultipleTypes(const char *pTypeName, size_t& n) = 0;
   virtual void releaseType(void *) = 0;

private:
   iCatalog *m_pImpl;
};