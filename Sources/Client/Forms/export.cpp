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
/* $Log: /ILWIS 3.0/ApplicationForms/export.cpp $
 * 
 * 26    1-10-04 14:57 Retsios
 * Added export option for gdal
 * 
 * 25    2-09-02 18:42 Hendrikse
 * added added .csy,  expCsy, fefCsy similar to maplist .mpl treatment to
 * implement interface of coordsystem export to ArcGis
 * 
 * 24    5-07-02 19:20 Koolhoven
 * prevent warnings with Visual Studio .Net
 * 
 * 23    11/22/01 12:27p Martin
 * new parsing for the sPar expression, Split lost its space, tab because
 * it conflicts with longfilenames
 * 
 * 22    11/12/01 2:04p Martin
 * for splitting the parms when dropping a map on a form, pamrlist is used
 * instead of Split. Split can not handle long filenames
 * 
 * 21    16/03/01 9:16 Willem
 * Removed double code
 * 
 * 20    16/03/01 9:11 Willem
 * Export now checks if a single argument is passed to the form. If so it
 * will be interpreted as an existing filename and used to set the
 * selection to this object in the object list on the form
 * 
 * 19    9-02-01 3:32p Martin
 * made the combo for gdb format somewhat larger
 * 
 * 18    6-02-01 4:34p Martin
 * extra check to prevent illegal input from the GDBFormatInfo.def file
 * 
 * 17    5/02/01 15:13 Willem
 * Export form now also stores last choice for Geogateway export. Form
 * interaction now corrected
 * 
 * 16    30/01/01 17:02 Willem
 * Added filename quoting in export command string
 * 
 * 15    12-01-01 9:28a Martin
 * added support for export through geogateway
 * 
 * 14    8/01/01 14:08 Willem
 * Typing error in allowable tables for export disabled both .hsa and .hss
 * export.
 * 
 * 13    23/11/00 16:37 Willem
 * All histograms can now be exported as tables
 * 
 * 12    14/11/00 16:46 Willem
 * Added a browse button on the export form
 * 
 * 11    18/08/00 17:49 Willem
 * The format list for table is now correct (again)
 * 
 * 10    11/08/00 16:40 Willem
 * Added remark to disallow entering long filenames when export to ilwis
 * format before version 3
 * 
 * 9     15/06/00 16:10 Willem
 * Removed TRACE statement
 * 
 * 8     15/06/00 16:09 Willem
 * Added protection against array indices out of bounds in the string
 * format arrays
 * 
 * 7     8-05-00 8:49a Martin
 * added pragma to stop warning 4786
 * 
 * 6     4/05/00 15:51 Willem
 * The last selected format for each object type is now saved in the
 * registry and is now selected as the default when the export form is
 * opened again
 * 
 * 5     4/05/00 12:11 Willem
 * Output file name is now passed on from the form (again)
 * 
 * 4     4/05/00 11:36 Willem
 * Adjusted callback to display the output filename before function exits
 * 
 * 3     1/05/00 16:37 Willem
 * Export form now checks output names; in case of ilwis output they must
 * start with a letter.
 * 
 * 2     29/02/00 17:13 Willem
 * The ExportToExternal() function does no longer exist and has been
 * replaced by ::Export()
 * 
 * 1     25/02/00 18:12 Willem
 * Ported Export forms from v2.23
 */
// Revision 1.10  1998/09/16 17:33:54  Wim
// 22beta2
//
// Revision 1.9  1998/05/26 18:42:47  Willem
// Cleaned up the export object code. The code has been moved to conv.c
// and the export function is now calling the function in conv.c.
//
// Revision 1.8  1998/03/13 16:33:44  Willem
// Accidentely removed a statement, re-added again.
//
// Revision 1.7  1998/03/13 16:31:47  Willem
// Added removal of temporary files before creating new temporary files
//
// Revision 1.6  1998/02/09 14:17:26  Wim
// Export form calls SetNoSystemDir()
//
// Revision 1.5  1998-02-03 22:50:52+01  Willem
// Temporary files are now placed in the directory pointed to by the
// TEMP environment variable or the TMP environment variable.
// If both are not set the current directory is used.
//
// Revision 1.4  1997/08/13 10:32:50  Wim
// Use in Export() the parameter on the commandline as default object name
//
// Revision 1.3  1997-07-30 22:54:32+02  Willem
// When the conversion to 1.4 fails (exporting to TIF for example) the time
// of the -cnvtmp- file is checked to catch this failure. The convert 14 will
// not be started in that case, preventing a waiting dos box.
//
// Revision 1.2  1997/07/28 20:14:15  Willem
// The temporary files are now created in the output directory. This is
// done to have the place of the temporary files explicitely defined.
//
/* Export
// by Wim Koolhoven
// (c) ILWIS System Development, ITC
	Last change:  WN   15 Jun 98    1:03 pm
*/
#define EXPORT_C

#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "Client\Forms\EXPORT.H"
#include "Client\Forms\expobj.h"
#include "Engine\Base\Tokbase.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\FormElements\flddat.h"

#include "Engine\DataExchange\CONV.H"
#include "Headers\Htp\Ilwismen.htp"
#include "Headers\Hs\Appforms.hs"
#include "Client\Editors\Utils\SBDestination.h"
#include "Client\ilwis.h"

#include <map>

class FieldExportFormat: public FieldOneSelect
{
public:
  FieldExportFormat(FormEntry* parent, long* value, const Array<ExportItem>& arr)
  : FieldOneSelect(parent, value), aei(arr)
  {
    SetFieldWidth(2*FLDNAMEWIDTH+20);
    SetIndependentPos();
  }
  void create();
private:
  const Array<ExportItem>& aei;  
};

void FieldExportFormat::create()
{
  FieldOneSelect::create();
  for (int i = 0; i < aei.iSize(); ++i)
    ose->AddString(aei[i].sDescription.c_str());
  ose->SetCurSel(0);
}

class ExportForm: public FormWithDest
{
public:
	ExportForm(CWnd*, Exporting*, String* sName, String* sCmd, String* sOutput);
	
private:
	int    CallBackOutName(Event*);
	int    CallBackExportMethod(Event*);
	int    CallBack(Event*);
	int    BrowseClick(Event*);
	bool   fIsExportToIlwis();
	int    exec();
	
	void   LoadSettings();
	String sGetCommand(Array<ExportItem>& lst, int iNdx);
	
	Exporting* exp;

	FieldDataTypeLarge    *fdtl;
	PushButton            *pbs;
	RadioGroup            *rgExportMethod;
	RadioButton           *rbILWIS, *rbGDAL, *rbGDB;
	FieldExportFormat     *fefRas, *fefRasGDAL, *fefRasGDB, *fefSeg, *fefPol, *fefPnt, *fefTbl, *fefMpl, *fefCsy;
	map<String, String>   mssPrevExport;
	FieldString           *fsOutName;
	StaticText            *stRemark;
	StaticText						*stMethod;

	String                sObjectName, *sOutputName;
	String                *sName;
	String                *sCommand;
	long                  iOption;
	int										iExportMethod; // 0 == ILWIS native Export, 1 == GDAL Export, 2 == GeoGateway
	bool                  fInMethodCallback;
};

ExportForm::ExportForm(CWnd* wPar, Exporting* exporting, String* sNam, String* sCmd, String* sOutput)
: FormWithDest(wPar, TR("Export")), 
  exp(exporting), sName(sNam), sCommand(sCmd), iExportMethod(0), fInMethodCallback(false)
{
	sObjectName = *sName;
	sOutputName = sOutput;

	LoadSettings();

	fdtl = new FieldDataTypeLarge(root, &sObjectName, ".mpr.mpa.mps.mpp.tbt.his.hsp.hsa.hss.mpl.csy");
	fdtl->SetNoSystemDir();
	fdtl->SetCallBack((NotifyProc)&ExportForm::CallBack);
	stMethod = new StaticText(root, TR("&Method"));
	rgExportMethod = new RadioGroup(root,"", &iExportMethod, true);
	rbILWIS = new RadioButton(rgExportMethod, TR("ILWIS"));
	rbILWIS->SetIndependentPos();
	//rbGDAL = new RadioButton(rgExportMethod, TR("GDAL"));
	//rbGDAL->SetIndependentPos();
	rbGDB = new RadioButton(rgExportMethod, TR("GDAL"));
	rbGDB->SetIndependentPos();
	rgExportMethod->SetCallBack((NotifyProc)&ExportForm::CallBackExportMethod);

	StaticText* st = new StaticText(root, TR("&Format"));
	iOption = 0;
	fefRas = new FieldExportFormat(root, &iOption, exp->expRas);
	fefRas->Align(st, AL_UNDER);
	fefRasGDAL = new FieldExportFormat(root, &iOption, exp->expGDALRas);
	fefRasGDAL->Align(st, AL_UNDER);
	fefRasGDAL->SetWidth(130);
	fefRasGDB = new FieldExportFormat(root, &iOption, exp->expGDBRas);
	fefRasGDB->Align(st, AL_UNDER);
	fefRasGDB->SetWidth(130);
	fefSeg = new FieldExportFormat(root, &iOption, exp->expSeg);
	fefSeg->Align(st, AL_UNDER);
	fefPol = new FieldExportFormat(root, &iOption, exp->expPol);
	fefPol->Align(st, AL_UNDER);
	fefPnt = new FieldExportFormat(root, &iOption, exp->expPnt);
	fefPnt->Align(st, AL_UNDER);
	fefTbl = new FieldExportFormat(root, &iOption, exp->expTbl);
	fefTbl->Align(st, AL_UNDER);
	fefMpl = new FieldExportFormat(root, &iOption, exp->expMpl);
	fefMpl->Align(st, AL_UNDER);
	fefCsy = new FieldExportFormat(root, &iOption, exp->expCsy);
	fefCsy->Align(st, AL_UNDER);
	FieldBlank* fb = new FieldBlank(root);
	fb->Align(fefTbl, AL_UNDER);
	fsOutName = new FieldString(root, TR("&Output Filename"), sOutputName);
	fsOutName->SetWidth(120);
	fsOutName->SetCallBack((NotifyProc)&ExportForm::CallBackOutName);
	fsOutName->SetIndependentPos();
	pbs = new PushButton(root, "...", (NotifyProc)&ExportForm::BrowseClick);
	pbs->psn->iPosX = fsOutName->psn->iPosX + fsOutName->psn->iMinWidth;
	pbs->psn->iMinWidth = 18;
	pbs->Align(fsOutName, AL_AFTER);
	pbs->SetIndependentPos();

	String s('x', 60);
	stRemark = new StaticText(root, s);
	stRemark->Align(fsOutName, AL_UNDER);
	SetMenHelpTopic("ilwismen\\export.htm");
	create();
}

void ExportForm::LoadSettings()
{
	IlwisSettings settings("ImportExport\\Export");
	String sKey;
	sKey = settings.sValue("RasterExport", String());
	if (sKey.length() > 0) mssPrevExport[".mpr"] = sKey;
	sKey = settings.sValue("RasterExportGeoGateway", String());
	if (sKey.length() > 0) mssPrevExport[".mprgeo"] = sKey;
	sKey = settings.sValue("RasterExportGDAL", String());
	if (sKey.length() > 0) mssPrevExport[".mprgdal"] = sKey;
	sKey = settings.sValue("PolygonExport", String());
	if (sKey.length() > 0) mssPrevExport[".mpa"] = sKey;
	sKey = settings.sValue("SegmentExport", String());
	if (sKey.length() > 0) mssPrevExport[".mps"] = sKey;
	sKey = settings.sValue("PointExport", String());
	if (sKey.length() > 0) mssPrevExport[".mpp"] = sKey;
	sKey = settings.sValue("TableExport", String());
	if (sKey.length() > 0) mssPrevExport[".tbt"] = sKey;
	sKey = settings.sValue("MapListExport", String());
	if (sKey.length() > 0) mssPrevExport[".mpl"] = sKey;
	sKey = settings.sValue("CoordSystemExport", String());
	if (sKey.length() > 0) mssPrevExport[".csy"] = sKey;
}

int ExportForm::BrowseClick(Event*)
{
	CSBDestination sb(0, "Select output folder");
	sb.SetFlags(BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT);
	FileName fnData(fsOutName->sVal());
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
		fsOutName->SetVal(fnData.sFullName(false));
	}
	return 1;
}

int ExportForm::CallBackExportMethod(Event*)
{
	if (fInMethodCallback)
		return 1;

	rgExportMethod->StoreData();
	if (2 == iExportMethod) // GeoGateway
	{
		fdtl->SetExt(".mpr");
		iOption = fefRasGDB->ose->SelectString(0, mssPrevExport[".mprgeo"].c_str());
	}
	else if (1 == iExportMethod) // GDAL
	{
		fdtl->SetExt(".mpr");
		iOption = fefRasGDAL->ose->SelectString(0, mssPrevExport[".mprgdal"].c_str());
	}
	else // ILWIS native Export
	{
		fdtl->SetExt(".mpr.mpa.mps.mpp.tbt.his.hsp.hsa.hss.mpl.csy");
		iOption = fefRas->ose->SelectString(0, mssPrevExport[".mpr"].c_str());
	}

	CallBack(0);
	
	return 1;
}

int ExportForm::CallBackOutName(Event*)
{
	fsOutName->StoreData();
	fdtl->StoreData();

	FileName fn(*sOutputName);

	if (sObjectName.length() == 0)
	{
		stRemark->SetVal("");
		DisableOK();
		return 1;
	}
	else if (sOutputName->length() == 0)
	{
		stRemark->SetVal(TR("Output file name required"));
		DisableOK();
		return 1;
	}
	else if (fIsExportToIlwis())  // for versions before ILWIS 3
	{
		if (!fn.fValid())
		{
			stRemark->SetVal(TR("ILWIS object names must begin with a letter"));
			DisableOK();
			return 1;
		}
		if (fn.sFile.length() > 8 || fn.sExt.length() > 3)
		{
			stRemark->SetVal(TR("Filename must be a short name (8.3)"));
			DisableOK();
			return 1;
		}
	}

	stRemark->SetVal("");
	EnableOK();

	return 1;
}

int ExportForm::CallBack(Event*)
{
	fdtl->StoreData();
	
	fefRas->Hide();
	fefRasGDAL->Hide();
	fefRasGDB->Hide();
	fefSeg->Hide();
	fefPol->Hide();
	fefPnt->Hide();
	fefTbl->Hide();
	fefMpl->Hide();
	fefCsy->Hide();
	stMethod->Hide();
	rgExportMethod->Hide();
	
	*sName = sObjectName;
	FileName fn(sObjectName);
	fsOutName->SetVal(fn.sFile);
	if (sName->length() == 0)
		return 1;
	
	if (fn.sExt == ".mps")
	{
		iOption = fefSeg->ose->SelectString(0, mssPrevExport[".mps"].c_str());
		fefSeg->Show();
	}
	else if (fn.sExt == ".mpa")
	{
		iOption = fefPol->ose->SelectString(0, mssPrevExport[".mpa"].c_str());
		fefPol->Show();
	}
	else if (fn.sExt == ".mpp")
	{
		iOption = fefPnt->ose->SelectString(0, mssPrevExport[".mpp"].c_str());
		fefPnt->Show();
	}
	else if (fn.sExt == ".tbt" || fn.sExt == ".his" || fn.sExt == ".hsa" || fn.sExt == ".hsp" || fn.sExt == ".hss")
	{
		iOption = fefTbl->ose->SelectString(0, mssPrevExport[".tbt"].c_str());
		fefTbl->Show();
	}
	else if (fn.sExt == ".mpl")
	{
		iOption = fefMpl->ose->SelectString(0, mssPrevExport[".mpl"].c_str());
		fefMpl->Show();
	}
	else if (fn.sExt == ".csy")
	{
		iOption = fefCsy->ose->SelectString(0, mssPrevExport[".csy"].c_str());
		fefCsy->Show();
	}
	else
	{
		fInMethodCallback = true;  // prevent callback of RadioGroup to do something
		stMethod->Show();
		rgExportMethod->Show();
		fInMethodCallback = false;

		if (2 == iExportMethod) // GeoGateway
		{
			iOption = fefRasGDB->ose->SelectString(0, mssPrevExport[".mprgeo"].c_str());
			fefRasGDB->Show();
		}
		else if (1 == iExportMethod) // GDAL
		{
			iOption = fefRasGDAL->ose->SelectString(0, mssPrevExport[".mprgdal"].c_str());
			fefRasGDAL->Show();
		}
		else // ILWIS native raster export
		{
			iOption = fefRas->ose->SelectString(0, mssPrevExport[".mpr"].c_str());
			fefRas->Show();
		}
	}
	if (iOption == -1)
		iOption = 0;  // select first in list when not found in list
	
	return 1;  
}

String ExportForm::sGetCommand(Array<ExportItem>& lst, int iNdx)
{
	if (iNdx >= 0 && (unsigned)iNdx < lst.size())
		return lst[iNdx].sCmd;
	else
		return String();
}

bool ExportForm::fIsExportToIlwis()
{
	root->StoreData();
	if (sOutputName->length() == 0)
		return false;

	String sExCmd;
	*sName = sObjectName;
	FileName fn(*sName);
	if (fn.sExt == ".mpr")
	{
		if (2 == iExportMethod) // GeoGateway
			sExCmd = sGetCommand(exp->expGDBRas, iOption);
		else if (1 == iExportMethod) // GDAL
			sExCmd = sGetCommand(exp->expGDALRas, iOption);
		else // // ILWIS native raster export
			sExCmd = sGetCommand(exp->expRas, iOption);
	}
	else if (fn.sExt == ".mps")
		sExCmd = sGetCommand(exp->expSeg, iOption);
	else if (fn.sExt == ".mpa")
		sExCmd = sGetCommand(exp->expPol, iOption);
	else if (fn.sExt == ".mpp")
		sExCmd = sGetCommand(exp->expPnt, iOption);
	else if (fn.sExt == ".mpl")
		sExCmd = sGetCommand(exp->expMpl, iOption);
	else if (fn.sExt == ".csy")
		sExCmd = sGetCommand(exp->expCsy, iOption);
	else if (fn.sExt == ".tbt" || fn.sExt == ".his" || fn.sExt == ".hsa" || fn.sExt == ".hsp" || fn.sExt == ".hss")
		sExCmd = sGetCommand(exp->expTbl, iOption);

	if (sExCmd.length() > 0 && fCIStrEqual(sExCmd.sLeft(5), "Ilwis"))
		return true;
	else
		return false;
}

int ExportForm::exec()
{
	FormWithDest::exec();
	*sName = sObjectName;
	FileName fn(sObjectName);

	IlwisSettings settings("ImportExport\\Export");

	String s;
	String sOutputExt;
	if (fn.sExt == ".mpr")
	{
		ExportItem ei;
		switch(iExportMethod)
		{
		case 0: // ILWIS native raster export
			ei = exp->expRas[iOption];
			break;
		case 1: // GDAL
			ei = exp->expGDALRas[iOption];
			break;
		case 2: // GeoGateway
			ei = exp->expGDBRas[iOption];
			break;
		default:
			exp->expRas[iOption];
		}
		*sCommand = ei.sCmd;
		s = ei.sDescription;
		sOutputExt = ei.sExt;
		if (2 == iExportMethod) // GeoGateway
			settings.SetValue("RasterExportGeoGateway", s.sLeft(s.length() - 5));
		else if (1 == iExportMethod) // GDAL
			settings.SetValue("RasterExportGDAL", s.sLeft(s.length() - 5));
		else // ILWIS native raster export
			settings.SetValue("RasterExport", s.sLeft(s.length() - 5));
	}
	else if (fn.sExt == ".mps")
	{
		*sCommand = exp->expSeg[iOption].sCmd;
		s = exp->expSeg[iOption].sDescription;
		settings.SetValue("SegmentExport", s.sLeft(s.length() - 5));
	}
	else if (fn.sExt == ".mpa")
	{
		*sCommand = exp->expPol[iOption].sCmd;
		s =  exp->expPol[iOption].sDescription;
		settings.SetValue("PolygonExport", s.sLeft(s.length() - 5));
	}
	else if (fn.sExt == ".mpp")
	{
		*sCommand = exp->expPnt[iOption].sCmd;
		s = exp->expPnt[iOption].sDescription;
		settings.SetValue("PointExport", s.sLeft(s.length() - 5));
	}
	else if (fn.sExt == ".mpl")
	{
		*sCommand = exp->expMpl[iOption].sCmd;
		s = exp->expMpl[iOption].sDescription;
		settings.SetValue("MapListExport", s.sLeft(s.length() - 5));
	}
	else if (fn.sExt == ".csy")
	{
		*sCommand = exp->expCsy[iOption].sCmd;
		s = exp->expCsy[iOption].sDescription;
		settings.SetValue("CoordSystemExport", s.sLeft(s.length() - 5));
	}
	else if (fn.sExt == ".tbt" || fn.sExt == ".his" || fn.sExt == ".hsa" || fn.sExt == ".hsp" || fn.sExt == ".hss")
	{
		*sCommand = exp->expTbl[iOption].sCmd;
		s = exp->expTbl[iOption].sDescription;
		settings.SetValue("TableExport", s.sLeft(s.length() - 5));
	}
	String sExpCmd, sGeo;
	FileName fnInput(*sName);
	FileName fnOutput(*sOutputName);
	if (sOutputExt != "" && fnOutput.sExt == "")
		fnOutput.sExt = sOutputExt;
	if (1 == iExportMethod) // GeoGateway
			sGeo = ",GDAL";
		
	sExpCmd = String("export %S(%S,%S%S)", *sCommand, fnInput.sRelativeQuoted(), fnOutput.sRelativeQuoted(), sGeo);

	IlwWinApp()->Execute(sExpCmd);

	return 1;  
}

Exporting::Exporting()
{
	fFilled = false;
}  
 
void Exporting::Fill(const String& s, Array<ExportItem>& eia)
{
	FileName fn(s);
	fn.Dir(IlwWinApp()->Context()->sIlwDir() + "\\Resources\\Def\\");
	File fil(fn);
	fil.KeepOpen(true);
	while (!fil.fEof()) 
	{
		String s;
		fil.ReadLnAscii(s);
		TextInput txtinp(s);
		TokenizerBase tokenizer(&txtinp);
		Token tok;
		ExportItem item;
		tok = tokenizer.tokGet();
		item.sCmd = tok.sVal();
		tok = tokenizer.tokGet();
		item.sDescription = tok.sVal();
		if (!tokenizer.fEnd()) {
			tok = tokenizer.tokGet();
			item.sExt = tok.sVal();
		}
		eia &= item;
	}
	fil.KeepOpen(false);
}

void Exporting::FillArrays()
{
	try
	{
		if (fFilled)
			return;
		Fill("expras.def", expRas);
		Fill("expseg.def", expSeg);
		Fill("exppol.def", expPol);
		Fill("exppnt.def", expPnt);
		Fill("exptbl.def", expTbl);
		Fill("expmpl.def", expMpl);
		Fill("expcsy.def", expCsy);
		Fill("expgdal.def", expGDALRas);
		
		String sPath = IlwWinApp()->Context()->sIlwDir() + "System";
//		File FormatDef(sPath + "Resources\Def\\GDBFormatInfo.def");
		expGDBRas.clear();
		//while (!FormatDef.fEof() )	
		//{
		//	String sLine;
		//	FormatDef.ReadLnAscii(sLine);	
		//	if ( sLine[0] !=';')
		//	{
		//		Array<String> arParts;
		//		Split(sLine, arParts, ",");
		//		if ( arParts.size() > 6 && arParts[3].iVal() != -1 )
		//		{
		//			ExportItem item;
		//			if ( arParts[6].sTrimSpaces() == "true")
		//			{
		//				item.sCmd = arParts[1].sTrimSpaces();
		//				item.sDescription = arParts[0].sTrimSpaces() + " " + arParts[2].sTrimSpaces() + " .xxx";
		//				expGDBRas.push_back(item);
		//			}						
		//		}				
		//	}			
		//}	
		
		fFilled = true;
	}
	catch (const ErrorObject& err)
	{
		err.Show();
	}
}

// 1. The export command is fully specified, sPar has the
//    syntax: <format>(input, output[, optional parameters])
//    The export command itself was already stripped off.
//    -> simply execute the command
// 2. The export is not fully specified. sPar contains one
//    filename with the object to be exported. (Generated by
//    dropping an ILWIS object on the "export" option in the
//    operation list/tree). The form is displayed with the 
//    object highlighted.
// 3. The export is not fully specified. sPar is empty or
//    has less than 3 parameters -> display the form
void Exporting::Export(CWnd* w, const char* sPar)
{
	Array<String> as;
	String sExpr(sPar);
	Split(sExpr, as, "(,)");
	if (as.iSize() >= 3)
	{
		::Export(sExpr);
		return;
	}

	FillArrays();
	String sName, sFormat, sOutput;
	if (sPar != 0) 
	{
		TextInput ti(sPar);
		TokenizerBase tokenizer(&ti);
		Token tok;
		tok = tokenizer.tokGet();
		sName = tok.sVal();
	}  
	ExportForm form(w, this, &sName, &sFormat, &sOutput);
}

