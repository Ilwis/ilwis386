#include "Headers\toolspch.h"
#include "Engine\Base\System\module.h"
#include "Engine\Base\DataObjects\Version.h"
#include "Headers\Hs\ilwisgen.hs"

using namespace ILWIS;

bool ILWIS::Version::fSupportsBinaryVersion(ILWIS::Version::BinaryVersion version, bool throwError) {
	for(vector<BinaryVersion>::iterator cur = binaryVersions.begin(); cur != binaryVersions.end(); ++cur) {
		if ( (*cur) == version)
			return true;
	}
	if ( throwError) {
		String sVers = BinaryVersionNumberToString(version);
		throw ErrorObject(String(TR("This binary version( %S) of the data file(s) is not supported").c_str(), sVers));
	}
	return false;
}

bool ILWIS::Version::fSupportsODFVersion(const String& version, bool throwError){
	for(vector<String>::iterator cur = supportedODFVersions.begin(); cur != supportedODFVersions.end(); ++cur) {
		if (( *cur) == version)
			return true;
	}
	if ( throwError)
		throw ErrorObject(String(TR("This Object definition version( %S) of the data file(s) is not supported").c_str(), version));

	return false;
}
void ILWIS::Version::addBinaryVersion(ILWIS::Version::BinaryVersion v) {
	if ( find(binaryVersions.begin(), binaryVersions.end(), v) == binaryVersions.end())
		binaryVersions.push_back(v);
}
void ILWIS::Version::addODFVersion(const String& v) {
	if ( find(supportedODFVersions.begin(), supportedODFVersions.end(), v) == supportedODFVersions.end())
		supportedODFVersions.push_back(v);
}

String ILWIS::Version::BinaryVersionNumberToString(ILWIS::Version::BinaryVersion v) const {
	switch (v) {
		case ILWIS::Version::bvFORMAT14:
			return "Format 1.4";
		case ILWIS::Version::bvFORMAT20:
			return "Format 2.0";
		case ILWIS::Version::bvFORMAT30:
			return "Format 3.0";
		case ILWIS::Version::bvFORMATFOREIGN:
			return "Foreign Format";
		case ILWIS::Version::bvPOLYGONFORMAT37:
			return "Polygon Format 3.7";
		default:
			return "?";
	};
	return "?";
}

String ILWIS::Version::ModuleVersionNumberToString(ILWIS::Module::ModuleInterface v) {
	switch (v) {
		case ILWIS::Module::mi36:
			return "Module Interface 3.6";
		case ILWIS::Module::mi37:
			return "Module Interface 3.7";
		case ILWIS::Module::mi38:
			return "Module Interface 3.8";
		default:
			return "?";
	};
	return "?";
}

bool ILWIS::Version::fSupportsModuleInterfaceVersion(ILWIS::Module::ModuleInterface version, const String& module, bool throwError) {
	for(vector<ILWIS::Module::ModuleInterface>::iterator cur = moduleVersions.begin(); cur != moduleVersions.end(); ++cur) {
		if ( (*cur) == version)
			return true;
	}
	if ( throwError) {
		String sVers = ModuleVersionNumberToString(version);
		throw ErrorObject(String(TR("This module interface version( %S) of the plugin %S is not supported").c_str(), sVers, module));
	}
	return false;

}
void ILWIS::Version::addModuleInterfaceVersion(ILWIS::Module::ModuleInterface inf) {
	if ( find(moduleVersions.begin(), moduleVersions.end(), inf) ==  moduleVersions.end())
		moduleVersions.push_back(inf);
}



