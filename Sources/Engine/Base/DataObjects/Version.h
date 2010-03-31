#pragma once

#include "Engine\Base\System\module.h"

namespace ILWIS {
	class _export Version {
	public:
		enum BinaryVersion{ bvFORMAT14, bvFORMAT20, bvFORMAT30, bvFORMATFOREIGN, bvUNKNOWN, bvPOLYGONFORMAT37};

		bool fSupportsBinaryVersion(ILWIS::Version::BinaryVersion version, bool throwError = true);
		bool fSupportsODFVersion(const String& version, bool throwError=true);
		void addBinaryVersion(ILWIS::Version::BinaryVersion v);
		void addODFVersion(const String& v);
		bool fSupportsModuleInterfaceVersion(ILWIS::Module::ModuleInterface inf, const String& module,bool throwError=true);
		void addModuleInterfaceVersion(ILWIS::Module::ModuleInterface inf);
		const static String IlwisVersion;
		const static String IlwisShortName;

	private:
		String BinaryVersionNumberToString(ILWIS::Version::BinaryVersion v) const;
		static String ModuleVersionNumberToString(ILWIS::Module::ModuleInterface v);
		vector<String> supportedODFVersions;
		vector<BinaryVersion> binaryVersions;
		vector<ILWIS::Module::ModuleInterface> moduleVersions;

	};
};