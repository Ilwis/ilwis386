class ForeignFormat;

typedef ForeignFormat* (*CreateImportFunction)(const FileName& fnObj, ParmList& pm);
typedef ForeignFormat* (*CreateQueryFunction)();

struct _export ForeignFormatFuncs {
	ForeignFormatFuncs(CreateImportFunction f1=NULL, CreateQueryFunction f2=NULL) : importFunc(f1), queryFunc(f2) {}
	CreateImportFunction importFunc;
	CreateQueryFunction	queryFunc;
};

typedef void (*ForeignFormatInfo)(map<String, ForeignFormatFuncs> *funcs);

class _export ForeignFormatMap {
public:
	void AddFormats();
	ForeignFormatFuncs *operator[](const String& sMethod);

private:
	void AddModule(const FileName& fn);

	map<String, ForeignFormatFuncs> mpCreateFunctions;           		
};