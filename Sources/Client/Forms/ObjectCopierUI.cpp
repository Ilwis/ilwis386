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
#include "Client\Headers\formelementspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\File\Directory.h"
#include "Client\ilwis.h"
#include "Engine\Base\StlExtensions.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Base\File\BaseCopier.h"
#include "Client\Forms\ObjectCopierUI.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\constant.h"
#include "Headers\Hs\Mainwind.hs"

class CollisionQuestionForm : public FormBaseDialog
{
	public:

		CollisionQuestionForm(CWnd *par, const String& sTitle, FileName& _fnObj1, FileName& _fnObj2, ObjectCopierUI::chChoice &iC, bool fYesAllPresent=true) :
			FormBaseDialog(par, sTitle, 
				fbsSHOWALWAYS|fbsMODAL|fbsNOCANCELBUTTON|fbsNOOKBUTTON | fbsBUTTONSUNDER|fbsNOBEVEL),
      iChoice(iC),
			fnObj1(_fnObj1),
			fnObj2(_fnObj2)
		{
			int iImage = IlwWinApp()->iImage(fnObj1.sExt);
			HICON icon;
			if (iImage)
				icon = IlwWinApp()->ilLarge.ExtractIcon(iImage);

			String sName = fnObj1.sRelative();
			String sMessage(TR("This folder already contains a file named %S").c_str() , sName);
			new StaticText ( root, sMessage);			
			new FieldBlank(root);
			sMessage = String(TR("Would you like to replace the existing file ?"));
			new StaticText ( root, sMessage);			
			
			FlatIconButton *fib = new FlatIconButton(root, icon, sName, (NotifyProc)&CollisionQuestionForm::PressButton1, fnObj1, false, FlatIconButton::fbsNORMAL | FlatIconButton::fbsNOCONTEXTMENU);
			GetAttributes(fnObj1, sMessage);
			new StaticText ( root, sMessage);
			new FieldBlank(root);
			new StaticText(root, TR("With this one ?"));

			sName = fnObj2.sRelative();	

			iImage = IlwWinApp()->iImage(fnObj2.sExt);
			if (iImage)
				icon = IlwWinApp()->ilLarge.ExtractIcon(iImage);			

			fib = new FlatIconButton(root, icon, sName, (NotifyProc)&CollisionQuestionForm::PressButton2, fnObj2, false, FlatIconButton::fbsNORMAL | FlatIconButton::fbsNOCONTEXTMENU);
			GetAttributes(fnObj2, sMessage);
			new StaticText ( root, sMessage);			
			
			FieldGroup *fg = new FieldGroup(root);

			new FieldBlank(fg);
			PushButton *pb2;
			PushButton *pb1 = new PushButton(root, TR("Yes"), (NotifyProc)&CollisionQuestionForm::Yes); 
			pb1->SetIndependentPos();
			if ( fYesAllPresent )
			{
				pb2 = new PushButton(root, TR("Yes to All"), (NotifyProc)&CollisionQuestionForm::YesToAll); 
				pb2->Align(pb1, AL_AFTER);
				pb2->SetIndependentPos();
			}
			else
				pb2 = pb1; // for alignment
			pb1 = new PushButton(root, TR("No"), (NotifyProc)&CollisionQuestionForm::No); 
			pb1->Align(pb2, AL_AFTER);
			pb1->SetIndependentPos();
			pb2 = new PushButton(root, TR("Cancel"), (NotifyProc)&CollisionQuestionForm::Cancel); 
			pb2->Align(pb1, AL_AFTER);
//			fg->Align(ic, AL_AFTER);
			create();
		}
    void GetAttributes(const FileName& fn, String& sMessage)
		{
			CFile file(fn.sFullPath().c_str(), CFile::shareDenyWrite);
			CFileStatus status;
			file.GetStatus(status);
			String sTime(status.m_mtime.Format("%c"));
			String sSize("%d", status.m_size);
			sMessage = String(TR("%S bytes, modified: %S").c_str(), sSize, sTime);
		}			

    int Yes(Event *)
		{
			iChoice = ObjectCopierUI::chYES;
			FormBaseDialog::OnOK();
			return 1;
		}
		int No(Event *)
		{
			iChoice = ObjectCopierUI::chNO;
			FormBaseDialog::OnOK();
			return 1;
		}
		int YesToAll(Event  *)
		{
			iChoice = ObjectCopierUI::chYESTOALL;
			FormBaseDialog::OnOK();
			return 1;
		}
		int Cancel(Event *)
		{
			iChoice = ObjectCopierUI::chCANCEL;
			FormBaseDialog::OnOK();
			return 1;
		}
		int PressButton1(Event *)
		{
			String sCmd("prop %S", fnObj1.sFullPath());
			IlwWinApp()->Execute( sCmd);			
			return 1;
		}
		int PressButton2(Event *)
		{
			String sCmd("prop %S", fnObj2.sFullPath());
			IlwWinApp()->Execute( sCmd);
			return 1;
		}					
		int exec()
		{	return 1;}
	private:
		ObjectCopierUI::chChoice& iChoice;
		FileName fnObj1;
		FileName fnObj2;
};

class MultipleObjectQuestionForm : public FormBaseDialog
{
	public:

		MultipleObjectQuestionForm(CWnd *par, const String& sTitle, const String& sQuestion,  ObjectCopierUI::chChoice &iC, bool fYesAllPresent=true) :
			FormBaseDialog(par, sTitle, 
				fbsSHOWALWAYS|fbsMODAL|fbsNOCANCELBUTTON|fbsNOOKBUTTON | fbsBUTTONSUNDER|fbsNOBEVEL),
      iChoice(iC)
		{
			HICON hic = IlwWinApp()->LoadStandardIcon(IDI_QUESTION);
			StaticIcon *ic = new StaticIcon(root, hic, true);
			FieldGroup *fg = new FieldGroup(root);
			Array<String> arTxt;
			Split(sQuestion, arTxt, "\n");
			for(int i=0; i < arTxt.size(); ++i )
				new StaticText(fg, arTxt[i]); 

			new FieldBlank(fg);
			PushButton *pb2;
			PushButton *pb1 = new PushButton(root, TR("Yes"), (NotifyProc)&MultipleObjectQuestionForm::Yes); 
			pb1->SetIndependentPos();
			if ( fYesAllPresent )
			{
				pb2 = new PushButton(root, TR("Yes to All"), (NotifyProc)&MultipleObjectQuestionForm::YesToAll); 
				pb2->Align(pb1, AL_AFTER);
				pb2->SetIndependentPos();
			}
			else
				pb2 = pb1; // for alignment
			pb1 = new PushButton(root, TR("No"), (NotifyProc)&MultipleObjectQuestionForm::No); 
			pb1->Align(pb2, AL_AFTER);
			pb1->SetIndependentPos();
			pb2 = new PushButton(root, TR("Cancel"), (NotifyProc)&MultipleObjectQuestionForm::Cancel); 
			pb2->Align(pb1, AL_AFTER);
			fg->Align(ic, AL_AFTER);
			create();
		}
    int Yes(Event *)
		{
			iChoice = ObjectCopierUI::chYES;
			FormBaseDialog::OnOK();
			return 1;
		}
		int No(Event *)
		{
			iChoice = ObjectCopierUI::chNO;
			FormBaseDialog::OnOK();
			return 1;
		}
		int YesToAll(Event  *)
		{
			iChoice = ObjectCopierUI::chYESTOALL;
			FormBaseDialog::OnOK();
			return 1;
		}
		int Cancel(Event *)
		{
			iChoice = ObjectCopierUI::chCANCEL;
			FormBaseDialog::OnOK();
			return 1;
		}
		int exec()
		{	return 1;}
	private:
		ObjectCopierUI::chChoice& iChoice;
};

ObjectCopierUI::ObjectCopierUI(CWnd *parent) :
	BaseCopier(),
	wndParent(parent)
{}

ObjectCopierUI::~ObjectCopierUI()
{
}

ObjectCopierUI::ObjectCopierUI(CWnd *parent, const vector<FileName>& files, const Directory& _dirDestination) :
	BaseCopier(files, _dirDestination),
	wndParent(parent)
{
}

ObjectCopierUI::ObjectCopierUI(CWnd *parent, const FileName& fnFile, const Directory& _dirDestination ) :
    BaseCopier(fnFile, _dirDestination),
	wndParent(parent)
{
}	

ObjectCopierUI::ObjectCopierUI(CWnd *parent, const FileName& fnFile, const FileName& _fnDestination ) :
	BaseCopier(fnFile, _fnDestination),
	wndParent(parent)
{
}

void ObjectCopierUI::CopyFiles(const FileName& fnInitial, const FileName& _fnDestination, bool fBreakDep)
{
	ObjectCopierUI cop(0, fnInitial, _fnDestination);
	cop.Copy(fBreakDep);
}

// In several places in the GatherFiles() function the following code can be found:
// 		IlwWinApp()->SetCurDir( (*file).sPath());
//		// Some statements
//		IlwWinApp()->SetCurDir( sCurDir);
// Here for the statements in between the current directory is temporarily set
// to the directory of the object under consideration (== *file). This is needed
// to avoid having wrong file paths in files contained in this object.
bool ObjectCopierUI::GatherFiles(ObjectStructure& osStruct, bool fBreakDep, bool fQuiet)
{
	chChoice chBreakDependencies =  fBreakDep || fQuiet ? chYES : chANY;
	chChoice chCopyEntireCollection =  fQuiet ? chYESTOALL : chANY;
	chChoice choiceCopyEntireContainer =  fQuiet ? chYESTOALL :	chANY;
	chChoice chCopyLinkedTables = chYES;  // force unconditional copy of linked tables

	String sCurDir = IlwWinApp()->sGetCurDir();
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
		osStruct.RetrieveEntireCollection( choiceCopyEntireContainer == chYESTOALL || choiceCopyEntireContainer == chANY ? true : false);
		
		if ( trq.fText(String(TR("Collecting files of: %S").c_str(), (*file).sRelative())))
			return false;

		
		IlwWinApp()->SetCurDir( (*file).sPath());
		IlwisObject obj = IlwisObject::obj( *file);
		IlwWinApp()->SetCurDir( sCurDir);
		
		if ( obj.fValid() )
		{
			if ( !fCIStrEqual(dirDestination.sFullPath(), (*file).sPath()))
			{
				String sType, sMessage;
				ObjectInfo::ReadElement("Ilwis", "Type", obj->fnObj, sType);
				if (fCIStrEqual(sType, "DataBaseCollection"))  // Cannot copy database collections
					sMessage = TR("Cannot copy database collections to another directory");
				if (IlwisObject::iotObjectType(obj->fnObj) == IlwisObject::iotTABLE)
					if (obj->fUseAs())   // cannot copy UseAs tables
						sMessage = TR("Cannot copy use-as tables to another directory");
				if (!sMessage.empty())
				{
						IlwWinApp()->GetMainWnd()->MessageBox(sMessage.c_str(), TR("Copy files").c_str(), MB_OK | MB_ICONEXCLAMATION );
						continue;
				}
			}

			bool fDependent = obj->fDependent() || obj->fUsesDependentObjects();
			if ( fDependent )
			{
				bool fBreak = osStruct.fBreakDependencies();
				if ( chBreakDependencies == chANY)
				{
					String sMessage(TR("Break Dependencies of %S").c_str(), obj->fnObj.sRelative(false));
					int iRet = IlwWinApp()->GetMainWnd()->MessageBox(sMessage.c_str(), TR("Break Dependencies").c_str(), MB_YESNOCANCEL | MB_ICONQUESTION );
					if ( iRet == IDCANCEL)
						return false;
					fBreak = iRet == IDNO ? false : true;
					chBreakDependencies = fBreak ? chYES : chNO;
				}
				osStruct.SetBreakDependencies( fBreak );
			}
			// if the object is a container ask if its contents should also move
			if ( ObjectStructure::fIsContainer( *file) )
			{

				if (choiceCopyEntireContainer != chYESTOALL || choiceCopyEntireContainer == chANY)
				{
					String sObjType;
					String sMessage(TR("Copy entire %S '%S', (object plus contents)?").c_str(), obj->sType(),  obj->fnObj.sFile);
					String sTitle(TR("Copy Entire %S").c_str(), obj->sType()); 
					MultipleObjectQuestionForm frm(wndParent, sTitle, sMessage, choiceCopyEntireContainer, iCountContainers > 1);
				}
				if ( choiceCopyEntireContainer == chNO )
					osStruct.RetrieveEntireCollection( false);
				if ( choiceCopyEntireContainer == chCANCEL)
					return false;
				
			}
			// system objects may be copied, but only if they are in the original list of files to be copied
			// so temporarly enable the adding of system objects. They may never be added in the course of recursion
			// through all the objects used by the object. 
			if ( ObjectInfo::fSystemObject(*file))
			{
				osStruct.AcceptSystemObject(true);
				IlwWinApp()->SetCurDir( (*file).sPath());
				obj->GetObjectStructure(osStruct);
				IlwWinApp()->SetCurDir( sCurDir);
				osStruct.AcceptSystemObject(false);
			}	
			else
			{
				IlwWinApp()->SetCurDir( (*file).sPath());
				obj->GetObjectStructure(osStruct);
				IlwWinApp()->SetCurDir( sCurDir);
			}
		}
		else
		{
			if ( IlwisObject::iotObjectType(*file) == IlwisObject::iotANY)
			{
				IlwWinApp()->SetCurDir( (*file).sPath());
				Array<FileName> arFiles;
				ForeignFormat::GetDataFiles(*file, arFiles);
				for(int i=0; i < arFiles.size(); ++i)
					if ( arFiles[i].fExist()  )
						osStruct.AddFile(arFiles[i]);
				IlwWinApp()->SetCurDir( sCurDir);
			}
		}
	}

	if (chCopyLinkedTables == chANY && osStruct.fFindLinkedTables(fnFiles))
	{
		String sMessage(TR("Copy attribute tables linked to domains?"));
		int iRet =  IlwWinApp()->GetMainWnd()->MessageBox(sMessage.c_str(), TR("Copy files").c_str(), MB_YESNOCANCEL | MB_ICONQUESTION );
		if ( iRet == IDCANCEL )
			return false;
		
		chCopyLinkedTables = iRet == IDYES ? chYES : chNO;

		if (chCopyLinkedTables == chNO)
		{
			osStruct.CopyLinkedTable(false);
			osStruct.RemoveLinkedTables();
		}
	}

	if ( osStruct.fBreakDependencies() )
	{
		bool *fNoUpdate = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
		if ( *fNoUpdate )
			return true;
		IlwWinApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, 0, 0);
	}
	return true;
}

void ObjectCopierUI::Copy(bool fBreakDependencies, bool fQuiet)
{
	try
	{
		CWaitCursor cw;
		if ( fnFiles.size() == 0) // huh, no files
			return;

		if (!wndParent)
			wndParent = IlwWinApp()->GetMainWnd();

		if (!dirDestination.fExist())
			throw ErrorObject(String(TR("Directory, %S does not exist").c_str(), dirDestination.sFullPath()));

		bool fCopyInSameDir = false;
		if ( fCIStrEqual(dirDestination.sFullPath(), fnFiles[0].sPath()))
			fCopyInSameDir = true;

		ObjectStructure osStruct;
		osStruct.SetCommandAction(ObjectStructure::caCOPY);
		osStruct.GetAssociatedFiles(!fCopyInSameDir);

		bool *fDoNotLoadGDB = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));
		*fDoNotLoadGDB = true;			
		
		if (!GatherFiles(osStruct, fBreakDependencies, fQuiet))
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
			chChoice choice = chANY;
			MultipleObjectQuestionForm frm(IlwWinApp()->GetMainWnd(), TR("Copy files"), TR("Some objects used by the objects to be copied are not located in the initial directory.\nShould these files also be copied?"), choice, false);
			if ( choice == chNO) 
				fUseAbsolutePaths = true;
			if ( choice == chCANCEL )
			{
				bool *fDoNotLoadGDB = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));
				*fDoNotLoadGDB = false;
				return;
			}
		}

		if (!fCopyInSameDir)
			if (!ResolveCollisions(osStruct, fQuiet))
				return;
		
		char *sNameBuf=0, *sDestBuf=0;
		list<String> lstFilesUsed, lstOutPutFileNames;

		// set the outputnames. The function will watch if system objects are copied (and force a rename)
		if ( SetNewNames(osStruct, fUseAbsolutePaths, fQuiet))
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

				bool *fNoUpdate = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
				if ( *fNoUpdate == false)
				{
					FileName *fn = new FileName(dirDestination.sFullPathQuoted());
					
					IlwWinApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, 0, (long)fn);
				}
			}
		}
		fDoNotLoadGDB = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));
		*fDoNotLoadGDB = false;
	}
	catch (ErrorObject& err)
	{
		err.Show();
		bool *fDoNotLoadGDB = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));
		*fDoNotLoadGDB = false;
	}
	catch (CException* err)
	{
		err->ReportError();
		err->Delete();
		bool *fDoNotLoadGDB = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTLOADGDB));
		*fDoNotLoadGDB = false;
	}
}

bool ObjectCopierUI::ResolveCollisions(ObjectStructure& osStruct, bool fQuiet)
{
	list<FileName> lstFilesPresent, lstUsed;	
	GetDestinationDirFiles(lstFilesPresent);
	osStruct.GetUsedODFs(lstUsed);

	chChoice choice = fQuiet ? chYESTOALL : chANY;
	chChoice choiceRO = fQuiet ? chYESTOALL : chANY;

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
			if ( choice != chYESTOALL )
			{
				CollisionQuestionForm frm(wndParent, TR("Overwrite existing files"), *cur, fnExist, choice, true);
			}
			if ( choice == chCANCEL )
				return false;
			if ( choice == chNO )
			{
				// remove odf entry from the osstruct
				osStruct.RemoveFile(*cur, FileName(), ObjectStructure::rmFILEONLY);
			}
			// replaced files in the destination dir could be read-only, so ask
			else if ( osStruct.fCopyToReadOnly( *cur ))
			{
				if ( choiceRO != chYESTOALL )
				{
					String sMessage(TR("The file %S that will be overwritten is read-only.\n Should it be overwritten?").c_str(), (*cur).sFile + (*cur).sExt);
					MultipleObjectQuestionForm frm(wndParent, TR("Overwrite existing files"), sMessage, choiceRO, true);
				}
				if ( choiceRO == chNO )
					osStruct.RemoveFile(*cur, FileName(), ObjectStructure::rmFILEONLY);
				if ( choiceRO == chCANCEL )
					return false;
			}
		}
	}
	return true;
}

bool fAskForRename(ObjectStructure::ODFEntry *entry, const Directory& dirDestination)
{
	// first check for copy of system objects
	if ( ObjectInfo::fSystemObject( entry->fnFile)) // system files
	{
		// system objects may be copied but not to the same name
		String sFile = entry->fnFile.sFile + entry->fnFile.sExt;
		String sFile2 = entry->fnDestination.sFile + entry->fnDestination.sExt;
		if ( fCIStrEqual( sFile , sFile2 ))
			return true;
	}
	// Now check if the destination path differs, in which case no new name is needed
	if ( dirDestination.sFullPath() != entry->fnFile.sPath())
		return false;

	if ( entry->fnDestination.fExist() &&
			IlwisObject::iotObjectType(entry->fnFile) != IlwisObject::iotANY) // only ilwis objects
		return true;
	else if ( entry->fnDestination.fExist() &&
			IlwisObject::iotObjectType(entry->fnFile) == IlwisObject::iotANY &&
			 entry->fnODF == FileName() ) // stand alone foreign files
		return true;

	return false;
}

class NewNameQuestionForm : public FormWithDest
{
	public:
		NewNameQuestionForm(CWnd *par, const String& sTitle, const FileName& fn, const FileName& fnExist, FileName* _fnObjOld)
			: FormWithDest(par, sTitle, fbsSHOWALWAYS|fbsMODAL| fbsBUTTONSUNDER|fbsNOBEVEL),
			fnObjOld(_fnObjOld)
	{
		IlwisObject obj = IlwisObject::obj(fn);
		String sType = "file";
		if ( obj.fValid() )
			sType = obj->sType();
			
		sNewName = fnObjOld->sFile;
		String sQuestion = String(TR("The %S %S already exists.").c_str(), sType, fnExist.sFileExt());
		new StaticText(root, sQuestion);
		FieldString *fsN = new FieldString(root, TR("New name") , &sNewName);
		fsN->SetIndependentPos();
		fsN->SetWidth(100);

		create();
	}

	int exec()
	{
		FormWithDest::exec();
		FileName fnNew(sNewName + fnObjOld->sExt, *fnObjOld); // make sure the correct path is used
		*fnObjOld = fnNew;
		return 1;
	}

	private:
		FileName *fnObjOld;
		String sNewName;
};

bool ObjectCopierUI::SetNewNames(ObjectStructure& osStruct, bool fUseAbsolutePaths, bool fQuiet)
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
			if (fQuiet)
				continue;
			if ( IlwisObject::iotObjectType( entry->fnODF) != IlwisObject::iotANY ||
				entry->fnODF == FileName() ) 
			{
				bool fAsk = fAskForRename(entry, dirDestination);
				while ( fAsk)
				{
					FileName fnObjOld = entry->fnDestination; 
					entry->fnDestination.sFile = String("Copy of %S", entry->fnDestination.sFile);
					FileName fnObjNew = entry->fnDestination;
					NewNameQuestionForm frm(wndParent, TR("Copy files"), entry->fnFile, fnObjOld, &fnObjNew);
					if (frm.fOkClicked())
					{
						if (!fnObjNew.fExist())
						{
							osStruct.SetDestinationFile(entry->fnFile, fnObjNew);
							fAsk = false;
						}							
						else
						{
							fAsk = true;
							wndParent->MessageBox( String(TR("%S already exists in destination directory").c_str(), fnObjNew.sRelative()).c_str(), TR("Error").c_str(), MB_OK | MB_ICONERROR);
						}							
					}
					else
						return false; // cancel pressed, abort operation
				}
			}				
		}			
	}		
	return true;
}



