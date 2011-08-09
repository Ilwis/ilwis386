/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include <set>
#include "Headers\Hs\OTHEROBJ.hs"

ObjectStructure::ODFEntry::ODFEntry()
{
}

ObjectStructure::ODFEntry::ODFEntry(const FileName& fnValue, const String& _sSection, const String& _sEntry, bool _fReLinkContainerPaths ) :
	sFile(fnValue.sFullPath()),
	fnFile(fnValue)	,	
	sSection(_sSection),
	sEntry(_sEntry),
	fReLinkContainerPaths(_fReLinkContainerPaths),
	fFirst(true)
{
}

ObjectStructure::ODFEntry::ODFEntry(const FileName& _fnODF, const FileName& _fnF, const String& sValue, const String& _sSection, const String& _sEntry, bool _fReLinkContainerPaths ) :
	sFile(sValue),
	sSection(_sSection),
	sEntry(_sEntry),
	fnODF(_fnODF),
	fnFile(_fnF),
	fReLinkContainerPaths(_fReLinkContainerPaths),
	fFirst(true)
{
}	

bool ObjectStructure::ODFEntry::operator==(const ODFEntry& entry)	
{
	return sFile == entry.sFile &&
		     fCIStrEqual(sSection, entry.sSection) &&
				 fCIStrEqual(sEntry, entry.sEntry);
}
//-------------------------------------------------------------------------
ObjectStructure::ObjectStructure() :
 fRetrieveAssociatedFiles(true),
	fAcceptSystemObjects(false),
	fAbsolutePaths(false),
	fRetrieveEntireCollection(true),
	caAction(ObjectStructure::caANY),
	fCopyLinkedTables(true),
	fBreakDep(true)
{}	 
 
bool ObjectStructure::AddEntry(ODFEntry& entry)
{
	// check if the file is already in the list, no file may be added a second time	
	for(vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur)
	{
		String sFile1 = (*cur).fnFile.sFile + (*cur).fnFile.sExt;
		String sFile2 = entry.fnFile.sFile + entry.fnFile.sExt;
		bool fFilesTheSame = fCIStrEqual(sFile1, sFile2);
		bool fLocationTheSame = (*cur).fnFile == entry.fnFile;
		if (fFilesTheSame && fLocationTheSame) // entirely the same file
		{
			if (entry.sSection == "") // true singular ODF, not an entry in another ODF
				return false;
			entry.fFirst = false;
		}			
		if (fFilesTheSame && !fLocationTheSame)
		{
			// could be the same false but then the modified time of the file must also be the same
			long iTime1=iUNDEF, iTime2=iUNDEF;
			if ( IlwisObject::iotObjectType(entry.fnFile) != IlwisObject::iotANY)
			{
				ObjectInfo::ReadElement("Ilwis", "Time", (*cur).fnFile, iTime1);
				ObjectInfo::ReadElement("Ilwis", "Time", entry.fnFile, iTime2);
			}
			else
			{
				CFileStatus fstat1, fstat2;
				CFile::GetStatus((*cur).fnFile.sFullPath().c_str(), fstat1);
				CFile::GetStatus(entry.fnFile.sFullPath().c_str(), fstat2);
				iTime1 = fstat1.m_mtime.GetTime();
				iTime2 = fstat2.m_mtime.GetTime();				
			}				
			if ( caAction == caCOPY && (iTime1 != iTime2) || (iTime1==iUNDEF && iTime2 == iUNDEF )) // same name but different dates possible conflict
			{
				String sMessage1(TR("Name Conflict. Two different files with the same name but located in different directories\n are copied to the same directory. Copy aborted."));
				String sMessage2 = sMessage1 + String(TR("\n The files are : %S and\n %S").c_str(), (*cur).fnFile.sFullPath(), entry.fnFile.sFullPath());
				throw ErrorObject(sMessage2);
			}
			entry.fFirst = false; // ok the locations may be different but they are the same

		}
	}		
	aoeRequired.push_back(entry);	

	return entry.fFirst;
}

void ObjectStructure::AddFile(const FileName& fn)
{
	if ( fn == FileName() )
		return;
	if (!fn.fExist())
		return;

  // system objects may not be added		
	if (!fAcceptSystemObjects && ObjectInfo::fSystemObject( fn ))
		return;	

	ODFEntry entry(fn, "", "");
	AddEntry(entry);
}

void ObjectStructure::AddFile(const FileName& fnODF, const String& sSection, const String& sEntry, const String& sDefaultExtension, bool _fReLinkContainerPaths)
{ 
	String sValue;
	// retrieve value from odf.
	ObjectInfo::ReadElement(sSection.c_str(), sEntry.c_str(), fnODF, sValue);
	if ( sValue == "")
		return;	
	
	FileName fnFile(sValue);
	
	// if it has no extension add an extension
	if (  fnFile.sExt == "")
	{
		sValue += sDefaultExtension;
		fnFile.sExt += sDefaultExtension;
	}
	// check if file contains a path, if not so make sure the path is pointing to the same dir as the odf is in
	// the current dir may not be the same as the odf's. So filename automatically adds the current(wrong) dir
	// in the filename
	String sFile = fnFile.sFile + fnFile.sExt;
	if ( sFile.size() == sValue.size() )
		fnFile.Dir( fnODF.sPath());
	
  // non existing files can not be copied (!)
	if (!fnFile.fExist())
		return;

  // system objects may not be added		
	if (!fAcceptSystemObjects && ObjectInfo::fSystemObject(fnFile))
		return;

	// check if entry being used have absolute path
	if (!fIsContainer(fnODF))
		if (sSection != "" )
			if (piContainsPathInfo(sValue) == piABSOLUTE || !fCIStrEqual(fnFile.sPath(), dirBase.sFullPath()))
				fAbsolutePaths = true;

	// OK add the entry 
	ODFEntry entry(fnODF, fnFile, sValue, sSection, sEntry, _fReLinkContainerPaths);
	if (AddEntry(entry) && !_fReLinkContainerPaths)
	{
		// if the entry is an ODF, retrieve the files belonging to it
		if (IlwisObject::iotObjectType( fnFile ) != IlwisObject::iotANY)
		{
			IlwisObject obj = IlwisObject::obj(fnFile);
			if ( obj.fValid())
				obj->GetObjectStructure(*this);
		}					
	}		
}

ObjectStructure::ODFEntry* ObjectStructure::oeGetODFEntry(int i)
{

	if ( i >= 0 && i < aoeRequired.size() )
			return &(aoeRequired[i]);

	return NULL;
}


bool ObjectStructure::fObjectContainDependecies(const FileName& fnObj)
{
	return true;
}

unsigned __int64* iFileSize(ObjectStructure::ODFEntry oeEntry, unsigned __int64* iSize)
{
	String sPath = oeEntry.fnFile.sFullPath();
	HANDLE handle =CreateFile(sPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		                             FILE_ATTRIBUTE_NORMAL, NULL);
	if ( handle && handle != INVALID_HANDLE_VALUE )
	{
		*iSize += GetFileSize(handle, NULL);
	}	
	CloseHandle(handle);

	return iSize;
}

unsigned __int64 ObjectStructure::iSizeNeeded()
{
	unsigned __int64 iSize = 0;
	for_each(aoeRequired.begin(), aoeRequired.end(), bind2nd(ptr_fun(iFileSize), &iSize));
	return iSize;
}

void ObjectStructure::GetUsedODFs(list<FileName>& lstFiles)
{
	for( vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur )
	{
		// do not add files already in the list
		list<FileName>::iterator where = find(lstFiles.begin(), lstFiles.end(), (*cur).fnFile);
		if ( where == lstFiles.end() )
			lstFiles.push_back( (*cur).fnFile);
	}		
}

// Predicate function used in both GetUsedFiles functions.
// It is used to avoid having duplicate file entries in the list of files to copy
bool EqualODFEntry(ObjectStructure::ODFEntry oe1, ObjectStructure::ODFEntry oe2)
{
	return oe1.fnFile == oe2.fnFile;
}

void ObjectStructure::GetUsedFiles(list<String>& lstFiles, bool fUseAbsolutePaths)
{
	for( vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur )
	{
		// if absolute paths are used (in odf) and the user wants to keep these files at
		// that location skip the file
		// note that top level odf's ( odf's that are explcitely asked to be copied do not
		// contain an fnODF member only a fnFile member
		if ( (*cur).fnODF != FileName() )		
			if ( (fUseAbsolutePaths && piContainsPathInfo( (*cur).sFile) == piABSOLUTE) ||
				   (*cur).fReLinkContainerPaths )
				continue;

		// Now check if we already have copied this file into the list
		// This is done by checking the items in the aoeRequired vector that already have been
		// handled, using the find_if STL algorithm combined with the EqualODFEntry predicate function
		if (find_if(aoeRequired.begin(), cur, bind2nd(ptr_fun(EqualODFEntry), (*cur))) == cur)
			lstFiles.push_back((*cur).fnFile.sFullPath());
	}				
}

void ObjectStructure::GetUsedFiles(list<String>& lstFiles, list<String>& lstFilesNew, bool fUseAbsolutePaths)
{
	for( vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur )
	{
		// if absolute paths are used (in odf) and the user wants to keep these files at
		// that location skip the file
		// note that top level odf's ( odf's that are explcitely asked to be copied do not
		// contain an fnODF member only a fnFile member		
		if ( (*cur).fnODF  != FileName())		
			if ( (fUseAbsolutePaths && piContainsPathInfo((*cur).sFile) == piABSOLUTE) ||
				   (*cur).fReLinkContainerPaths ||
					 !(*cur).fFirst)
				continue;

		if ((*cur).fnFile.sPath() != dirBase.sPath() && fUseAbsolutePaths)			
			continue;

		if ( (*cur).fnDestination == (*cur).fnFile ) // should not happen, but i case of error will not corrupt the original
			continue;

		// Now check if we already have copied this file into the list
		// This is done by checking the items in the aoeRequired vector that already have been
		// handled, using the find_if STL algorithm combined with the EqualODFEntry predicate function
		if (find_if(aoeRequired.begin(), cur, bind2nd(ptr_fun(EqualODFEntry), (*cur))) == cur)
		{
			lstFiles.push_back((*cur).fnFile.sFullPath());
			lstFilesNew.push_back((*cur).fnDestination.sFullPath());
		}
	}				
}

// Check for attribute tables linked to domains.
// For all tables linked to a domain, check if they are selected for copy
// If some tables are only linked to a domain, then ask to copy linked tables
bool ObjectStructure::fFindLinkedTables(const vector<FileName>& fnFiles)
{
	for (vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur )
	{
		if (IlwisObject::iotObjectType((*cur).fnODF) == IlwisObject::iotDOMAIN)
			if ((*cur).sEntry == "AttributeTable" && (*cur).sSection.find("DomainSort") != String::npos)
			{
				// found a domain with a linked table; now try to locate the table and see if
				// it is not linked to.
				// (*cur).fnFile is the linked table
				bool fLinked = true;
				// check the original selected files for the table name; if it is included it
				// must be copied anyway
				for (vector<FileName>::const_iterator ifn = fnFiles.begin(); ifn != fnFiles.end(); ++ifn)
				{
					fLinked = (*ifn) != (*cur).fnFile;
					if (!fLinked)
						return false;
				}

				for (vector<ODFEntry>::iterator tab = aoeRequired.begin(); tab != aoeRequired.end(); ++tab )
				{
					if (IlwisObject::iotObjectType((*tab).fnFile) == IlwisObject::iotTABLE)
						if (cur != tab &&     // prevent comparison with itself
								(*tab).fnFile == (*cur).fnFile)   // are the table names equal?
						{
							fLinked &= (IlwisObject::iotObjectType((*tab).fnODF) == IlwisObject::iotDOMAIN);
						}
					if (!fLinked)
						break;    // The table needs to be copied anyway, irrespectfull of its link to the domain
				}

				if (fLinked)
					return true;  // at least one table is only linked to a domain and
				                  // may need to be copied, so ask
			}
	}
	return false;
}

void ObjectStructure::RemoveLinkedTables()
{
	for( int i=0; i < aoeRequired.size(); ++i)
	{
		if (IlwisObject::iotObjectType(aoeRequired[i].fnODF) == IlwisObject::iotDOMAIN)
			if (aoeRequired[i].sEntry == "AttributeTable" && aoeRequired[i].sSection.find("DomainSort") != String::npos)
				RemoveFile(aoeRequired[i].fnFile, aoeRequired[i].fnODF);
	}
}

void ObjectStructure::RemoveFile(const FileName fn, const FileName fnParentODF, bool fRemoveEntireObject)
{
	FileName fnSearch = fn;
	String sFile2 = fnSearch.sFile + fnSearch.sExt;

	for( vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end();)
	{
		if ( fnSearch == FileName() )
		{
			if ( (*cur).fnODF == fnParentODF )
			{
				fnSearch = (*cur).fnFile;
				sFile2 = fnSearch.sFile + fnSearch.sExt;
			}
			else
				++cur;
			
			continue;
		}

		if ( fnParentODF.fValid())
		{
			// the entries removed must belong to the same ODF as the 'parent'(ODF) file
			if ( fnParentODF != (*cur).fnODF )
			{
				++cur;
				continue;
			}
		}

		// Regular case:
		// If search file matches the name of the current entry in the list, or
		// if search file matches the name of the ODF the current entry is listed in:
		// Then remove the current entry from the list.
		// Also if the current entry is a ILWIS ODF and the parent ODF is valid, remove
		// the tree of files belonging to the parent ODF, but only in case the entire object
		// tree must be removed (fRemoveEntireObject flag = ObjectStructure::rmFULLOBJECT)
		FileName fnFile((*cur).fnFile);
		FileName fnODF = (*cur).fnODF;
		String sFile = fnFile.sFile + fnFile.sExt;
		String sODF  = fnODF.sFile + fnODF.sExt;
		if ( sFile2 == sFile || (sFile2 == sODF && fRemoveEntireObject))
		{
			aoeRequired.erase(cur);
			//if files are using this object (odf) remove them also
			if (fRemoveEntireObject && IlwisObject::iotObjectType( fnFile) != IlwisObject::iotANY && fnODF.fValid() )
			{
				RemoveFile(FileName(), fnFile, fRemoveEntireObject);
			}
		}
		else
			++cur;
	}
}

void ObjectStructure::RemoveUnNeededFiles(bool fUseAbsPaths)
{
	for( vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); )
	{
		if ( piContainsPathInfo((*cur).sFile) == piABSOLUTE && fUseAbsPaths && (*cur).fnODF.fValid())
			RemoveFile((*cur).fnFile, (*cur).fnODF);
		else
			++cur;
	}
}

ObjectStructure::PathInfo ObjectStructure::piContainsPathInfo(const String& sValue)
{
	size_t iSlash = sValue.find("\\");
	if ( iSlash != -1 )
	{
		size_t iColon = sValue.find(":");
		if ( iColon != -1 && iColon < iSlash )
			return piABSOLUTE;
		else
			return piRELATIVE;
	}
	return piNONE;
}

bool ObjectStructure::fUsesAbsolutePaths()
{
	return fAbsolutePaths;
}

bool ObjectStructure::fFilesContainRelevantPathInfo(const Directory& dirDestination) const
{
	for( vector<ODFEntry>::const_iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur )	
	{
		ODFEntry entry = (*cur);
		// as containers are handled differently in the UI their path info is not relevant
		if (entry.fnODF != FileName() && !fIsContainer( entry.fnODF ))
		{
			if ( piContainsPathInfo(entry.sFile) == piABSOLUTE )
			{
				// prevent question for missing files in source when they are already in destination
				if (entry.fnFile.sPath() != dirDestination.sPath())
					return true;
			}
		}			
	}
	return false;
}

bool ObjectStructure::fIsContainer(const FileName& fn)
{
	IlwisObject::iotIlwisObjectType iotType = IlwisObject::iotObjectType(fn);
	bool fRet =  iotType == IlwisObject::iotOBJECTCOLLECTION || 
				 iotType == IlwisObject::iotLAYOUT ||
				 iotType == IlwisObject::iotMAPVIEW; 

	if ( iotType == IlwisObject::iotMAPLIST )
	{
		int iNr;
		ObjectInfo::ReadElement("MapList", "Maps", fn, iNr);
		if ( iNr != iUNDEF)
		{
			String sMap;
			ObjectInfo::ReadElement("MapList", "Map1", fn, sMap);
			fRet =  sMap.find(":1") != -1 ? false : true;
		}
	}	
	return fRet;
}

int ObjectStructure::iFindFilePattern(const String& sOrg, const String& sPattern, int iFrom, int& iPatternLength)
{
	const String sIllegal("/*?\"<>|%='");  // characters not allowed in a filename
	size_t iFind = sOrg.find(sPattern, iFrom);
	iPatternLength = sPattern.length();
	size_t iRevFind = sOrg.find_last_of(sIllegal, iFind);
	if (iRevFind != String::npos && iRevFind < iFind - 1)
	{
		iPatternLength += iFind - iRevFind - 1;
		iFind = iRevFind + 1;
	}

	return iFind;
}

void ObjectStructure::SetPathsToDestination(const Directory& dirDestination, bool fUseAbsolutePaths)
{
	map<String, list<ODFEntry> > mpReplacePaths;
	for( vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur )
	{
		// we could have to relink the path of an domain table to its attributetable
		FileName fnDest = (*cur).fnDestination;
		bool fIsNewReadOnly = File::fReadOnly(fnDest);
		if ( IlwisObject::iotObjectType(fnDest) == IlwisObject::iotDOMAIN)
		{
			if ( fCopyLinkedTables )
			{
				FileName fnTable;
				ObjectInfo::ReadElement("DomainSort", "AttributeTable", fnDest, fnTable);
				if ( fnTable.fExist() )
				{
					// if the table will be readonly, we temporarily need to make it writable
					bool fIsTableReadOnly = File::fReadOnly(fnTable);
					if (fIsTableReadOnly)
						File::SetReadOnly(fnTable.sFullPath(), false);

					ObjectInfo::WriteElement("Table", "Domain", fnTable, fnDest);

					// switch back to its original readonly setting
					if (fIsTableReadOnly)
						File::SetReadOnly(fnTable.sFullPath(), true);
				}
			}
			else
				if (fnDest.fExist())
				{
					// make sure the file is in read/write mode
					if (fIsNewReadOnly)
						File::SetReadOnly(fnDest.sFullPath(), false);
					WritePrivateProfileString("DomainSort", "AttributeTable", NULL, fnDest.sFullPath().c_str());
					// return the file to its original read/write mode
					if (fIsNewReadOnly)
						File::SetReadOnly(fnDest.sFullPath(), true);
				}
		}
		if ( (*cur).sSection == "" ) // true ODF
			continue;

		PathInfo pi = piContainsPathInfo((*cur).sFile);
		// absolute paths that have to be changed
		if (pi == piABSOLUTE && !fUseAbsolutePaths )
		{
			// files in containers which use absolute paths need not to be relinked
			if ( fIsContainer((*cur).fnODF) && pi == piABSOLUTE && (*cur).fReLinkContainerPaths)
				continue;

			mpReplacePaths[(*cur).fnODF.sFullPath()].push_back((*cur));
		}
		// or paths in a container whose contents remained and contained no path information
		else if (pi == piNONE && fIsContainer((*cur).fnODF) && 
				!fRetrieveEntireCollection && 
				(*cur).fReLinkContainerPaths) // we do want to add other members of a maplist (e.g. domain) for rewriting the paths
		{
			mpReplacePaths[(*cur).fnODF.sFullPath()].push_back((*cur));
		}
		else if ( !(*cur).fFirst && !(*cur).fnDestination.fExist() ) // another (same) file prevented the copy, relink to that file
		{
			mpReplacePaths[(*cur).fnODF.sFullPath()].push_back((*cur));
		}
		else if ( (*cur).fnDestination.fExist() && (*cur).fnDestination.sFileExt() != (*cur).fnFile.sFileExt())
		{
			mpReplacePaths[(*cur).fnODF.sFullPath()].push_back((*cur));
		}
	}

	for(map<String, list<ODFEntry> >::iterator cur2 = mpReplacePaths.begin(); cur2 != mpReplacePaths.end(); ++cur2)
	{
		set<String> setSeenNames;
		list<ODFEntry> &lst = (*cur2).second;
		for( list<ODFEntry>::iterator cur3 = lst.begin(); cur3 != lst.end(); ++cur3)
		{
			FileName fnDestODF;
			ODFEntry entry = (*cur3);
			// if we have a data file, the name of the odf in the target directory may be deduced from
			// the name of the datafile. As the target (ODF) name may be different from the original name, we do not have a
			// direct link to the file we have to adapt to get the links correct.
			if ( entry.fnDestination.fIsIlwisDataFile() )
				fnDestODF = entry.fnDestination;
			else
			{
				// name odf is same only path is different
				fnDestODF = entry.fnODF;
				fnDestODF.Dir(entry.fnDestination.sPath());
			}
			fnDestODF.sExt = entry.fnODF.sExt;
			bool fIsNewReadOnly = File::fReadOnly(fnDestODF);
			
			FileName fnSearch(entry.fnDestination);
			
			PathInfo pi = piContainsPathInfo(entry.sFile);
			// entries must be replaced if an absolute path is used where none is wanted,
			// if container are retrieved without moving their contents
			// if a file belonging to a set is not copied because of other constraints
			if ((pi == piABSOLUTE	&& !fUseAbsolutePaths) || // absolute path, it is copied so make it relative
					(pi == piNONE && fIsContainer(fnDestODF) && !fRetrieveEntireCollection) ||
					(pi == piNONE && !fnSearch.fExist() && entry.fnFile.fExist()) ||  // it is not copied for other reasons, relink it
					(pi == piNONE && entry.fnFile.sFileExt() != fnSearch.sFileExt())) // renamed file in another dir
			{
				String sSearch = entry.sFile;
				size_t iPnt = sSearch.rfind('.');
				if ( iPnt != string::npos )
					sSearch = sSearch.substr(0, iPnt);

				// do not search for already replaced names
				if ( setSeenNames.find(sSearch) != setSeenNames.end() )
					continue;
				else
					setSeenNames.insert(sSearch);
				
				FileName fnNewEntry(fnSearch);
				if (!fnDestODF.fExist())
					continue;
				
				// make sure the destination ODF is writable
				if (fIsNewReadOnly)
					File::SetReadOnly(fnDestODF.sFullPath(), false);
				CFile file(fnDestODF.sFullPath().c_str(), CFile::modeReadWrite);
				
				String sNewFile	;
				// container entries that are not copied should be relinked and
				// files that are not copied for another reason should be relinked
				// else put the short file name there
				if ((pi == piNONE && fIsContainer(fnDestODF) && !fRetrieveEntireCollection) ||
						(pi == piNONE && !fnSearch.fExist()))
					sNewFile = entry.fnFile.sFullPathQuoted(false);
				else
					sNewFile = fnNewEntry.sFile ;// + fnNewEntry.sExt;

				size_t iSize = file.GetLength();
				char *sFile = new char[iSize + 1];
				file.Read( sFile, iSize);
				sFile[iSize] = 0;
				String sODF(sFile);
				String sTempODF(sODF);
				sTempODF.toLower(); // used for case insensitive compare
				sSearch.toLower();
				String sNewODF;
				int iWhere = -1;
				int iStart = 0;
				// will search through the file to replace occurence of sSearch with sNewFile. Note
				// that both vars (sSearch and sNewFile) do not(!) contain an extension. The extension 
				// needed will be determined locally. In this way also references to table columns will
				// be automatically changed
				int iPatternLength;  // length of pattern found
				while( (iWhere = iFindFilePattern(sTempODF, sSearch, iStart, iPatternLength)) != String::npos) //sTempODF.find(sSearch, iStart)) != -1 )
				{
					String sFront = sODF.substr( iStart, iWhere - iStart);
					String sFile = sODF.substr(iWhere, iPatternLength + 4);
					PathInfo piPattern = piContainsPathInfo(sFile);
					int iPnt = sFile.rfind('.');  // if no point it is not a file, leave it alone
					bool fDomainOrRpr = false;
					if ( iPnt == -1)
					{
						// exceptions to the above, should not be but allas
						// Domain and Representation names can occur without extension,
						// so a pure text match is needed
						String sRprKey = string("Representation=");
						String sDomKey = string("Domain=");
						unsigned int iP = sFront.rfind(sRprKey);
						fDomainOrRpr = iP != string::npos && iP == max(0, sFront.size() - sRprKey.size()) ;
						if (!fDomainOrRpr)
						{
							iP = sFront.rfind(sDomKey);
							fDomainOrRpr = iP != string::npos && iP == max(0, sFront.size() - sDomKey.size());
						}
					}

					if ( iPnt != -1 || fDomainOrRpr )
					{
						FileName fnFileEntry(sFile);
						fnFileEntry.Dir(dirDestination.sFullPath());
						// only replace entries of files:
						// 1. that do not contain pathinfo themselves
						// 2. that do not exist in this directory
						if (piPattern != piABSOLUTE && !fnFileEntry.fExist())
						{
							String sFileExt = iPnt != string::npos ? sFile.substr(iPnt) : "";
							sFront += sNewFile + sFileExt;
						}
						else
						{
							// add file name to the entry
							if ( fUseAbsolutePaths)
								sFront += sFile;
							else
								sFront += fnFileEntry.sFile + fnFileEntry.sExt;
						}	
						// calculate new point to start searching from
						int iExt = fnFileEntry.sExt.length();
						iStart = iWhere + iPatternLength + iExt;  // with extension
					}
					else
					{
						String sOld = sODF.substr(iWhere, iPatternLength);
						sFront += sOld; // leave everything as it was
						iStart = iWhere + iPatternLength; // no ext.
					}

					sNewODF += sFront;
				}
				String sRest = sODF.substr(iStart); // the rest;
				sNewODF += sRest;
				file.SetLength(0); // wipe the original;
				file.Write(sNewODF.c_str(), sNewODF.size());
				file.Close();
				delete sFile;
				// restore the original read/write mode of the current ODF
				if (fIsNewReadOnly)
					File::SetReadOnly(fnDestODF.sFullPath(), true);
			}
		}
	}
}

// SetPathsToDestination() is called in case of copying in the same directory
void ObjectStructure::SetPathsToDestination()
{
	for( vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur )	
	{
		if ( (*cur).sSection != "" )
		{
			PathInfo pi = piContainsPathInfo((*cur).sFile);
			FileName fnDest = (*cur).fnDestination;
			String sNewFile;
			if ( pi == piNONE)
				sNewFile = fnDest.sFile + fnDest.sExt;
			else
				sNewFile = fnDest.sPath() + "\\" + fnDest.sFile + fnDest.sExt;

			FileName fnDestODF = (*cur).fnODF;
			if ( fnDestODF.fExist() )
			{
				// if the copied file will be readonly, we temporarily need to make
				// it writable to be able to adjust all section to have the correct
				// file references
				bool fIsNewReadOnly = File::fReadOnly(fnDestODF);
				if (fIsNewReadOnly)
					File::SetReadOnly(fnDestODF.sFullPath(), false);

				ObjectInfo::WriteElement((*cur).sSection.c_str(), (*cur).sEntry.c_str(), fnDestODF, sNewFile);
				// exception needed for crosslinked dom/rpr
				if ( IlwisObject::iotObjectType(fnDestODF) == IlwisObject::iotDOMAIN )
				{
					FileName fnRpr;
					ObjectInfo::ReadElement("Domain", "Representation", fnDestODF, fnRpr);
					if ( fnRpr.fExist() )
					{
						ObjectInfo::WriteElement("Representation", "Domain", fnRpr, fnDestODF.sRelative());
						// In case of rprclass update the domain in the table section as well
						FileName fnDom;
						int iRead = ObjectInfo::ReadElement("Table", "Domain", fnRpr, fnDom);
						if (iRead > 0)
							ObjectInfo::WriteElement("Table", "Domain", fnRpr, fnDestODF.sRelative());
					}
					// Copying domains in the same directory will always lose the attribute table.
					WritePrivateProfileString("DomainSort", "AttributeTable", NULL, fnDestODF.sFullPath().c_str());
				}
				// Now set the copied ODF file in readonly again if it had been changed temporarily only
				if (fIsNewReadOnly)
					File::SetReadOnly(fnDestODF.sFullPath(), true);
			}
		}
	}
}

bool ObjectStructure::fGetAssociatedFiles()
{
	return fRetrieveAssociatedFiles && !fAcceptSystemObjects;
}

FileName ObjectStructure::fnODFFileNamePresent(const FileName& fn)
{
	if ( IlwisObject::iotObjectType( fn) == IlwisObject::iotANY )
		return FileName();
	
	for( vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur )
	{
		String sFile = (*cur).fnFile.sFile + (*cur).fnFile.sExt;
		String sFile2 = fn.sFile + fn.sExt;	
		if ( fCIStrEqual(sFile, sFile2) )
			return (*cur).fnFile;
	}	
	return FileName();
}

bool ObjectStructure::fCopyToReadOnly(const FileName& fn)
{
	for( vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur )
	{
		String sFile = (*cur).fnFile.sFile + (*cur).fnFile.sExt;
		String sFile2 = fn.sFile + fn.sExt;	
		if ( fCIStrEqual(sFile, sFile2) )
		{
			return File::fReadOnly( fn);
		}	
	}		
	return false;
}

void ObjectStructure::RetrieveEntireCollection(bool fYesNo)
{
	fRetrieveEntireCollection = fYesNo;
}

bool ObjectStructure::fRetrieveContentsContainer()
{
	return fRetrieveEntireCollection;
}

void ObjectStructure::RemoveSuperflousCollectionReferences(const Directory& dirDestination)
{
	for( vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur )
	{
		if ( ObjectInfo::fInCollection((*cur).fnFile) )
		{
			int iNr;
			String sCollection;
			ObjectInfo::ReadElement("Collection", "NrOfItems", (*cur).fnFile, iNr);			
			if ( iNr > 0)
			{
				for (int i=0; i < iNr; ++i)
				{
					String sKey("Item%d", i++);
					ObjectInfo::ReadElement("Collection", sKey.c_str(), (*cur).fnFile, sCollection);	
					FileName fnCol(sCollection);
					fnCol.Dir(dirDestination.sFullPath());
					bool fFound = false;
					for( vector<ODFEntry>::iterator cur2 = aoeRequired.begin(); cur2 != aoeRequired.end(); ++cur2 )	
					{
						if ( (*cur2).fnDestination == fnCol)
							fFound = true;
					}	
					if (!fFound)
					{
						FileName fn = (*cur).fnDestination;
						//fn.Dir(dirDestination.sFullPath());
						if ( fn.fExist() )
							IlwisObjectPtr::RemoveCollectionFromODF(fn, sCollection);
					}						
				}
			}				
		}
	}
}	

void ObjectStructure::GetAssociatedFiles(bool fYesNo)
{
	fRetrieveAssociatedFiles = fYesNo;
}

// Set the destination for the object fnOld to the new location fnNew; replace
// all appearances of fnOld in the ObjectStructure (only ODFEntry::fnDestination and ODFEntry::fnODF sections)
void ObjectStructure::SetDestinationFile(const FileName& fnOld, const FileName& fnNew)
{
	String sNewExt = "";
	// saveas may force a rename of the datafile to	the type belonging to the new type
	// do this only if it in the same dir, else other functionality will take care of this
	if ( fnOld.sPath() == fnNew.sPath() && fnOld.sExt != fnNew.sExt )
	{
		if ( IlwisObject::iotObjectType(fnNew) == IlwisObject::iotTABLE) // for the moment only case, others may follow
			sNewExt = ".tb#";
	}				
	for( vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur )	
	{
		// case 1: Give the destination (in ODFEntry::fnFile) object its new name
		if ( (*cur).fnFile == fnOld )
		{
			// Rename the object (after copy) with the name fnOld to fnNew
			(*cur).fnDestination = fnNew;
			// Restore the directory if the copy is to the same directory
			if ( fnNew.sFile == "") // is dir
			{
				(*cur).fnDestination.sFile = fnOld.sFile;			
				(*cur).fnDestination.sExt = fnOld.sExt;
			}				
		}
		// case 2: The owner object (in ODFEntry::fnODF) needs the new name
		else if ( ( *cur).fnODF == fnOld)
		{
			if ( (*cur).fnFile.fIsIlwisDataFile() )	
			{
				// The current entry is a datafile object in the section of the owner fnODF
				// for the datafile: always use the same basename as the owner object 
				(*cur).fnDestination = fnNew;	
				(*cur).fnDestination.sExt = (*cur).fnFile.sExt;  // set the correct datafile extension

				// if the type of an object changes (e.g. saveas) change the extension of datafile 
				if (sNewExt.length() > 0)
					(*cur).fnDestination.sExt = sNewExt;
			}	
			else
			{
				// The current entry is any object (even non-ILWIS, fe UseAs) in the section of the owner fnODF
				// The new name is set to its original name, but in the same path as its owner object
				(*cur).fnDestination = (*cur).fnFile;
				(*cur).fnDestination.Dir(fnNew.sPath());
			}				
			// Make sure that the object reference points to the correct owner object
			(*cur).fnODF = fnNew;				
		}
	}		
}

void ObjectStructure::AcceptSystemObject(bool fYesNo)
{
	fAcceptSystemObjects = true;
}

bool ObjectStructure::fBreakDependencies()
{
	return fBreakDep;
}

void ObjectStructure::SetBreakDependencies(bool fYesNo)
{
	fBreakDep = fYesNo;
}

void ObjectStructure::SetBaseDir(const Directory& dir)
{
	dirBase = dir;
}

ObjectStructure::CommandAction ObjectStructure::caGetCommandAction() const
{
	return caAction;
}

void ObjectStructure::SetCommandAction(ObjectStructure::CommandAction act)
{
	caAction = act;
}

void ObjectStructure::CopyLinkedTable(bool fYesNo) 
{
	fCopyLinkedTables = fYesNo;
}

bool ObjectStructure::fCopyLinkedTable() const
{
	return fCopyLinkedTables;
}

void ObjectStructure::BreakDependencies()
{
	if ( fBreakDependencies())
	{
		String sCurDir  = getEngine()->sGetCurDir();
		for( vector<ODFEntry>::iterator cur = aoeRequired.begin(); cur != aoeRequired.end(); ++cur )
		{
			if (!(*cur).fnDestination.fExist())
				continue;

			// retype of objects is only possible for non dependent objects, no test needed
			// test would fail anyway
			if ((*cur).fnFile.sExt != (*cur).fnDestination.sExt )
			{
				continue;
			}				
			getEngine()->SetCurDir((*cur).fnDestination.sPath());
			IlwisObject obj = IlwisObject::obj((*cur).fnDestination);
			if ( obj.fValid())
			{
				if (obj->fDependent())
					obj->BreakDependency();
			}				
		}
		getEngine()->SetCurDir(sCurDir);
	}		
}
