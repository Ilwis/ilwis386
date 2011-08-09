#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Base\StlExtensions.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Table\tbl.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\constant.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\File\BaseCopier.h"
#include "Engine\Base\File\ObjectCopier.h"
#include "Headers\Hs\Mainwind.hs"

ObjectCopier::ObjectCopier() {
}

ObjectCopier::ObjectCopier(const vector<FileName>& files, const Directory& _dirDestination) :
	BaseCopier(files, _dirDestination)
{
}

ObjectCopier::ObjectCopier(const FileName& fnFile, const Directory& _dirDestination ) :
	BaseCopier(fnFile, _dirDestination)
{
}

ObjectCopier::ObjectCopier(const FileName& fnFile, const FileName& _fnDestination ) :
	BaseCopier(fnFile, _fnDestination)
{
}

void ObjectCopier::CopyFiles(const FileName& fnInitial, const FileName& _fnDestination, bool fBreakDep)
{
	ObjectCopier cop(fnInitial, _fnDestination);
	cop.Copy(fBreakDep);
}

bool ObjectCopier::GatherFiles(ObjectStructure& osStruct, bool fBreakDep)
{
	String sCurDir = getEngine()->sGetCurDir();
	Tranquilizer trq;

	trq.SetTitle("Copy");

	ExpandWildcards(fnFiles);

	FileName fn = *(fnFiles.begin());
	osStruct.SetBaseDir( Directory(fn.sPath()));

	int iCountContainers = 0;

	for(vector<FileName>::iterator cur = fnFiles.begin(); cur != fnFiles.end(); ++cur)
	{
		if ( ObjectStructure::fIsContainer(*cur) )
			++iCountContainers;
		if ( iCountContainers > 1)
			break;
	}
	
	for(vector<FileName>::iterator file = fnFiles.begin(); file != fnFiles.end(); ++file)
	{
		osStruct.RetrieveEntireCollection( true);
		
		getEngine()->SetCurDir( (*file).sPath());
		IlwisObject obj = IlwisObject::obj( *file);
		getEngine()->SetCurDir( sCurDir);
		
		if ( obj.fValid() )
		{
			bool fDependent = obj->fDependent() || obj->fUsesDependentObjects();
			if ( fDependent )
			{
				bool fBreak = true;
				osStruct.SetBreakDependencies( fBreak );
			}
			// system objects may be copied, but only if they are in the original list of files to be copied
			// so temporarly enable the adding of system objects. They may never be added in the course of recursion
			// through all the objects used by the object. 
			if ( ObjectInfo::fSystemObject(*file))
			{
				osStruct.AcceptSystemObject(true);
				getEngine()->SetCurDir( (*file).sPath());
				obj->GetObjectStructure(osStruct);
				getEngine()->SetCurDir( sCurDir);
				osStruct.AcceptSystemObject(false);
			}	
			else
			{
				getEngine()->SetCurDir( (*file).sPath());
				obj->GetObjectStructure(osStruct);
				getEngine()->SetCurDir( sCurDir);
			}
		}
		else
		{
			if ( IlwisObject::iotObjectType(*file) == IlwisObject::iotANY)
			{
				getEngine()->SetCurDir( (*file).sPath());
				Array<FileName> arFiles;
				ForeignFormat::GetDataFiles(*file, arFiles);
				for(unsigned int i=0; i < arFiles.size(); ++i)
					if ( arFiles[i].fExist()  )
						osStruct.AddFile(arFiles[i]);
				getEngine()->SetCurDir( sCurDir);
			}
		}
	}


	if ( osStruct.fBreakDependencies() )
	{
		bool *fNoUpdate = (bool *)(getEngine()->getContext()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
		if ( *fNoUpdate )
			return true;
		CWnd *wnd = AfxGetMainWnd();
		if ( wnd)
			wnd->PostMessage(ILW_READCATALOG, 0, 0);
	}
	return true;
}


void ObjectCopier::Copy(bool fBreakDependencies)
{
	try
	{
		if ( fnFiles.size() == 0) // huh, no files
			return;

		if (!dirDestination.fExist())
			throw ErrorObject(String(TR("Directory, %S does not exist").c_str(), dirDestination.sFullPath()));

		bool fCopyInSameDir = false;
		if ( fCIStrEqual(dirDestination.sFullPath(), fnFiles[0].sPath()))
			fCopyInSameDir = true;

		ObjectStructure osStruct;
		osStruct.SetCommandAction(ObjectStructure::caCOPY);
		osStruct.GetAssociatedFiles(!fCopyInSameDir);

		bool *fDoNotLoadGDB = (bool *)(getEngine()->getContext()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));
		*fDoNotLoadGDB = true;			
		
		if (!GatherFiles(osStruct, fBreakDependencies))
			return;
		
		unsigned __int64 iSpaceAvailable;
		unsigned __int64 iSpaceRequired = osStruct.iSizeNeeded();

		if (!fSufficientDiskSpace(iSpaceRequired, iSpaceAvailable, dirDestination.sDriveName()))
		{
			char sTemp1[100];
			char sTemp2[100];
			sprintf(sTemp1, "%I64u", iSpaceAvailable);
			sprintf(sTemp2, "%I64u", iSpaceRequired);
			String sMessage(TR("Not Enough Disk space.\nRequired is %s bytes, Available are %s bytes").c_str(), sTemp2, sTemp1);
			throw ErrorObject(sMessage);
		}

		bool fUseAbsolutePaths = false;
		// path information may be in the entry, should the relevant files remain or be copied.
		if (osStruct.fFilesContainRelevantPathInfo(dirDestination))
		{
			fUseAbsolutePaths = true;
		}

		if (!fCopyInSameDir)
			if (!ResolveCollisions(osStruct))
				return;
		
		char *sNameBuf=0, *sDestBuf=0;
		list<String> lstFilesUsed, lstOutPutFileNames;

		// set the outputnames. The function will watch if system objects are copied (and force a rename)
		if ( SetNewNames(osStruct, fUseAbsolutePaths))
		{

			// retrieve both the input filenames and the output filenames
			osStruct.GetUsedFiles( lstFilesUsed, lstOutPutFileNames, fUseAbsolutePaths);

			if (lstFilesUsed.size() != 0) 
			{

				// build the string to use with the shFileOpartion
				BuildCopyStrings(lstFilesUsed, lstOutPutFileNames, &sNameBuf, &sDestBuf, fUseAbsolutePaths);

				//shFileOperation copies the files
				CopyFileList(lstOutPutFileNames, sNameBuf, sDestBuf);

				// relink paths to files not copied
				if ( fCopyInSameDir)
					osStruct.SetPathsToDestination();
				else
					osStruct.SetPathsToDestination( dirDestination, fUseAbsolutePaths );
				
				// unwanted references to collections must be removed
				osStruct.RemoveSuperflousCollectionReferences(dirDestination);
				osStruct.BreakDependencies(); // will happen only if needed

				delete [] sNameBuf; // allocated in BuildCopyStrings
				delete [] sDestBuf;	// allocated in BuildCopyStrings

				bool *fNoUpdate = (bool *)(getEngine()->getContext()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
				if ( *fNoUpdate == false)
				{
					FileName *fn = new FileName(dirDestination.sFullPathQuoted());
					CWnd *wnd = AfxGetMainWnd();
					if ( wnd)
						wnd->PostMessage(ILW_READCATALOG, 0, (WPARAM)fn);
				}
			}
		}
		fDoNotLoadGDB = (bool *)(getEngine()->getContext()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));
		*fDoNotLoadGDB = false;
	}
	catch (ErrorObject& err)
	{
		err.Show();
		bool *fDoNotLoadGDB = (bool *)(getEngine()->getContext()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));
		*fDoNotLoadGDB = false;
	}
	catch (CException* err)
	{
		err->ReportError();
		err->Delete();
		bool *fDoNotLoadGDB = (bool *)(getEngine()->getContext()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));
		*fDoNotLoadGDB = false;
	}
}

bool ObjectCopier::ResolveCollisions(ObjectStructure& osStruct)
{
	list<FileName> lstFilesPresent, lstUsed;	
	GetDestinationDirFiles(lstFilesPresent);
	osStruct.GetUsedODFs(lstUsed);

	// check if odf used are already present in the destination directory
	for( list<FileName>::iterator cur = lstFilesPresent.begin(); cur != lstFilesPresent.end(); ++cur)
	{
		// files in the destination dir could clash with files to be copied
		FileName fnExist = osStruct.fnODFFileNamePresent(*cur);
		if ( fnExist != FileName() )
		{
			CFileStatus fstat1, fstat2;
			CFile::GetStatus((*cur).sFullPath().c_str(), fstat1);
			CFile::GetStatus(fnExist.sFullPath().c_str(), fstat2);
			// are we talking about the same file ??, if so do not copy
			if (fstat1.m_mtime == fstat2.m_mtime && fstat1.m_size == fstat2.m_size  )
			{
				osStruct.RemoveFile(*cur, FileName(), ObjectStructure::rmFILEONLY);
				continue;
			}
		}
	}
	return true;
}

bool ObjectCopier::SetNewNames(ObjectStructure& osStruct, bool fUseAbsolutePaths)
{
	int i = 0;
	
	ObjectStructure::ODFEntry* entry = 0;
	bool fDestinationFileSet = false; 	
	while ( (entry = osStruct.oeGetODFEntry(i++)) != 0 )
	{
		if ( entry )
		{
			if (fnDestination != FileName() && !fDestinationFileSet)
			{
				osStruct.SetDestinationFile(entry->fnFile, fnDestination);
				fDestinationFileSet = true;
			}				
			else if ( entry->fnDestination == FileName())
			{
				entry->fnDestination = entry->fnFile;
				if ( !( fUseAbsolutePaths && 
					     ObjectStructure::piContainsPathInfo(entry->sFile) == ObjectStructure::piABSOLUTE))
							 entry->fnDestination.Dir(dirDestination.sFullPath());
				if (entry->fnODF == FileName()) 
					entry->fnDestination.Dir(dirDestination.sFullPath());					
				
			}
			continue;
		}			
	}		
	return true;
}
