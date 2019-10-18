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
// ScriptForm.cpp: implementation of the ScriptForm class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\AppFormsPCH.h"
#include "Engine\Scripting\Script.h"
#include "Client\ilwis.h"
#include "Client\Forms\ScriptForm.h"
#include "Client\FormElements\fldval.h"
#include "Client\FormElements\fldtbl.h"
#include "Headers\Hs\Script.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(ScriptForm, FormBaseDialog)
  ON_COMMAND(IDHELP, OnHelp)
  ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()    

ScriptForm::ScriptForm(const Script& script, const String& parms)
:	FormWithDest(0, 0 != script->sDescr().length() ? script->sDescr() : script->sTypeName(), fbsSHOWALWAYS & ~fbsMODAL),
	scr(script)
{
	ParmList pl(parms);
	iParams = scr->iParams();
	sVal = new String[iParams];
	for (int i = 0; i < iParams; ++i) {
		String sQuestion = scr->sParam(i);
		String sDefault = scr->sDefaultValue(i);
		sVal[i] = (i < pl.iSize()) ? pl.sGet(i) : sDefault;
		switch (scr->ptParam(i))
		{
			case ScriptPtr::ptSTRING:
				new FieldString(root, sQuestion, &sVal[i]);
				break;
			case ScriptPtr::ptFILENAME:
				new FieldString(root, sQuestion, &sVal[i]);
				break;
			case ScriptPtr::ptCOLUMN:
				new FieldString(root, sQuestion, &sVal[i]);
				break;
			case ScriptPtr::ptVALUE:	{
				DomainValueRangeStruct dvrs(-1e9,1e9,0.001);
				new FieldVal(root, sQuestion, dvrs, &sVal[i]);
				break; }
			case ScriptPtr::ptDOMAIN:
				new FieldDomain(root, sQuestion, &sVal[i]);
				break;
			case ScriptPtr::ptRPR:
				new FieldRepresentation(root, sQuestion, &sVal[i]);
				break;
			case ScriptPtr::ptGEOREF:
				new FieldGeoRefExisting(root, sQuestion, &sVal[i]);
				break;
			case ScriptPtr::ptCOORDSYS:
				new FieldCoordSystem(root, sQuestion, &sVal[i]);
				break;
			case ScriptPtr::ptRASMAP:
				new FieldMap(root, sQuestion, &sVal[i]);
				break;
			case ScriptPtr::ptSEGMAP:
				new FieldSegmentMap(root, sQuestion, &sVal[i]);
				break;
			case ScriptPtr::ptPOLMAP:
				new FieldPolygonMap(root, sQuestion, &sVal[i]);
				break;
			case ScriptPtr::ptPNTMAP:
				new FieldPointMap(root, sQuestion, &sVal[i]);
				break;
			case ScriptPtr::ptTABLE:
		    new FieldDataType(root, sQuestion, &sVal[i], ".TBT", true);
				break;
			case ScriptPtr::ptMAPVIEW:
		    new FieldDataType(root, sQuestion, &sVal[i], ".MPV", true);
				break;
			case ScriptPtr::ptMAPLIST:
		    new FieldDataType(root, sQuestion, &sVal[i], ".MPL", true);
				break;
			case ScriptPtr::ptTBL2D:
		    new FieldDataType(root, sQuestion, &sVal[i], ".TA2", true);
				break;
			case ScriptPtr::ptANNTXT:
		    new FieldDataType(root, sQuestion, &sVal[i], ".ATX", true);
				break;
			case ScriptPtr::ptSMS:
		    new FieldDataType(root, sQuestion, &sVal[i], ".SMS", true);
				break;
			case ScriptPtr::ptMATRIX:
		    new FieldDataType(root, sQuestion, &sVal[i], ".MAT", true);
				break;
			case ScriptPtr::ptFILTER:
		    new FieldDataType(root, sQuestion, &sVal[i], ".FIL", true);
				break;
			case ScriptPtr::ptFUNCTION:
		    new FieldDataType(root, sQuestion, &sVal[i], ".FUN", true);
				break;
			case ScriptPtr::ptSCRIPT:
		    new FieldDataType(root, sQuestion, &sVal[i], ".ISL", true);
				break;
		}
	}
	//IlwWinApp()->setHelpItem(""); // set help button on
	root->psn->SetCol(0); // initialize positioner columns
	short iMax = 0;
	MinWidthOfColumn(root->psn, 1, iMax); // compute the maximum of the iMinWidths for column 1
	if (iMax < 250)
		SetMinWidthOfColumn(root->psn, 1, 250);
	create();
  ShowWindow(SW_SHOW);
}

ScriptForm::~ScriptForm()
{
	delete [] sVal;
}

void ScriptForm::MinWidthOfColumn(FormEntryPositioner * psn, short iColNr, short& iMax)
// gets the maximum of the minimum width values of each positioner in column iColNr
// then check it's child positioners
{
    if ((psn->iCol == iColNr) && (psn->iMinWidth > iMax))
        iMax = psn->iMinWidth;
    for (short i = 0; i < psn->psnChild.iSize(); i++)
        MinWidthOfColumn(psn->psnChild[i], iColNr, iMax);
}

void ScriptForm::SetMinWidthOfColumn(FormEntryPositioner * psn, short iColNr, short iWid)
// sets the minimum width of each positioner in column iColNr to iWid
// then set it's child positioners
{
    if (psn->iCol == iColNr)
        psn->iMinWidth = iWid;
    for (short i = 0; i < psn->psnChild.iSize(); i++)
    {
        if (!psn->psnChild[i]->fIndependentPos)
            SetMinWidthOfColumn(psn->psnChild[i], iColNr, iWid);
    }
}

String ScriptForm::sExec()
{
	String sRes;
	for (int i = 0; i < iParams; ++i) {
		if (i > 0)
			sRes &= " ";
    String s = sVal[i];
    switch (scr->ptParam(i)) {
      case ScriptPtr::ptSTRING: {
        if ((s[0] != '\"') && (s[s.length()-1] != '\"'))
          s = String("\"%S\"", s);
        break;
      }
      case ScriptPtr::ptVALUE: { // do nothing more
        break;
      }
      case ScriptPtr::ptFILENAME: 
			{
				FileName fn(s);
        s = fn.sRelativeQuoted();
        break;
      }
      case ScriptPtr::ptCOLUMN: {
        s = s.sQuote();
        break;
      }
      default: { // all others are ilwis objects; remove default path
        FileName fn(s);
        bool fExt = scr->fParamIncludeExtension(i);
        s = fn.sRelativeQuoted(fExt, scr->fnObj.sPath());
      }
    }
    sRes &= s;
	}
	return sRes;
}

int ScriptForm::exec()
{
  FormWithDest::exec();

	String sDir = IlwWinApp()->sGetCurDir();
	String sCommand("run %S %S",scr->sNameQuoted(), sExec());
	IlwWinApp()->Execute(sCommand);

  return 1;
}

void ScriptForm::OnHelp()
{
	FileName fn = scr->fnObj;

	FileName fnCss = fn;
	fnCss.sFile = "ilwis";
	fnCss.sExt = ".css";
	if (!File::fExist(fnCss)) {
		String sOrig = IlwWinApp()->Context()->sIlwDir() + "Scripts\\ilwis.css";
		CopyFile(sOrig.c_str(), fnCss.sFullPath().c_str(), TRUE);
	}

	fn.sExt = ".htm";
	if (!File::fExist(fn)) {
		ofstream os(fn.sFullName().c_str());
		os << "<html>\n<head>\n<title>" 
			<< fn.sFile.c_str() << "</title>\n"
			<< "<meta name=\"Generator\" content=\"ILWIS Script Form\">\n"
			<< "<link rel=stylesheet type=\"text/css\" href=\"ilwis.css\">\n"
			<< "</head>\n"
			<< "<body text=\"#000000\" bgcolor=\"#FFFFFF\">\n"
			<< "<h1 class=only1>Script " << fn.sFile.c_str() << "</h1>\n"
			<< "<p>" << scr->sDescription.c_str() << "</p>\n"
			<< "<p class=defnewpar>Some more explanation could be written.</p>\n"
			<< "<p class=diakopje>Dialog box options:</p>\n"
			<< "<table cellspacing=0>\n";
		int iParams = scr->iParams();
		for (int i = 0; i < iParams; ++i) {
			String sParam = scr->sParam(i);
			ScriptPtr::ParamType pt = scr->ptParam(i);
			String sDefault = scr->sDefaultValue(i);
			String sType;
			switch (pt) 
			{
				case ScriptPtr::ptRASMAP: sType =  TR("Raster Map.mpr"); break;
				case ScriptPtr::ptSEGMAP: sType =  TR("Segment Map.mps"); break;
				case ScriptPtr::ptPOLMAP: sType =  TR("Polygon Map.mpa"); break;
				case ScriptPtr::ptPNTMAP: sType =  TR("Point Map.mpp"); break;
				case ScriptPtr::ptTABLE: sType =  TR("Table.tbt"); break;
				case ScriptPtr::ptCOLUMN: sType =  TR("Column.clm"); break;
				case ScriptPtr::ptMAPLIST: sType =  TR("Map List.mpl"); break;
				case ScriptPtr::ptMAPVIEW: sType =  TR("Map View.mpv"); break;
				case ScriptPtr::ptCOORDSYS: sType =  TR("CoordSystem.csy"); break;
				case ScriptPtr::ptGEOREF: sType =  TR("GeoReference.grf"); break;
				case ScriptPtr::ptDOMAIN: sType =  TR("Domain.dom"); break;
				case ScriptPtr::ptRPR: sType =  TR("Representation.rpr"); break;
				case ScriptPtr::ptFILTER: sType =  TR("Filter.fil"); break;
				case ScriptPtr::ptSCRIPT: sType =  TR("Script.isl"); break;
				case ScriptPtr::ptFUNCTION: sType =  TR("Function.fun"); break;
				case ScriptPtr::ptMATRIX: sType =  TR("Matrix.mat"); break;
				case ScriptPtr::ptSMS: sType =  TR("Sample Set.sms"); break;
				case ScriptPtr::ptTBL2D: sType =  TR("2-Dimensional Table.ta2"); break;
				case ScriptPtr::ptANNTXT: sType =  TR("Annotation Text.atx"); break;
				case ScriptPtr::ptSTRING : sType =  TR("String"); break;
				case ScriptPtr::ptVALUE  : sType =  TR("Value"); break;
				case ScriptPtr::ptFILENAME: sType =  TR("Filename"); break;
			}
			FileName fnType(sType);
			sType = fnType.sFile;
			os << "<tr><td valign=\"top\" width=\"30%\"><p class=diabox>" << sParam.c_str() << ":</p></td>\n<td valign=\"top\"><p class=diabox>" 
				<< "Select a " << sType << " parameter to be used as " << sParam.c_str() << ".\n";
			if (sDefault != "")
				os << "Default is <i>" << sDefault.c_str() << "</i>";
			else 
				os << "There is no default value, you always have to specify this parameter yourself.";
			os << "</p></td>\n";
		}
		os << "</table>\n"
			<< "<p class=tip>Tip:</p>\n"
			<< "<p class=tiptext>This help text is automatically generated and can be changed by the script author.</p>\n"
			<< "<p class=seealso>See also:</p>\n"
			<< "<p class=seealsolinks><a href=\"ilwis:/ilwis/script_editor_functionality.htm\">Script Editor Functionality</a></p>\n"
			<< "<p class=seealsolinks><a href=\"ilwis:/ilwis/how_to_use_parameters_in_scripts.htm\">How to use parameters in Scripts</a></p>\n"
			<< "<p class=seealsolinks><a href=\"http://www.itc.nl/ilwis\">ILWIS on the Web</a></p>\n"
			<< "</body>\n"
			<< "</html>";
	}
	String sFile = fn.sFullPath();
}


