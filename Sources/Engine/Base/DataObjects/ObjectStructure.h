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
#ifndef OBJECTSTRUCTURE_H
#define OBJECTSTRUCTURE_H

class ObjectCopierUI;
class Directory;

#include "Engine\Base\File\Directory.h"

class _export ObjectStructure
{
	friend class ObjectCopierUI;
	friend class ObjectCopier;
	
	public:
		struct ODFEntry
		{
				ODFEntry();
				ODFEntry(const FileName& fnValue, const String& _sSection="", const String& _sEntry="", bool _fReLinkContainerPaths=false );
				ODFEntry(const IlwisObject& objValue, const String& _sSection="", const String& _sEntry="" );
				ODFEntry(const FileName& _fnODF, const FileName& fnF, const String& sValue, const String& _sSection, const String& _sEntry, bool fContainer = false );
				bool operator==(const ODFEntry& entry);

				String sFile; // File entry as-is in the ODF
				FileName fnFile; // Full filename of the file
				FileName fnDestination; // For CopyTo the file has to be copied in the same dir. Here new filename
				String sSection; // which section is relevant for the file
				String sEntry; // which entry is used
				FileName fnODF; // in which ODF is the file present
				bool fFirst;   // the file is the first in the list with the same name, 
				               // multiple instances of the same file may be in the list but only one is the first.
				bool fReLinkContainerPaths; // Container copied without contents
		};
		enum CommandAction{caCOPY, caDELETE, caANY};
		enum RemoveMethod{rmFULLOBJECT = true, rmFILEONLY = false}; // How to remove files from the objectstructure file list

		ObjectStructure();

		void AddFile(const FileName& fnFile);
		void AddFile(const FileName& _fnODF, const String& _sSection, const String& sEntry, const String& sDefaultExtension = "", bool _fReLinkContainerPaths = false);
		bool fGetAssociatedFiles();
		FileName fnODFFileNamePresent(const FileName& fn);
		bool fCopyToReadOnly(const FileName& fn);
		bool fObjectContainDependecies(const FileName& fn);
		bool fRetrieveContentsContainer();
		static bool fIsContainer(const FileName& fn);
		void GetUsedFiles(list<String>& lstFiles, bool fUseAbsolutePaths);
		void GetAssociatedFiles(bool fYesNo);
		void AcceptSystemObject(bool fYesNo);
		void SetBreakDependencies(bool fYesNo);
		bool fBreakDependencies();
		void SetBaseDir(const Directory& dir);
		void SetCommandAction(ObjectStructure::CommandAction act);
		ObjectStructure::CommandAction caGetCommandAction() const;
		void CopyLinkedTable(bool fYesNo);
		bool fCopyLinkedTable() const;
		void BreakDependencies();
		void RemoveUnNeededFiles(bool fUseAbsPaths);
		bool fFilesContainRelevantPathInfo(const Directory& dirDestination) const;
		bool fFindLinkedTables(const vector<FileName>& fnFiles);
		void RemoveLinkedTables();
		
	private:
		enum PathInfo{ piABSOLUTE, piRELATIVE, piNONE};
		vector<ODFEntry> aoeRequired; // all files that directly are needed. ODF + (possible) its binary data files
	
		bool fRetrieveAssociatedFiles;
		bool fRetrieveEntireCollection;
		bool fAcceptSystemObjects;
		Directory dirBase;   // the base path of the files added to the structure. Usually the path of the top level files to be copied
		bool fAbsolutePaths; // some entries use an absolute path
		bool fBreakDep;
		bool fCopyLinkedTables; // linked attribute tables maybe copied (or not)
		ObjectStructure::CommandAction caAction;

		FileName RemoveExistingObjects(const FileName& fn);	
		void GetUsedODFs(list<FileName>& lstFiles);
		void GetUsedFiles(list<String>& lstFiles, list<String>& lstFilesNew, bool fUseAbsolutePaths = false);

		void RemoveFile(const FileName fn, const FileName fnODF=FileName(), bool fRemoveEntireObject = rmFULLOBJECT);
		unsigned __int64 iSizeNeeded() ;
		bool fUsesAbsolutePaths();
		static PathInfo piContainsPathInfo(const String& sValue);
		int  iFindFilePattern(const String& sOrg, const String& sPattern, int iFrom, int& iPatternLength);
		void SetPathsToDestination(const Directory& dirDestination, bool fUseAbsolutePaths); // used for copy to different dirs
		void SetPathsToDestination();		// used for copy in same dir
		bool AddEntry(ODFEntry& oeFile);
		void GatherFiles(ObjectStructure& osStruct);
		void RetrieveEntireCollection(bool fYesNo);
		void RemoveSuperflousCollectionReferences(const Directory& dirDestination);
		void SetDestinationFile(const FileName& fnOld, const FileName& fnNew);
		ObjectStructure::ODFEntry* oeGetODFEntry(int i);
};

#endif
