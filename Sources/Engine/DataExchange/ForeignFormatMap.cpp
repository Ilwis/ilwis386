#include "Headers\toolspch.h"
#include "Engine\Table\tbl.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\ForeignFormatMap.h"


void ForeignFormatMap::AddFormats()
{
	CFileFind finder;

	String path = getEngine()->getContext()->sIlwDir() + "\\Extensions\\*.dll";
	BOOL fFound = finder.FindFile(path.c_str());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnNew (finder.GetFilePath());
			AddModule(fnNew);
		}
	}   
}

void ForeignFormatMap::AddModule(const FileName& fnModule) {
	String path = fnModule.sFullPath();
	HMODULE hm = LoadLibrary(path.c_str());
	if ( hm == NULL) {
		DWORD err = GetLastError();
		int z = 0;
	}

	if ( hm != NULL ) {
		ForeignFormatInfo f = (ForeignFormatInfo)GetProcAddress(hm, "getForeignFormatInfo");
		if ( f != NULL ) {
			map<String, ForeignFormatFuncs> funcs;
			(*f)(&funcs);
			int sz = funcs.size();
			if ( sz > 0 ) {
				for(map<String, ForeignFormatFuncs>::iterator cur = funcs.begin(); cur != funcs.end();++cur) {
					mpCreateFunctions[(*cur).first] = (*cur).second;
				}
			}
		}
	}  
}

ForeignFormatFuncs *ForeignFormatMap::operator[](const String& sMethod) {
	map<String, ForeignFormatFuncs>::iterator cur = mpCreateFunctions.find(sMethod);
	if ( cur != mpCreateFunctions.end())
		return &(*cur).second;
	return NULL;
}