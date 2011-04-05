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

#include "Client\Headers\AppFormsPCH.h"
#include "Headers\constant.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\Parm.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\System\mutex.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\Forms\RemoveFilesCollector.h"


LRESULT Cmddeleteobjects(CWnd *wnd, const String& s)
{
	ParmList pl(s);
	ObjectStructure *osStruct = new ObjectStructure();
	osStruct->GetAssociatedFiles(pl.fGet("noassociated") ? false : true);
	FilesForRemovalCollector frc(osStruct, pl);
	frc.CollectFiles();

	if ( frc.fRemoveOK())
		return (LRESULT)osStruct;

	return -1;
}

class MultipleObjectQuestionForm : public FormBaseDialog
{
public:
	MultipleObjectQuestionForm(CWnd *par, const String& sTitle, const String& sQuestion, 
                             Choice &ch, bool fYesAllPresent=true, LPCTSTR lpIcon=IDI_QUESTION) 
    : FormBaseDialog(par, sTitle, 
		fbsSHOWALWAYS|fbsMODAL|fbsNOCANCELBUTTON|fbsNOOKBUTTON|fbsBUTTONSUNDER|fbsNOBEVEL|fbsTOPMOST)
		, choice(ch)
	{
		HICON hic = IlwWinApp()->LoadStandardIcon(lpIcon);
		StaticIcon *ic = new StaticIcon(root, hic, true);
		FieldGroup *fg = new FieldGroup(root);
		Array<String> arTxt;
		Split(sQuestion, arTxt, "\n");
		int iNameWidth = 0;
		for(unsigned int i=0; i < arTxt.size(); ++i )
		{
			StaticText *st = new StaticText(fg, arTxt[i]);
			zDimension dim = st->Dim(arTxt[i]);
			dim.width() = min(dim.width(), 500);
			st->SetFieldWidth(dim.width());
		}

		new FieldBlank(fg);
		PushButton *pb2;
		PushButton *pb1 = new PushButton(root, SMSYes, (NotifyProc)&MultipleObjectQuestionForm::Yes); 
		if (choice == chYES || choice == chANY)
			pb1->SetDefault(true);
		pb1->SetIndependentPos();
		if ( fYesAllPresent )
		{
			pb2 = new PushButton(root, SMSYesToAll, (NotifyProc)&MultipleObjectQuestionForm::YesToAll); 
			pb2->Align(pb1, AL_AFTER);
			pb2->SetIndependentPos();
			if (choice == chYESTOALL)		
				pb2->SetDefault(true);
		}
		else
			pb2 = pb1; // for alignment
		pb1 = new PushButton(root, SMSNo, (NotifyProc)&MultipleObjectQuestionForm::No);
		if (choice == chNO )
			pb1->SetDefault(true);
		pb1->Align(pb2, AL_AFTER);
		pb1->SetIndependentPos();
		pb2 = new PushButton(root, SMSCancel, (NotifyProc)&MultipleObjectQuestionForm::Cancel); 
		if (choice == chCANCEL)
			pb2->SetDefault(true);
		pb2->Align(pb1, AL_AFTER);
		fg->Align(ic, AL_AFTER);
		create();
	}
private:
  int Yes(Event *)
	{
		choice = chYES;
		FormBaseDialog::OnOK();
		return 1;
	}
	int No(Event *)
	{
		choice = chNO;
		FormBaseDialog::OnOK();
		return 1;
	}
	int YesToAll(Event  *)
	{
		choice = chYESTOALL;
		FormBaseDialog::OnOK();
		return 1;
	}
	int Cancel(Event *)
	{
		choice = chCANCEL;
		FormBaseDialog::OnOK();
		return 1;
	}
	int exec()
	{	return 1; }
	Choice& choice;
};

// Implementation of class FilesForRemovalCollector
FilesForRemovalCollector::FilesForRemovalCollector(ObjectStructure *osStruct, ParmList& pl)
	: m_osStruct(osStruct), m_pl(pl)
{
	m_trq = new Tranquilizer();
	bool *fNoUpdate = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
	*fNoUpdate = true;
}

FilesForRemovalCollector::~FilesForRemovalCollector()
{
	bool *fNoUpdate = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
	*fNoUpdate = false;
	delete m_trq;
}

bool FilesForRemovalCollector::fRemoveOK()
{
	return m_choices.choice != chCANCEL;
}

void FilesForRemovalCollector::CollectFiles()
{
	m_fForce = m_pl.fGet("force");
	m_fQuiet = m_pl.fGet("quiet");
	
	m_choices.choice                  = m_fForce ? chYESTOALL : chANY;
	m_choices.choiceDelAnyWay         = m_fForce ? chYESTOALL : chANY;
	m_choices.choiceDelRO             = m_fForce ? chYESTOALL : chANY;
	m_choices.choiceContainerHandling = m_fQuiet ? chYESTOALL : chANY;

	Array<FileName> afn;
	// collect files from supplied parameters
	for (int iParm = 0; iParm < m_pl.iFixed(); ++iParm) 
	{
		String sFileMask = m_pl.sGet(iParm);
		FileName fnMask(sFileMask);
		// use as ilwis object if the mask could be an ilwis object or if the extension contains wildcard
		if (IlwisObject::iotObjectType(fnMask) != IlwisObject::iotANY || fnMask.sExt.find('*') != String::npos)
		{
			Array<String> asExt;
			IlwisObject::GetAllObjectExtensions(asExt);
			Array<FileName> afnTemp;  // needed because GetFileNames will clear the filename array first
			File::GetFileNames(sFileMask, afnTemp, &asExt);
			afn &= afnTemp;  // add the new series of filenames
		}
		else
		{
			Array<FileName> afnTemp;
			File::GetFileNames(sFileMask, afnTemp);
			for (unsigned int i = 0; i< afnTemp.size(); ++i)
			{
				vector<FileName> arrForeign;
				ForeignFormat::GetDataFiles(afnTemp[i], arrForeign);
				for(unsigned int j=0; j < arrForeign.size() ; ++j)
					afn &= arrForeign[j];
			}
		}
	}
	m_fYesToAllPresent =  afn.size() > 1 || m_pl.iFixed() > 1 ? true : false;
	if (!CollectObjectFiles(afn))
		m_choices.choice = chCANCEL;
}

bool FilesForRemovalCollector::CollectObjectFiles(Array<FileName>& afn)
{
	// loop through all collected objects and check if needed with the user if they should be deleted
	// if so at the end of the loop add these to m_osStruct
	// A check on a read/only directory is not necessary: Windows itself will allow removal
	// of files from such a directory without hesitation (no dialog)
	// Return false when the Cancel button (from any form) has been clicked
	Array<FileName> afnContents;

	for (unsigned int i = 0; i < afn.iSize(); ++i) 
	{
		FileName fn = afn[i];
		if (!fn.fValid() || !fn.fExist())
			continue;
		
		if ((m_trq != 0) && (afn.iSize() > 1)) 
		{
			String sText("del %S", fn.sFileExt());
			if (m_trq->fText(sText))
				return false;
		}
		try 
		{
			MutexFileName mut(fn);
			IlwisObject obj = IlwisObject::obj(fn);
			if (!obj.fValid())
			{
				m_osStruct->AddFile(fn);  // if the object is invalid, at least remove the ODF
				continue;
			}
			Choice choice = chCheckForRemoval(obj);
			if (choice == chCANCEL)
				return false;
			else if (choice == chYES || choice == chYESTOALL)
			{
				HandleDeletionOfContainers(obj, afnContents, m_choices.choiceContainerHandling);
				if (chCANCEL == m_choices.choiceContainerHandling)
					return false;
				if ( chNO == m_choices.choiceContainerHandling)
					m_osStruct->RetrieveEntireCollection(false);

				obj->GetObjectStructure(*m_osStruct);
			}
		}
		catch (const ErrorObject&) 
		{
			// The object is corrupt (missing something), remove it anyway.
			// As a result some data may be left behind, but the ODF will disappear
			m_osStruct->AddFile(fn);
		}
	}
	if (afnContents.size() > 0)
		return CollectObjectContentsFiles(afnContents);
	else
		return true;
}

bool FilesForRemovalCollector::CollectObjectContentsFiles(Array<FileName>& afnContents)
{
	// Collect files of objects that belong to the contents of a container
	// For these files it is not necessary to ask any questions.
	// Return false when the Cancel button (from any form) has been clicked

	// Force the deletion of container contents without asking anymore questions
	// Just collect the names of the files to delete
	m_fQuiet = true;
	m_fForce = true;
	for (unsigned int i = 0; i < afnContents.iSize(); ++i) 
	{
		FileName fn = afnContents[i];
		if (!fn.fValid() || !fn.fExist())
			continue;
		
		if ((m_trq != 0) && (afnContents.iSize() > 1)) 
		{
			String sText("del %S", fn.sFileExt());
			if (m_trq->fText(sText))
				return false;
		}
		try 
		{
			MutexFileName mut(fn);
			IlwisObject obj = IlwisObject::obj(fn);
			if (!obj.fValid())
			{
				m_osStruct->AddFile(fn);  // if the object is invalid, at least remove the ODF
				continue;
			}
			Choice choice = chCheckForRemoval(obj);
			if (choice == chYES || choice == chYESTOALL)
				obj->GetObjectStructure(*m_osStruct);
			else if (choice == chCANCEL)
				return false;
		}
		catch (const ErrorObject&) 
		{
			// The object is corrupt (missing something), remove it anyway.
			// As a result some data may be left behind, but the ODF will disappear
			m_osStruct->AddFile(fn);
		}
	}
	return true;
}

Choice FilesForRemovalCollector::chCheckForRemoval(const IlwisObject& obj)
{
	bool fStillOpen = obj->iRef > 1;
	if (m_fQuiet)
	{
		if (!obj->fReadOnly())  // R/O includes system objects as well
		{
			if ((!m_fForce && obj->fUsedInOtherObjects(0, false, m_trq)) || fStillOpen)
				return chNO;
		}
		return chYES;
	}
	else 
	{
		String s(SMSMsgDelete_s.c_str(), obj->sTypeName());
		if (obj->sDescription != "") 
			s &= String("\n(%S)", obj->sDescription);
		
		bool fUsedInOtherObjects = obj->fUsedInOtherObjects(0, false, m_trq);
		CWnd *wnd = IlwWinApp()->GetMainWnd();
		// error messages
		if (obj->fSystemObject())
		{
			wnd->MessageBox(SMSErrSystemObject.c_str(), SMSErrDelNotPossible.c_str(), MB_OK|MB_ICONEXCLAMATION);
			return chNO;
		}

		if (fUsedInOtherObjects && fStillOpen) 
		{
			String str(SMSErrDelNotPossibleStillInUse.c_str(), obj->sTypeName());
			wnd->MessageBox(str.c_str(), SMSErrDelNotPossible.c_str(), MB_OK|MB_ICONEXCLAMATION);
			return chNO;
		}
		if (fStillOpen) 
		{
			String s(SMSErrDelNotPossibleStillOpen.c_str(), obj->sTypeName());
			wnd->MessageBox(s.c_str(), SMSErrDelNotPossible.c_str(), MB_OK|MB_ICONEXCLAMATION);
			return chNO;
		}
		// if read only is not important enough, do not ask anything
		if (chYESTOALL == m_choices.choiceDelRO)
			return chYES;
		// warning: Read Only
		else if (obj->fReadOnly()) 
		{
			String sDelMsg(SMSMsgReadOnlyDeleteAnyway_S.scVal(), obj->fnObj.sRelative());
			MultipleObjectQuestionForm frm(IlwWinApp()->GetMainWnd(), SMSErrReadOnly, sDelMsg, m_choices.choiceDelRO, m_fYesToAllPresent, IDI_WARNING);
			return m_choices.choiceDelRO;
		}
		// if still in use is not important enough, also do not ask the normal case
		else if (chYESTOALL == m_choices.choiceDelAnyWay)
			return chYES;
		// remark: Still in use
		else if (fUsedInOtherObjects) 
		{
			String sDelMsg(SMSMsgDelNotPossibleStillInUseDelAnyway.c_str(), obj->sTypeName());
			MultipleObjectQuestionForm frm(IlwWinApp()->GetMainWnd(), SMSErrDelNotPossible, sDelMsg, m_choices.choiceDelAnyWay, m_fYesToAllPresent);
			return m_choices.choiceDelAnyWay;
		}
		// if choice is yestoall no question needed
		else if (m_choices.choice == chYESTOALL) 
			return chYES;
		// normal question
		else 
		{
			MultipleObjectQuestionForm frm(IlwWinApp()->GetMainWnd(), SMSMsgDelObject, s, m_choices.choice, m_fYesToAllPresent);
			return m_choices.choice;
		}
	}
	return chNO;
}

void FilesForRemovalCollector::HandleDeletionOfContainers(IlwisObject& obj, Array<FileName>& afn, Choice& choice)
{
	FileName fn = obj->fnObj;
	IlwisObject::iotIlwisObjectType iotType = IlwisObject::iotObjectType(fn);
	// only containers are processed
	if (iotType != IlwisObject::iotOBJECTCOLLECTION &&
		iotType != IlwisObject::iotMAPLIST &&
		iotType != IlwisObject::iotSTEREOPAIR)
		return;
	
	bool fAskQuestion = choice != chYESTOALL;
	if (iotType == IlwisObject::iotSTEREOPAIR)
	{
		choice = chYESTOALL;
		fAskQuestion = false;
	}
	if (iotType == IlwisObject::iotMAPLIST)
	{
		// maplist with only internal maps does not need processing
		String sMap;
		ObjectInfo::ReadElement("MapList", "Map1", fn, sMap);
		if ( sMap.find(":1") != -1 )	
			return;
		// dependent maplists always also delete their contents
		String sType;
		ObjectInfo::ReadElement("MapList", "Type", fn, sType);
		if ("MapListVirtual" == sType) 
		{
			choice = chYES;
			fAskQuestion = false;
		}
	}	
	if ( iotType == IlwisObject::iotOBJECTCOLLECTION && obj->sType() == "DataBaseCollection" )
	{
		fAskQuestion = false;
		choice = chYES;		
	}			
	// ask the question
	if (fAskQuestion)
	{
		Choice chDefault = chNO;
		String sDelMsg(SMSDeleteContentsAlso_S.scVal(), obj->sType(),  fn.sRelative(false));
		MultipleObjectQuestionForm frm(IlwWinApp()->GetMainWnd(), SMSTitleDeleteContainer, sDelMsg, chDefault, false);
		choice = chDefault;
		if (choice != chYESTOALL && choice != chYES)
			return;
	}				
	if (iotType == IlwisObject::iotOBJECTCOLLECTION)	
	{
		ObjectCollection oc(fn);
		if (!oc.fValid())
			return;
		// add the objects which are not yet in the array afn
		for(int j=0; j < oc->iNrObjects(); ++j)
		{
			FileName fnObj = oc->fnObject(j);
			vector<FileName>::iterator where = find(afn.begin(), afn.end(), fnObj);
			if (where == afn.end())
				afn &= fnObj;
		}									
	}	
	if (iotType == IlwisObject::iotMAPLIST)
	{
		MapList mpl(fn);
		if (!mpl.fValid())
			return;
		// add the maps which are not yet in the array afn
		for(int j = mpl->iLower(); j <= mpl->iUpper(); ++j)
		{
			FileName fnObj = mpl[j]->fnObj;
			if (fnObj.fExist())
			{
				vector<FileName>::iterator where = find(afn.begin(), afn.end(), fnObj);
				if (where == afn.end())
					afn &= fnObj;									
			}									
		}								
	}
	if (iotType == IlwisObject::iotSTEREOPAIR)
	{
		String sMap, sGeoRef;
		FileName fnMap;
		// Get map and georef of left side
		ObjectInfo::ReadElement("StereoPair", "Left", fn, sMap);
		fnMap = FileName(sMap, ".mpr");
		if (fnMap.fExist())
		{
			afn &= fnMap;
			ObjectInfo::ReadElement("Map", "GeoRef", fnMap, sGeoRef);
			afn &= FileName(sGeoRef, ".grf");
		}

		// Get map and georef of right side
		ObjectInfo::ReadElement("StereoPair", "Right", fn, sMap);
		fnMap = FileName(sMap, ".mpr");
		if (fnMap.fExist())
		{
			afn &= fnMap;
			ObjectInfo::ReadElement("Map", "GeoRef", fnMap, sGeoRef);
			afn &= FileName(sGeoRef, ".grf");
		}
	}
}
