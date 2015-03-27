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
/* $Log: /ILWIS 3.0/ApplicationForms/import.cpp $
 * 
 * 44    1-10-04 14:58 Retsios
 * Added gdal
 * 
 * 43    25-09-03 16:35 Retsios
 * Merge from GARtrip
 * 
 * 45    25-09-03 16:33 Retsios
 * Converted a plain string to an IlwisString
 * 
 * 44    30-07-03 14:38 Retsios
 * Added special code for gartrip import that has an extra "Height Offset"
 * parameter.
 * 
 * 42    17-01-03 13:10 Willem
 * - Removed: Some superfluous commented code
 * - Added: callback function to check the location of the output object
 * for read/only or file eixtance; The result of the check is displayed in
 * a new remark field at the bottom of the form
 * 
 * 41    7-01-03 15:07 Willem
 * - Added: Some simple protections against registry errors
 * 
 * 40    8/01/02 12:48p Martin
 * added htp numbers for ASTER
 * 
 * 39    7/10/02 3:36p Martin
 * not including extension when looking at the filelength
 * 
 * 38    5-07-02 19:20 Koolhoven
 * prevent warnings with Visual Studio .Net
 * 
 * 37    6/27/02 4:11p Martin
 * added change to max file length in the output file box
 * 
 * 36    6/24/02 3:48p Martin
 * no default will be generated if the filename is longer than 20 chars.
 * Type it yourself then
 * 
 * 35    6/03/02 2:37p Martin
 * Merge from ASTER branch
 * 
 * 38    5/24/02 12:49p Martin
 * added error handling for non ASTER files
 * 
 * 37    5/02/02 8:45a Martin
 * show button works correctly agains, relative path used for -ouput
 * option
 * 
 * 36    3/12/02 1:00p Martin
 * ASTER support added
 * 
 * 34    1/16/02 17:33 Willem
 * The SelectString function is now protected from getting a NULL pointer
 * (if sLastImport contains an empty string)
 * 
 * 33    11/06/01 2:28p Martin
 * GDB import now generates a full commandline
 * 
 * 32    11/06/01 1:01p Martin
 * syntax made by the import form for output was not correct
 * 
 * 31    9/12/01 16:03 Willem
 * Building the import command line now uses relative paths when possible
 * (sRelativeQuoted instead of sFullNameQuoted)
 * 
 * 30    8/28/01 4:21p Martin
 * added include file
 * 
 * 29    4/25/01 12:59 Willem
 * Added checks around registry communication
 * 
 * 28    17-04-01 11:19 Koolhoven
 * in CallBack change helptopic number
 * 
 * 27    23-03-01 2:50p Martin
 * different way of using the gdb expression
 * 
 * 26    27/02/01 15:09 Willem
 * Import in thread now properly initializes thread local variables before
 * starting and removes the variables when done
 * 
 * 25    6-02-01 9:49a Martin
 * generates correct statement when creating the syntax for a GDB file
 * with a different name
 * 
 * 24    6/02/01 9:02 Willem
 * Changed Execute() call to more thread-safe SendMessage()
 * 
 * 23    23/11/00 10:42 Willem
 * Removed superfluous right parenthesis in sBuildCommandLine()
 * 
 * 22    20/11/00 15:22 Willem
 * Added filtering to import file selection: Only non-Ilwis files are
 * displayed now
 * 
 * 21    17/11/00 16:24 Willem
 * The table wizard is now moved from the mainwindow to the import and is
 * now also started via a commandline
 * 
 * 20    17/11/00 15:43 Willem
 * Width of format selection box adjusted upward, depending on the font of
 * the form
 * 
 * 19    14/11/00 16:47 Willem
 * - The browse button has been made smaller
 * - General raster import needs to be handled separately, to be able to
 * show the General raster import form
 * 
 * 18    10/11/00 11:03 Willem
 * The checkboxes in the import form are now initialized:
 * GeoGateway:
 * - combine: default ON
 * - convert: default OFF
 * - show: default OFF
 * E00:
 * - Create pol: default OFF
 * 
 * 17    8/11/00 17:23 Willem
 * Extended the import form for extended option for geogateway and e00.
 * Also the import now generates a command line
 * 
 * 16    2/11/00 9:45 Willem
 * The latest used conversion is now only stored for "Import|Map". The
 * menu options "Import|General raster" or "Import|Geogateway"  do not
 * store the format in the registry anymore
 * 
 * 15    30/10/00 17:14 Willem
 * The mainwindow is now responsible for stopping/resuming the catalog
 * update messages
 * 
 * 14    27/10/00 16:52 Willem
 * The parent window is retrieved via function call instead of passing is
 * directly over thread boundary
 * 
 * 13    25/10/00 17:42 Willem
 * Import form now recognizes parameters to preselect either "general
 * raster" or "GeoGateway" formats
 * 
 * 12    22/09/00 17:06 Willem
 * The dBase import now is routed to the import table wizard
 * 
 * 11    12/09/00 12:18 Willem
 * The import now does not update the catalog windows until after the
 * import
 * 
 * 10    30/06/00 14:07 Willem
 * The import table wizard now also sets the main window command line
 * 
 * 9     21/06/00 18:13 Willem
 * Import now executes the result of the import table wizard
 * 
 * 8     16-06-00 11:01 Koolhoven
 * in CallBack() protect against iOption < 0
 * 
 * 7     24/05/00 9:11 Willem
 * Added the object output name as parameter to the wizard
 * 
 * 6     18/05/00 14:01 Willem
 * Added import Ascii tables wizard
 * 
 * 5     15/05/00 12:52 Willem
 * Import Form is now running in the thread of the MainWindow; it will
 * start the import itself in a new thread.
 * 
 * 4     4/05/00 16:57 Willem
 * - Improved ImportForm callback behaviour
 * - The last selected import is stored and set as default the next time
 * the Form is opened
 * 
 * 3     1-12-99 15:30 Wind
 * import.def was not searched in ilwis path
 * 
 * 2     18-06-99 9:43a Martin
 * ported the import forms
 * 
 * 1     17-06-99 3:18p Martin
// Revision 1.3  1998/10/07 12:09:09  Wim
// In FileCallBack() only the import filename will be set as default output when valid
// otherwise it will now be set to empty.
//
// Revision 1.2  1998-02-09 15:18:04+01  Wim
// Import form call SetNoSystemDir()
//
/* Import
// by Wim Koolhoven
// (c) ILWIS System Development, ITC
	Last change:  WK    7 Oct 98    1:08 pm
*/
#define IMPORT_C
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\selector.h"
#include "Client\FormElements\InfoText.h"
#include "Headers\messages.h"
#include "Headers\constant.h"
#include "engine\dataexchange\ForeignFormat.h"
#include "Client\Forms\IMPORT.H"
#include "Client\Forms\ImportAsciiTableWizard.h"
#include "Engine\Base\Tokbase.h"
#include "Engine\Base\DataObjects\Parm.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Base\File\Directory.h"
#include "Client\FormElements\flddat.h"
#include "Engine\DataExchange\CONV.H"
#include "Headers\Htp\Ilwismen.htp"
#include "Headers\Hs\Appforms.hs"
#include "Headers\Hs\IMPEXP.hs"
#include "Client\Forms\IMP14.H"
#include "Client\ilwis.h"
#include "Client\Editors\Utils\SBDestination.h"
#include "Engine\DataExchange\ForeignCollection.h"
#include "Client\FormElements\NonIlwisObjectLister.h"

class FieldImportFormat: public FieldOneSelect
{
public:
	FieldImportFormat(FormEntry* parent, long* value, const Array<ImportItem>& arr)
		: FieldOneSelect(parent, value), aii(arr)
	{
		CFont *fnt = IlwWinApp()->GetFont(IlwisWinApp::sfFORM);
		int iMax = 0;
		for (unsigned int i = 0; i < aii.iSize(); ++i)
		{
			String sFormat = aii[i].sDescr;
			zDimension dim = Dim(sFormat, fnt);
			if (dim.cx > iMax)
				iMax = dim.cx;
		}
		int iScrollX = GetSystemMetrics(SM_CYVSCROLL);
		SetFieldWidth(iMax + iScrollX);
		SetIndependentPos();
	}
	void create();
private:
	const Array<ImportItem>& aii;  
};

void FieldImportFormat::create()
{
	FieldOneSelect::create();
	for (unsigned int i = 0; i < aii.iSize(); ++i)
		ose->AddString(aii[i].sDescr.c_str());
	ose->SetCurSel(0);
}

class ImportForm: public FormWithDest
{
public:
	ImportForm(CWnd*, Importing*, String* sName, String* sCmd, String* sOutput, bool* fShow);

protected:
	String sBuildCommandLine();

private:
	int CallBack(Event*);
	int FileCallBack(Event*);
	int OutputFileCallBack(Event*);
	int BrowseClick(Event*);
	int exec();

	Importing             *imp;

	FieldDataTypeLarge    *fdtl;
	FieldImportFormat     *fif;
	FieldDataTypeCreate   *fdt;
	
	FieldGroup            *fgGDAL;
	FieldGroup            *fgASTER;	
	FieldGroup            *fgE00;
	FieldGroup            *fgGartrip;
	PushButton            *pbs;
	CheckBox              *cbShow;
	StaticText            *stRemark;

	String      sObjectName, sLastImport;
	String      *sName;
	String      *sCommand;
	long        iOption;
	bool        fFirstTime;
	bool        m_fShow;
	bool        m_fCreatePol;
	double      m_rGartripHeightOffset;
	bool        m_fCombine;
	bool        m_fConvert;
	bool				m_fUseRadiances;

	bool        *m_pfShow;
	bool        m_fMenuCmd; // special import started from menu (general raster anf GDAL)
};

ImportForm::ImportForm(CWnd* wPar, Importing* Import, String* sNam, String* sCmd, String* sOutput, bool* fShow)
	: FormWithDest(wPar, TR("Import")), 
		imp(Import), sName(sNam), sCommand(sCmd), m_pfShow(fShow), m_fCombine(true), m_fConvert(false), m_fCreatePol(false), m_fUseRadiances(true), m_rGartripHeightOffset(0.0)
{
	sObjectName = *sName;
	m_fShow = *m_pfShow;
	m_fMenuCmd = sCmd->length() > 0;  // true for the menu options and import|general raster import|gdal

	try
	{
		IlwisSettings settings("ImportExport\\Import");
		sLastImport = settings.sValue("Import", String());
	}
	catch (...)
	{
		// only catch registry errors; only set last import
		sLastImport = String();
	}
	fFirstTime = true;
	
	fdtl = new FieldDataTypeLarge(root, &sObjectName, ".*", new NonIlwisObjectLister(), false);
	fdtl->SetNoSystemDir();
	fdtl->SetCallBack((NotifyProc)&ImportForm::FileCallBack);
	fdtl->   SetWidth(250);
	StaticText* st = new StaticText(root, TR("Import &Format"));
	iOption = 0;
	fif = new FieldImportFormat(root, &iOption, imp->imp);
	fif->Align(st, AL_UNDER);
	fif->SetCallBack((NotifyProc)&ImportForm::CallBack);
	FieldBlank* fb = new FieldBlank(root);
	fb->Align(fif, AL_UNDER);

	FieldGroup *fgTypes = new FieldGroup(root, true);
	fdt = new FieldDataTypeCreate(fgTypes, TR("&Output Filename"), sOutput, "", true);
	fdt->SetCallBack((NotifyProc)&ImportForm::OutputFileCallBack);
	fdt->SetIndependentPos();
	pbs = new PushButton(fgTypes, "...", (NotifyProc)&ImportForm::BrowseClick);
	pbs->psn->iPosX = fdt->psn->iPosX + fdt->psn->iMinWidth;
	pbs->psn->iMinWidth = 18;
	pbs->Align(fdt, AL_AFTER);
	pbs->SetIndependentPos();

	
	cbShow = new CheckBox(fgTypes, TR("&Show"), &m_fShow);
	cbShow->Align(pbs, AL_AFTER);

	fgGDAL = new FieldGroup(fgTypes);
	fgGDAL->Align(fdt, AL_UNDER);
	
	cbShow = new CheckBox(fgTypes, TR("&Show"), &m_fShow);
	cbShow->Align(pbs, AL_AFTER);
	new CheckBox(fgGDAL, TR("Con&vert to ILWIS data format"), &m_fConvert);


	fgASTER = new FieldGroup(fgTypes);
	fgASTER->Align(fdt, AL_UNDER);

	new CheckBox(fgASTER, TR("Use Radiances (W/m2/sr/um)"), &m_fUseRadiances);
	new CheckBox(fgASTER, TR("Con&vert to ILWIS data format"), &m_fConvert);

	fgE00 = new FieldGroup(fgTypes);
	fgE00->Align(fdt, AL_UNDER);
	new CheckBox(fgE00, TR("Optionally Create Polygon Map"), &m_fCreatePol);

	fgGartrip = new FieldGroup(fgTypes);
	fgGartrip->Align(fdt, AL_UNDER);
	FieldReal* frGartripOffset = new FieldReal(fgGartrip, TR("Height Offset"), &m_rGartripHeightOffset);
	frGartripOffset->SetIndependentPos();

	String s('x', 60);
	stRemark = new InfoText(root, s);
	stRemark->Align(fgASTER, AL_UNDER);
	stRemark->SetVal(String());

	SetMenHelpTopic("ilwismen\\import_map.htm");
	create();
}

int ImportForm::BrowseClick(Event*)
{
	CSBDestination sb(0, "Select output folder");
	sb.SetFlags(BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT);
	FileName fnData(fdt->sName());
	String sPath = fnData.sPath();
	if (sPath.length() == 0)
		sPath = IlwWinApp()->sGetCurDir();
	if (sPath[sPath.length() - 1] == '\\')
		sPath = sPath.sLeft(sPath.length() - 1);  // remove trailing backslash
	sb.SetInitialSelection(sPath.c_str());
	if (sb.SelectFolder())
	{
		CString sBrowse = sb.GetSelectedFolder();
		fnData.Dir(String(sBrowse));
		if ( fnData.sFileExt().length() < 15)
			fdt->SetVal(fnData.sFullName(false));
		else
			fdt->SetVal("");
	}
	return 1;
}

int ImportForm::CallBack(Event*)
{
	if (fFirstTime)
	{
		fFirstTime = false;
		if (!m_fMenuCmd)
		{
			// menu options import|map (that is the simple "import" command
			// if there was a previous import format select it
			if (sLastImport.length() > 0)
				iOption = fif->ose->SelectString(0, sLastImport.c_str());
			else
				iOption = 0;
		}
		else
		{
			// menu options import|GDAL, and import|general raster
			unsigned int i = 0;
			while (i < imp->imp.size())
			{
				if (fCIStrEqual(imp->imp[i].sCmd, (*sCommand)) )
					break;
				i++;
			}
			if (i < imp->imp.size())  // found the item
			{
				String sDescr = imp->imp[i].sDescr.sLeft(imp->imp[i].sDescr.length() - 5);
				iOption = fif->ose->SelectString(0, sDescr.c_str()); // import submenu options (general raster)
			}
		}
	}
	else
		fif->StoreData();

	cbShow->Hide();
	fgASTER->Hide();
	fgGDAL->Hide();
	fgE00->Hide();
	fgGartrip->Hide();
	if (iOption >= 0 && (unsigned)iOption < imp->imp.size())
	{
		String sCmd = imp->imp[iOption].sCmd;
		if (fCIStrEqual(sCmd, "GDAL"))
		{
			cbShow->Show();
			fgGDAL->Show();
			//SetMenHelpTopic("ilwismen\\import_geogateway.htm");
		}

		if (fCIStrEqual(sCmd, "ASTER") )
		{
			cbShow->Show();
			fgASTER->Show();
			SetMenHelpTopic("ilwismen\\import_aster.htm");			
		}
		else if (fCIStrEqual(sCmd, "raster"))
			SetMenHelpTopic("ilwismen\\import_general_raster.htm");
		else 
		{
			SetMenHelpTopic("ilwismen\\import_map.htm");
			if (fCIStrEqual(sCmd, "e00"))
				fgE00->Show();
			else if (fCIStrEqual(sCmd, "gartrip"))
				fgGartrip->Show();
		}
	}

	fdtl->SetExt(imp->imp[iOption].sExt);
	FileCallBack(0);
	return 1;  
}

int ImportForm::FileCallBack(Event*)
{
	fdtl->StoreData();
	FileName fn(sObjectName);
	if (fn.fValid() && fn.sFile.length() < 15)
		fdt->SetVal(fn.sFile);
	else
	{
		fdt->SetVal("");
	}
	return 1;
}

int ImportForm::OutputFileCallBack(Event*)
{
	fdt->StoreData();
	FileName fnOutput(fdt->sName());  // output name
	if (fnOutput.fValid())
	{
		EnableOK();
		Directory dir(fnOutput);
		if (fnOutput.fExist())
			stRemark->SetVal(TR("File already exists"));
		else if (dir.fReadOnly())
		{
			stRemark->SetVal(TR("Cannot write output, directory is read/only"));
			DisableOK();
		}
		else
			stRemark->SetVal(String());
	}
	return 1;
}

String ImportForm::sBuildCommandLine()
{
	String sType = imp->imp[iOption].sCmd;
	if (fCIStrEqual(sType, "raster"))
		return String();   // general raster has its own second window; no need to start a thread

	String sCmd;
	FileName fnInput(sObjectName);  // input data
	FileName fnOutput(fdt->sName());  // output name
	String sOutputPath = fnOutput.sPath();
	if (fCIStrEqual(sType, "e00"))
	{
		sCmd = String("import %S(%S, %S", sType, fnInput.sRelativeQuoted(), fnOutput.sRelativeQuoted());
		if (m_fCreatePol)
			sCmd &= ", CreatePol";
		sCmd &= ")";
	}
	else if (fCIStrEqual(sType, "gartrip"))
	{
		sCmd = String("import %S(%S, %S", sType, fnInput.sRelativeQuoted(), fnOutput.sRelativeQuoted());
		if (m_rGartripHeightOffset != 0.0)
			sCmd &= String(", %lf", m_rGartripHeightOffset);
		sCmd &= ")";
	}
	else if (fCIStrEqual(sType, "Aster") || fCIStrEqual(sType, "GDAL"))
	{
		sCmd = String("open %S", fnInput.sRelativeQuoted());
		if (m_fConvert)
			sCmd &= " -import";
		if (!fCIStrEqual(fnInput.sRelativeQuoted(false), fnOutput.sRelativeQuoted(false)))
			sCmd &= String(" -output=%S", fnOutput.sRelativeQuoted(false));			
		if (!m_fShow)
			sCmd &= " -noshow"	;
		if ( !m_fUseRadiances )
			sCmd &= " -usednvalues";
		sCmd &= String(" -method=%S", sType);
	}
	else
		sCmd = String("import %S(%S, %S)", sType, fnInput.sRelativeQuoted(), fnOutput.sRelativeQuoted());

	return sCmd;
}

int ImportForm::exec()
{
	FormWithDest::exec();
	*sName = sObjectName;
	*sCommand = imp->imp[iOption].sCmd;
	*m_pfShow = m_fShow;

	if (!m_fMenuCmd)  // do not change the registry for special menu started imports
	{
		try
		{
			IlwisSettings settings("ImportExport\\Import");
			
			String s;
			s = imp->imp[iOption].sDescr;
			if (s.length() > 5)
				s = s.sLeft(s.length() - 5);  // account for the extension info (mostly .xxx)
			settings.SetValue("Import", s);
		}
		catch (...)
		{
			// only catch registry errors; do nothing
		}
	}

	String sCmd = sBuildCommandLine();
	if (sCmd.length() > 0)  // only for "general raster" this is not executed; that is handled separately
	{
		char* str = sCmd.sVal();

		IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
	}

	return 1;  
}

Importing::Importing()
{
	FileName fn("import.def");
	fn.Dir(IlwWinApp()->Context()->sIlwDir() + "Resources\\Def\\");
	try {
		File fil(fn);
		fil.KeepOpen(true);
		while (!fil.fEof()) {
			String s;
			fil.ReadLnAscii(s);
			TextInput txtinp(s);
			TokenizerBase tokenizer(&txtinp);
			Token tok;
			ImportItem item;
			tok = tokenizer.tokGet();
			item.sCmd = tok.sVal();
			tok = tokenizer.tokGet();
			item.sExt = tok.sVal();
			tok = tokenizer.tokGet();
			item.sDescr = tok.sVal();
			imp &= item;
		}
		fil.KeepOpen(false);
	}
	catch (const ErrorObject& err) {
		err.Show();
	}
}

struct WinAndParmImport
{ // struct for parms of import thread
	WinAndParmImport(): sParams(0), sPath(0) {}
	~WinAndParmImport() 
	{ 
		delete sParams; 
		delete sPath;
	}
	String* sPath;
	String* sParams;
};

static UINT DoImportInThread(LPVOID p)
{
	WinAndParmImport *wp = (WinAndParmImport*)(p);
	IlwWinApp()->SetCurDir(*(wp->sPath));
	CWnd* w = IlwWinApp()->GetMainWnd();
	String sOptions = *(wp->sParams);

	IlwWinApp()->Context()->InitThreadLocalVars();
	
	Import(sOptions);

	IlwWinApp()->Context()->RemoveThreadLocalVars();
	
	delete wp;
	wp = NULL;

	return FALSE;
}

// AskImport is called:
// 1. without parameters: 
//         show the import form
// 2. with parameters "-generalraster" or "-gdal" or "-tablewizard": 
//         show the import form with preselected format
// 3. with a command line (not including the "import" command:
//         start the import in a thread
void Importing::AskImport(CWnd* w, const char* sPar)
{
	String sCmd(sPar);
	ParmList pm(sCmd);

	bool fGenRas = pm.fExist("generalraster");
	bool fGDAL = pm.fExist("gdal");
	bool fTableWzd = pm.fExist("tablewizard");

	String sName, sFormat, sOutput;
	// preselect format for "Import Map" and "Import Via menu options
	if (fGenRas)
		sFormat = "raster";
	else if (fGDAL)
		sFormat = "gdal";

	bool fShow = false;
	if (sCmd.length() == 0 || fGenRas || fGDAL || fTableWzd)
	{
		if (fTableWzd)
		{
			AsciiTableWizard frm(w, FileName(), FileName(), TableExternalFormat::ifUnknown);
			if (frm.DoModal() == ID_WIZFINISH ) 
			{
				String s = frm.sBuildExpression();

				char* str = s.sVal();
				IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
			}
		}
		else
		{
			ImportForm form(w, this, &sName, &sFormat, &sOutput, &fShow);
			if (form.fOkClicked() && fCIStrEqual(sFormat, "raster"))
			{
				FileName fn(sName);
				//ImportRasterForm frm(w, fn, sOutput);
			}
		}
	}
	else
	{
		WinAndParmImport *wp = new WinAndParmImport;
		wp->sParams = new String(sCmd);
		wp->sPath = new String(IlwWinApp()->sGetCurDir());
		AfxBeginThread(DoImportInThread, (LPVOID)(wp)); 
	}
}

