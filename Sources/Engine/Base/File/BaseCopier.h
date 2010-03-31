#pragma once

class _export BaseCopier
{
public:
	BaseCopier(const vector<FileName>& files, const Directory& _dirDestination);
	BaseCopier(const FileName& fnFile, const Directory& _dirDestination );
	BaseCopier(CWnd *parent, const FileName& fnFile, const FileName& _fnDestination );
	BaseCopier(const FileName& fnFile, const FileName& _fnDestination );
	BaseCopier(void);
	~BaseCopier(void);

protected:
	void FilesToBeCopied(const vector<FileName>& files) ;
	void FileToBeCopied( const FileName& fn);
	bool fHasWildcards(FileName & fn);
	void ExpandWildcards(vector <FileName> & fnList);
	void GetDestinationDirFiles(list<FileName>& lstFiles);
	bool fSufficientDiskSpace(unsigned __int64 & iSpaceRequired, unsigned __int64& iSpaceAvailable, String sDrive);
	void BuildCopyStrings(	list<String>& lstFilesUsed, list<String>& lstFilesNew, char **sNameBuf, char **sDestBuf, bool fUseAbsolutePaths);
	void CopyFileList(list<String>& lstFilesUsed, char *sNameBuf, char *sDestBuf);
	void ExpandWildcards(FileName & fnWildcard, vector <FileName> & fnList);

	vector<FileName>	fnFiles;
	Directory			dirDestination;
	FileName			fnDestination; // used for singel file copies where a destination name is specified
};
