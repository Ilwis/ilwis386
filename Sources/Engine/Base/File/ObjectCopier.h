#pragma once

class ObjectCopier : public BaseCopier
{
public:
	ObjectCopier(void);
	ObjectCopier(const vector<FileName>& files, const Directory& _dirDestination);
	_export ObjectCopier(const FileName& fnFile, const Directory& _dirDestination );
	_export ObjectCopier(const FileName& fnFile, const FileName& _fnDestination );
	_export static void CopyFiles(const FileName& fnInitial, const FileName& _fnDestination, bool fBreakDep);
	bool GatherFiles(ObjectStructure& osStruct, bool fBreakDep);
	void _export Copy(bool fBreakDependencies);
	bool ResolveCollisions(ObjectStructure& osStruct);
	bool SetNewNames(ObjectStructure& osStruct, bool fUseAbsolutePaths);
};
