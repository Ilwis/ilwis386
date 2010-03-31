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
/* $Log: /ILWIS 3.0/TableWindow/Tblforms.cpp $
* 
* 44    23-12-05 16:34 Hendrikse
* New form that comes when Tablecommand "findazimdist" is given on the
* table comm line.
* It is to compute geodetic (ellipsoidal) distance and azimuth between
* latlons (or projected points) from 2 given columns with coordsystem
* domains (CoordsysViaLatLon required)
* The functions rEllipsoidalAzimuth(llFrom, llTo) and
* rEllipsoidalDistance(llFrom, llTo) from class Ellipsoid are used 
* and in case of distances beyond 800 km, rDistance(cTmp1, cTmp2) from
* class Distance is used
* 
* 43    24-10-03 16:50 Willem
* - Changed: The table commands Join, Aggregate and LSQ now let their
* forms handle the default aggregate/regression selection instead of by
* the field. This prevents losing references to temporaries
* 
* 42    29-08-03 18:50 Hendrikse
* changed strict if (iSecondCoordCol >= 1) into less strict if
* (iSecondCoordCol >= 0)
* added 4 checks on 0-pointers (non-defined input columns
* 
* 41    27-05-03 12:38 Willem
* - Changed: Make sure that after RemoveCol the table is stored whole to
* prevent incomplete data on disk. This is done by calling tbl->Store
* instead of col->Store. (This is a temporary solution; in fact the code
* should be revised completely)
* 
* 40    27-03-03 18:10 Hendrikse
* implemeted better Callbacks  for the change of selected columns in
* (undocumented Find7parms function in TableForms project (geodetic datum
* transform)
* added a global iActivePoints param to display to the user in the
* application form
* 
* 39    12-03-03 10:31 Hendrikse
* implemented m_fsCentroidP = new FieldString to allow copying centroid
* coords
* 
* 38    10-03-03 12:04 Hendrikse
* added new callbacks adn removed redund mebers Xcentroid etc.
* Implemented use of Array<double> rX1 etc . Corrected the use of rXpiv,
* rYpiv, rZpiv.
* Extended output with used pivot : x0, y0, z0
* 
* 37    27-02-03 22:06 Hendrikse
* implemented use of variables to construct the Find7ParametersForm and
* its callback functions  and computation algorithm 
* 
* 36    5-02-03 14:28 Hendrikse
*  dvrs of columns can now be changed (change made by Martin was skipped
* by accident; is added again.
* moved ResultForm class and its constructor
* debugged 	rDzErr = sqrt(rDzErr / iActivePnt);
* removed redundent outcommented code
* implemented 2 columnForms (not finished yet)
* 
* 35	20-01-03 8:58 Hendrikse
* added classes to make forms for finding 7 parameters of
* Helmert transformation (Bursa-Wolf)
* from cts1 to cts2 (geocentric systems)
* Find7ParametersForm
*
* 34    22-11-01 11:24 Retsios
* Probably a typing mistake that resulted in bug#5058: the loop should go
* up to the max. of the columns of the same table whose columns are being
* read.
* 
* 33    25-10-01 19:55 Retsios
* Prevent ILWIS from crashing when some columns are invalid (due to
* invalid odf)
* 
* 32    15-08-01 9:56 Koolhoven
* Join Wizard now has htp numbers set
* 
* 31    9-05-01 9:00 Hendrikse
* added classes to make forms for Datum shift finding:
* InverseMolodenskyForm finds dx,dy,da from inverted equations
* FindMolodenskyShiftsForm from LatLonH to CTS conversions
* It works only in tables where 2 coord colums (dom CoordSysLatlon or
* CoordSysProjection) and 2 heights columns are present and a column
* named 'active' 
* 
* 30    27-02-01 19:07 Koolhoven
* ColumnPropForm: create domain form help button added
* 
* 29    2/24/01 15:21 Retsios
* Comment out unnecessary code to repair memory leak
* 
* 28    17-11-00 17:03 Koolhoven
* allow in wizard joining of a table with itself
* in JoinAggrPP::OnSetActive() call FuncCallBack() instead of trying to
* set it our selves
* hide stWeight whenever needed
* 
* 27    13-11-00 17:47 Koolhoven
* allow in JoinWizard::jkDetermine() that two tables with both domain
* None can join columns
* 
* 26    11-10-00 19:12 Koolhoven
* Aggregation Page of JoinWizard has now correct title
* 
* 25    25-07-00 16:37 Koolhoven
* JoinWizard destructor now deletes pages (huge memory leak)
* 
* 24    25-07-00 16:28 Koolhoven
* show info in TableWizard in two sperate groups, for better readability
* 
* 23    25-07-00 15:55 Koolhoven
* enable/disable radiobuttons on second page instead of show/hide
* 
* 22    25-07-00 14:09 Koolhoven
* removed some superfluous space
* no real changes
* 
* 21    7/12/00 2:42p Wind
* adapted JoinWizard
* 
* 20    5/24/00 5:00p Wind
* ConfusionMatrix activated and made available through commandline
* MatrixConfusion(tbl,col1,col2,colfreq)
* 
* 19    5/23/00 5:03p Wind
* bug in JoinWizard when going back and forward several times
* 
* 18    5/22/00 11:55a Wind
* adapted form cumulative to sortby column
* 
* 17    5/17/00 10:22a Wind
* allow more flexibility when both tables have matching domains
* 
* 16    4/18/00 3:51p Wind
* finished JoinWizard
* 
* 15    4/12/00 6:04p Wind
* working on JoinWizard (not yet finished)
* 
* 14    4/04/00 5:02p Wind
* changed text on 'create domain' button in property form of string
* columns
* 
* 13    4/04/00 4:30p Wind
* added cnt function to TblStatForm 
* 
* 12    3/29/00 4:26p Wind
* show hourglass when converting strings to domain
* 
* 11    3/29/00 2:58p Wind
* - show internal domains for column definition
* - apply min/max aggregation only on value columns (in form)
* 
* 10    1-03-00 17:20 Wind
* changed title of form domain (bug 1191)
* 
* 9     28-02-00 12:00 Wind
* adapted to changes in constructos of FieldColumn
* 
* 8     14-02-00 17:18 Wind
* quoted names for tables if necessary
* 
* 7     19-01-00 16:00 Wind
* bug in propform when column did not depend on anything (f.e. %k).
* 
* 6     5-01-00 18:07 Wind
* add extension to domain in propform
* 
* 5     9-09-99 15:15 Koolhoven
* Added ColSemiVarForm
* 
* 4     1-07-99 15:54 Koolhoven
* 
* 3     1-07-99 11:10 Koolhoven
* 
* 2     30-06-99 15:38 Koolhoven
* 
* 1     30-06-99 15:31 Koolhoven
// Revision 1.20  1998/10/15 17:55:47  Wim
// In AggregateForm::ColFuncCallBack() do not allow a bool domain for the
// Std, Avg and Sum functions, only value domains allowed.
//
// Revision 1.19  1998-10-05 09:09:52+01  Wim
// htp in Create Domain (from Column) form
//
// Revision 1.18  1998-10-02 12:07:42+01  Wim
// ColumnPropForm will now display the correct number of decimals
//
// Revision 1.17  1998-09-16 18:34:44+01  Wim
// 22beta2
//
// Revision 1.16  1997/10/08 13:43:05  Wim
// Switched least squares form on
// (help topic)
//
// Revision 1.15  1997-09-24 18:01:26+02  Wim
// Check sDomain empty in callback
//
// Revision 1.14  1997-09-17 15:52:56+02  Wim
// ColumnPropForm::exec() if changing from String always convert
//
// Revision 1.13  1997-09-12 16:32:00+02  Wim
// Replaced calls to tvw->col() with tvw->cv()
//
// Revision 1.12  1997-09-11 16:49:52+02  Wim
// Do not show vr with domain bool in ColumnPropForm
//
// Revision 1.11  1997-09-04 14:54:51+02  Wim
// Removed check if in open columns in BreakLink() n ColumnPropForm
//
// Revision 1.10  1997-09-04 14:32:31+02  Wim
// Check if domain already exists in CreateDomain() in ColumnPropForm
//
// Revision 1.9  1997-09-04 14:08:07+02  Wim
// Added some default column names in LeastSquares Form
//
// Revision 1.8  1997-08-28 15:43:36+02  Wim
// Added some limitations on types of columns to use in Join and Aggregate Forms
//
// Revision 1.7  1997-08-28 11:36:50+02  Wim
// try catch loop in MakeUpToDate() of ColumnPropForm
//
// Revision 1.6  1997-08-20 20:48:13+02  Wim
// Added callback on FieldColumn in AggregateForm
//
// Revision 1.5  1997-08-18 18:50:27+02  Wim
// Only allow value columns in Least Squares Form
//
// Revision 1.4  1997-08-14 20:41:11+02  Wim
// Always ask for number of decimals in column property form
//
// Revision 1.3  1997-08-12 12:40:19+02  Wim
// Removed some commented out old code
//
// Revision 1.2  1997-08-06 18:55:23+02  janh
// added call of AdditionalInfoButton() and AditionalInfo in
// constructor of columnPropForm
//
/* Table Forms
by Wim Koolhoven, october 1996
(c) Computer Department ITC
Last change:  WK   18 May 99    5:31 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Engine\Applications\COLVIRT.H"
//#include "Engine\Table\COLCALC.H"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldaggr.h"
#include "Client\FormElements\fldsmv.h"
#include "Engine\Table\tblview.h"
#include "Engine\Table\tblinfo.h"
#include "Engine\Domain\dmcoord.h"
#include "Client\TableWindow\TableDoc.h"
#include "Engine\Table\tblview.h"
#include "ApplicationsUI\Tblforms.h"
//#include "dsp/propinff.h"
#include "Headers\Hs\JoinWizard.hs"
#include "Headers\Hs\Table.hs"
#include "Headers\Hs\Mainwind.hs"
#include "Headers\Hs\DOMAIN.hs"
#include "Client\FormElements\FieldObjShow.h"
#include "Client\FormElements\fldsmv.h"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\csviall.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Distance.h"
#include "Engine\Base\Algorithm\Lstsqrs.h"
#include "Headers\messages.h"
#include <set>

TableForm::TableForm(CWnd* parent, const String& sTitle) : FormWithDest(parent, sTitle) {

}

TableView *TableForm::getView(CWnd* owner) {
	CFrameWnd *frame = (CFrameWnd *)owner;
	CDocument* doc = frame->GetActiveDocument();
	if (doc) {
		TableDoc *tbldoc = dynamic_cast<TableDoc*>(doc);
		return const_cast<TableView *>(tbldoc->view());
	}
	return NULL;
}

void TableForm::setView(CWnd *parent) {
	CFrameWnd *frame = (CFrameWnd *)parent;
	CDocument* doc = frame->GetActiveDocument();
	if (doc) {
		tbldoc = dynamic_cast<TableDoc*>(doc);
		view =  const_cast<TableView *>(tbldoc->view());
	}
}




LRESULT Cmdclmaggregate(CWnd *parent, const String& dummy)
{
	new AggregateForm(parent);

	return -1;
}

AggregateForm::AggregateForm(CWnd* parent)
: TableForm(parent, STBTitleAggregate)
{
	setView(parent);
	fcol = new FieldColumn(root, STBUiCol, view, &sCol);
	fcol->SetCallBack((NotifyProc)&AggregateForm::FuncCallBack);
	sFunc = 0;
	m_sDefault = String();
	faf = new FieldAggrFunc(root, STBUiFunction, &sFunc, m_sDefault);
	faf->SetCallBack((NotifyProc)&AggregateForm::FuncCallBack);
	fGroupBy = false;
	cbGroup = new CheckBox(root, STBUiGroupBy, &fGroupBy);
	new FieldColumn(cbGroup, "", view, &sGroupCol, dmCLASS|dmIDENT|dmBOOL);
	fWeight = false;
	cbWeight = new CheckBox(root, STBUiWeight, &fWeight);
	cbWeight->Align(cbGroup, AL_UNDER);
	new FieldColumn(cbWeight, "", view, &sWeightCol, dmVALUE);
	fTable = false;
	cbTable = new CheckBox(root, STBUiOutTbl, &fTable);
	cbTable->Align(cbWeight, AL_UNDER);
	new FieldTableCreate(cbTable, "", &sTable);
	FieldString* fs = new FieldString(root, STBUiOutCol, &sOutCol, Domain(), false);
	fs->Align(cbTable, AL_UNDER);
	String sFill('x', 60);
	stRemark = new StaticText(root, sFill);
	stRemark->SetIndependentPos();
	SetMenHelpTopic(htpColAggregate);
	create();
}

int AggregateForm::ColFuncCallBack(Event*)
{
	fcol->StoreData();
	Column col = view->col(sCol);
	bool fOk = false;
	if (col.fValid()) {
		Domain dm = col->dm();
		if (*sFunc == "Std" || *sFunc == "Avg" || *sFunc == "Sum" || *sFunc == "Min" || *sFunc == "Max") {
			if (dm->pdv() && !dm->pdbool())
				fOk = true;
			else
				stRemark->SetVal(SMSErrColNeedsDomainValue);    
		}
		else if (*sFunc == "Med") {
			if (dm->pdv() || dm->pdsrt())
				fOk = true;
			else
				stRemark->SetVal(SMSErrColNeedsSortableDomain);    
		}    
		else
			fOk = true;
	}
	if (fOk) {
		EnableOK();
		stRemark->SetVal("");
	}  
	else {
		DisableOK();
	}  
	return 0;
}

int AggregateForm::FuncCallBack(Event*) 
{
	faf->StoreData();
	if (0 == sFunc || -1 == (long)sFunc) {
		stRemark->SetVal("");
		DisableOK();
		return 0;
	}
	if (*sFunc == "Std" || *sFunc == "Avg" ||
		*sFunc == "Prd" || *sFunc == "Med") 
	{
		cbWeight->Show();  
	}
	else {
		fWeight = false;  
		cbWeight->Hide();  
	}  
	return ColFuncCallBack(0);
}

int AggregateForm::exec() {
	FormWithDest::exec();
		String sExpr;
		String sAgg = sCol.sQuote();
		String sEmpty = "";
		String sGroup = fGroupBy ? sGroupCol.sQuote() : sEmpty;
		String sWeight = "1";
		if (fWeight) 
			sWeight = sWeightCol.sQuote();

		Table tbl = tbldoc->table();
		if (fTable)
			sExpr = String("ColumnJoin%S(%S%S,%S,%S,%S)", 
			*sFunc, tbl->fnObj.sFile.sQuote(), tbl->fnObj.sExt,
			sAgg, sGroup, sWeight);
		else
			sExpr = String("ColumnAggregate%S(%S,%S,%S)", 
			*sFunc, sAgg, sGroup, sWeight);  
		Table tblOut;
		if (fTable) {
			FileName fn(sTable, ".tbt", false);
			if (!File::fExist(fn)) {
				if (fGroupBy) {
					Column col(tbl,sGroupCol.sQuote());
					tblOut = Table(fn, col->dm());
				}
				else {
					tblOut = Table(fn, tbl->dm());
					if (tblOut->dm()->pdnone())
						tblOut->iRecNew(tbldoc->iRows());
				}        
			}
			else
				tblOut = Table(fn);
			sOutCol = String("%S.%S", fn.sFullNameQuoted(false), sOutCol.sQuote());
		}  
		else
			sOutCol = sOutCol.sQuote();
		String sCmd("%S = %S", sOutCol, sExpr);
		char* str = sCmd.sVal();
		GetOwner()->SendMessage(ILWM_EXECUTE, 1, (LPARAM)str);
	
		return 1;
}

LRESULT Cmdclmconfusionmatrix(CWnd *parent, const String& dummy)
{
	new ConfusionMatrixForm(parent);

	return -1;
}

ConfusionMatrixForm::ConfusionMatrixForm(CWnd* parent)
: TableForm(parent, STBTitleConfusionMatrix)
{
	setView(parent);
	if (view->iCols() > 0)
		sCol1 = view->cv(0)->sName();
	if (view->iCols() > 1)
		sCol2 = view->cv(1)->sName();
	if (view->iCols() > 2)
		sColFreq = view->cv(2)->sName();
	new FieldColumn(root, STBUiFirstColumn, view, &sCol1, dmCLASS|dmIDENT|dmGROUP);
	new FieldColumn(root, STBUiSecondColumn, view, &sCol2, dmCLASS|dmIDENT|dmGROUP);
	new FieldColumn(root, STBUiFrequencyColumn, view, &sColFreq, dmVALUE);
	SetMenHelpTopic(htpConfusionMatrix);
	create();
}

int ConfusionMatrixForm::exec() {
		FormWithDest::exec();
	String sCmd("show MatrixConfusion(%S,%S,%S,%S)", view->fnObj.sFullNameQuoted(), sCol1.sQuote(), sCol2.sQuote(), sColFreq.sQuote());
	IlwWinApp()->Execute(sCmd);

	return 1;
}

LRESULT Cmdclmcumulative(CWnd *parent, const String& dummy)
{
	new CumulativeForm(parent);

	return -1;
}

CumulativeForm::CumulativeForm(CWnd* parent)
: TableForm(parent, STBTitleCumulative)
{

	setView(parent);
	fSortBy = false;
	new FieldColumn(root, STBUiCol, view, &sCol, dmVALUE);
	CheckBox* cb = new CheckBox(root, STBUISortByCol, &fSortBy);
	new FieldColumn(cb, "", view, &sColSortBy, dmCLASS|dmIDENT|dmIMAGE|dmBOOL|dmBIT|dmVALUE|dmGROUP);
	FieldString* fs = new FieldString(root, STBUiOutCol, &sOutCol, Domain(), false);
	fs->Align(cb, AL_UNDER);

	SetMenHelpTopic(htpColCumulative);
	create();
}

int CumulativeForm::exec() {
	FormWithDest::exec();
	String sExpr;
	if (fSortBy)
		sExpr = String("ColumnCumulative(%S,%S)", sCol.sQuote(),sColSortBy.sQuote());
	else
		sExpr = String("ColumnCumulative(%S)", sCol.sQuote());
	String sCmd("%S = %S", sOutCol.sQuote(), sExpr);
	char* str = sCmd.sVal();
	GetOwner()->SendMessage(ILWM_EXECUTE, 1, (LPARAM)str);

	return 1;
}

LRESULT Cmdclmleastsquares(CWnd *parent, const String& dummy)
{
	new LeastSquaresForm(parent);

	return -1;
}


LeastSquaresForm::LeastSquaresForm(CWnd* parent)
: TableForm(parent, STBTitleLeastSquares)
{
	setView(parent);
	if (view->iCols() > 0)
		sColX = view->cv(0)->sName();
	if (view->iCols() > 1)
		sColY = view->cv(1)->sName();
	new FieldColumn(root, STBUiColX, view, &sColX, dmVALUE|dmIMAGE);
	new FieldColumn(root, STBUiColY, view, &sColY, dmVALUE|dmIMAGE);
	sFunc = 0;
	m_sDefault = String();
	frf = new FieldRegressionFunc(root, STBUiFunction, &sFunc, m_sDefault);
	frf->SetCallBack((NotifyProc)&LeastSquaresForm::FuncCallBack);
	fiTerms = new FieldInt(root, STBUiNrTerms, &iTerms, RangeInt(2,100));
	FieldString* fs = new FieldString(root, STBUiOutCol, &sOutCol, Domain(), false);
	SetMenHelpTopic(htpColLeastSquares);
	create();
}

int LeastSquaresForm::exec() {
	FormWithDest::exec();
	if (*sFunc == String("trigonometric") && iTerms < 3)
		iTerms = 3;
	String sExpr("ColumnLeastSquaresFit(%S,%S,%S,%i)", 
		sColX.sQuote(), sColY.sQuote(), *sFunc, iTerms);
	String sCmd("%S = %S", sOutCol.sQuote(), sExpr);
	char* str = sCmd.sVal();
	GetOwner()->SendMessage(ILWM_EXECUTE, 1, (LPARAM)str);

	return 1;
    
}

int LeastSquaresForm::FuncCallBack(Event*) 
{
	frf->StoreData();
	if (0 == sFunc || -1 == (long)sFunc)
		return 0;
	if (*sFunc == "polynomial") {
		fiTerms->SetVal(2);
		fiTerms->Show();
	}  
	else if (*sFunc == "trigonometric") {
		fiTerms->SetVal(3);
		fiTerms->Show();
	}  
	else {
		iTerms = 2;
		fiTerms->Hide();
	}
	return 0;
}

const int iTSFUNCS = 12;
class FieldTblStatsFuncSimple;

class FieldTblStatsFunc: public FieldGroup
{
public:
	FieldTblStatsFunc(FormEntry* parent, const String& sQuestion, String** sFunc);
	virtual void SetCallBack(NotifyProc np);
private:
	FieldTblStatsFuncSimple* fsfs;
};

class FieldTblStatsFuncSimple: public FieldOneSelect
{
public:
	FieldTblStatsFuncSimple(FormEntry* par, String** sFunc)
		: FieldOneSelect(par, (long*)sFunc, false)
	{
		SetWidth(75);
		int i = 0;
		ts[i].sFun = "Avg";
		ts[i++].sName = STBFunStatAvg;
		ts[i].sFun = "Min";
		ts[i++].sName = STBFunStatMin;
		ts[i].sFun = "Max";
		ts[i++].sName = STBFunStatMax;
		ts[i].sFun = "Sum";
		ts[i++].sName = STBFunStatSum;
		ts[i].sFun = "Cnt";
		ts[i++].sName = STBFunStatCnt;
		ts[i].sFun = "StDev";
		ts[i++].sName = STBFunStatStDev;
		ts[i].sFun = "Corr";
		ts[i++].sName = STBFunStatCorr;
		ts[i].sFun = "Var";
		ts[i++].sName = STBFunStatVar;
		ts[i].sFun = "Covar";
		ts[i++].sName = STBFunStatCovar;
		ts[i].sFun = "StdErr";
		ts[i++].sName = STBFunStatStdErr;
		ts[i].sFun = "ttest";
		ts[i++].sName = STBFunStatTtest;
		ts[i].sFun = "ChiSquare";
		ts[i++].sName = STBFunStatChiSquare;
	}
	void create() {
		FieldOneSelect::create();
		for (int i = 0; i < iTSFUNCS; ++i) {
			int id = ose->AddString(ts[i].sName.sVal());
			SendMessage(*ose,CB_SETITEMDATA,id,(long)&ts[i].sFun);
		}
		ose->SetCurSel(0);
	}
private:
	struct TblStatsStruct {
		String sFun;
		String sName;
	};
	TblStatsStruct ts[iTSFUNCS];
};

FieldTblStatsFunc::FieldTblStatsFunc(FormEntry* parent, const String& sQuestion, String** sFunc)
: FieldGroup(parent)
{
	if (sQuestion.length() != 0)
		new StaticTextSimple(this, sQuestion);
	fsfs = new FieldTblStatsFuncSimple(this, sFunc);
	if (children.iSize() > 1) // also static text
		children[1]->Align(children[0], AL_AFTER);
}

void FieldTblStatsFunc::SetCallBack(NotifyProc np) 
{
	fsfs->SetCallBack(np);
}

LRESULT Cmdclmstatistiscs(CWnd *parent, const String& dummy)
{
	new TblStatsForm(parent);

	return -1;
}

TblStatsForm::TblStatsForm(CWnd* parent)
: TableForm(parent, STBTitleStatistics)
{
	view = getView(parent);
	sFunc = 0;
	fsf = new FieldTblStatsFunc(root, STBUiFunction, &sFunc);
	fsf->SetCallBack((NotifyProc)&TblStatsForm::FuncCallBack);
	int iFirstValCol = iUNDEF;
	int i=0;
	for (; i < view->iCols(); i++) 
		if (view->cv(i)->fValues()) {
			iFirstValCol = i;
			break;
		}
		if (iFirstValCol >= 0) 
			sCol1 = view->cv(iFirstValCol)->sName();
		int iSecondValCol = iUNDEF;
		for (i++; i < view->iCols(); i++) 
			if (view->cv(i)->fValues()) {
				iSecondValCol = i;
				break;
			}
			if (iSecondValCol >= 1)
				sCol2 = view->cv(iSecondValCol)->sName();
			rVal = 0;
			fc1 = new FieldColumn(root, STBUiCol, view, &sCol1, dmVALUE|dmIMAGE);
			fc2 = new FieldColumn(root, STBUiCol2, view, &sCol2, dmVALUE|dmIMAGE);
			fcExp = new FieldColumn(root, STBUiExpected, view, &sCol2, dmVALUE|dmIMAGE);
			fcExp->Align(fc1, AL_UNDER);
			fr = new FieldReal(root, STBUiTrueValue, &rVal);
			fr->Align(fc1, AL_UNDER);
			SetMenHelpTopic(htpTblStatistics);
			create();
}

int TblStatsForm::FuncCallBack(Event*)
{
	fsf->StoreData();
	if (0 == sFunc || -1 == (long)sFunc)
		return 0;
	String str = sFunc->toLower();
	if (str == "min" ||
		str == "max" ||
		str == "avg" ||
		str == "var" ||
		str == "sum" ||
		str == "cnt" ||
		str == "stdev" ||
		str == "stderr")
	{
		fc2->Hide();
		fcExp->Hide();
		fr->Hide();
	}
	else if (*sFunc == "ttest") {
		fc2->Hide();
		fcExp->Hide();
		fr->Show();
	}
	else if (*sFunc == "chisquare") {
		fc2->Hide();
		fcExp->Show();
		fr->Hide();
	}
	else {
		fc2->Show();
		fcExp->Hide();
		fr->Hide();
	}
	return 0;  
}

int TblStatsForm::exec()
{
	FormWithDest::exec();
	if (0 == sFunc || -1 == (long)sFunc)
		return 0;
	String str = sFunc->toLower();
	if (str == "min" ||
		str == "max" ||
		str == "avg" ||
		str == "sum" ||
		str == "cnt" ||
		str == "var" ||
		str == "stdev" ||
		str == "stderr")
		sExpr = String("? %S(%S)", *sFunc, sCol1);
	else if (str == "ttest")
		sExpr = String("? %S(%f,%S)", *sFunc, rVal, sCol1);
	else
		sExpr = String("? %S(%S,%S)", *sFunc, sCol1, sCol2);

	const char* sm = sExpr.scVal();

	GetOwner()->SendMessage(ILWM_EXECUTE, 1, (LPARAM)sm);
	return 0;
}

LRESULT Cmdclmsort(CWnd *parent, const String& s)
{
	CFrameWnd *frame = (CFrameWnd *)parent;
	CDocument* doc = frame->GetActiveDocument();
	TableDoc *tbldoc = dynamic_cast<TableDoc*>(doc);
	TableView *tvw = const_cast<TableView *>(tbldoc->view());

	ParmList pm(s);
	String sCol = pm.sGet(0).sUnQuote();
	TableView::SortOrder so;
	if (pm.fExist("sortorder"))
		so = fCIStrEqual(pm.sGet("sortorder"), "descending")  ? TableView::soDESCENDING : TableView::soASCENDING;
	else 
		so = TableView::soASCENDING; // default
	ColumnView cv;
	if (sCol.length() > 0) { //  take this column
		cv = tvw->cv(sCol);
		if (cv.fValid()) {
			int iKey = tvw->iCol(sCol);
			tvw->SetSortOrder(so); // SetKey does the sorting
			CWaitCursor wc;
			tvw->SetKey(iKey);  
			tbldoc->UpdateAllViews(0);
			return -1;
		}
	}
	else {
		int iSort = 0;
		cv = tvw->cvKey(); // previous sort column
		if (cv.fValid()) {
			iSort = 1;
			sCol = cv->sName();
			so = tvw->so; // previous sort order
		}
		ColSortOnForm frm(parent, &iSort, &sCol, (int*)&so);
		if (frm.fOkClicked()) {
			int iKey = -1;
			if (iSort)
				iKey = tvw->iCol(sCol);
			tvw->SetSortOrder(so); // SetKey does the sorting
			CWaitCursor wc;
			tvw->SetKey(iKey);  
			tbldoc->UpdateAllViews(0);
		}
	}

	return -1;
}

ColSortOnForm::ColSortOnForm(CWnd* parent, int* iSort, String* sCol, int* iSortOrder) : TableForm(parent, STBTitleSort)
{
	TableView *tvw = getView(parent);
	      new FieldBlank(root);
      RadioGroup* rg = new RadioGroup(root, STBUiRecordSequence, iSort);
      if (tvw->dm()->pdsrt())
        new RadioButton(rg, STBUiDomain);
      else  
        new RadioButton(rg, STBUiRawData);
      RadioButton* rb = new RadioButton(rg, STBUiCol);
      new FieldColumn(rb, "", tvw, sCol);
      RadioGroup* rgsort = new RadioGroup(rb, STBUISortOrder, iSortOrder);
      rgsort->Align(rb, AL_UNDER);
      new RadioButton(rgsort, STBUIAscending);
      new RadioButton(rgsort, STBUIDescending);
      SetMenHelpTopic(htpTblSort);
      create();
}

int ColSortOnForm::exec() {
	FormWithDest::exec();
		return 1;
}

LRESULT Cmdclmslicing(CWnd *parent, const String& dummy)
{
	new ColSlicingForm(parent);

	return -1;
}

ColSlicingForm::ColSlicingForm(CWnd *parent) :TableForm(parent, STBTitleSort)
{
	view = getView(parent);
	new FieldBlank(root);
	new FieldColumn(root, STBUiColName, Table(view->fnTbl()), &sInpCol, dmVALUE|dmIMAGE);
	new FieldBlank(root);

	new FieldDomainC(root, STBUiDomain, &sDomain, dmGROUP);
	new FieldBlank(root);
	new FieldString(root, STBUiOutCol, &sOutCol, Domain(), false);
	new FieldBlank(root);

	SetMenHelpTopic(htpColSlicing);
	create();
}

int ColSlicingForm::exec() {
	FormWithDest::exec();
	String sCmd("%S=clfy(%S,%S)", sOutCol, sInpCol, sDomain);
   	char* str = sCmd.sVal();
	GetOwner()->SendMessage(ILWM_EXECUTE, 1, (LPARAM)str);

	return 1;
}

LRESULT Cmdclmsemivar(CWnd *parent, const String& dummy)
{
	new ColSemiVarForm(parent);

	return -1;
}

ColSemiVarForm::ColSemiVarForm(CWnd* parent)
: TableForm(parent, STBTitleSemiVariogram)
{
	view = getView(parent);
	if (view->iCols() > 0)
		sColDist = view->cv(0)->sName();
	new FieldColumn(root, STBUiDistCol, view, &sColDist, dmVALUE);
	new FieldSemiVariogram(root, STBUiSemiVar, &smv);
	new FieldString(root, STBUiOutCol, &sOutCol, Domain(), false);
	SetMenHelpTopic(htpColSemiVar);
	create();
}

int ColSemiVarForm::exec() {
  	FormWithDest::exec();
	  String sSmv = smv.sExpression();
      String sExpr("ColumnSemiVariogram(%S,%S)", sColDist.sQuote(), sSmv);
      String sCmd("%S = %S", sOutCol.sQuote(), sExpr);
	  char* str = sCmd.sVal();
	  GetOwner()->SendMessage(ILWM_EXECUTE, 1, (LPARAM)str);

   	return 1;
}

// Join Wizard classes

LRESULT Cmdjoin(CWnd *parent, const String& s)
{
	TableView *tvw = TableForm::getView(parent);
    JoinWizard frm(parent);
    if (frm.DoModal() == ID_WIZFINISH ) {
      FileName fnt(frm.sTbl);
      String sTbl = fnt.sRelativeQuoted(true, tvw->fnObj.sPath());
      String sExpr;
      String sApplicName = "ColumnJoin";
      Column colKey = tvw->cv(frm.sKeyCol);
      if (!frm.fKeyCol && !frm.fViaKey) {
        sExpr = String("%S(%S,%S)", sApplicName, 
                           sTbl, frm.sCol.sQuote());
      }
      else if (frm.fKeyCol && !frm.fViaKey && colKey->dm() == frm.tblinf.dm()) {
        sExpr = String("%S(%S,%S,%S)", sApplicName, 
                       sTbl, frm.sCol.sQuote(), frm.sKeyCol.sQuote());
      }
      else if (!frm.fAggregate) {
        if (frm.fKeyCol)
          sExpr = String("%S2ndKey(%S,%S,%S,%S)", sApplicName, 
                         sTbl, frm.sCol.sQuote(), frm.sViaKey.sQuote(), frm.sKeyCol.sQuote());
        else  
          sExpr = String("%S2ndKey(%S,%S,%S)", sApplicName, 
                         sTbl, frm.sCol.sQuote(), frm.sViaKey.sQuote());
      }
      else {
        if (!frm.fWeight)
          frm.sWeightCol = "1";
				else 
				  frm.sWeightCol = frm.sWeightCol.sQuote();
        if (frm.fKeyCol)
          sExpr = String("%S%S(%S,%S,%S,%S,%S)", sApplicName, *frm.sFunc, 
                         sTbl, frm.sCol.sQuote(), frm.sViaKey.sQuote(), frm.sWeightCol, frm.sKeyCol.sQuote());
        else  
          sExpr = String("%S%S(%S,%S,%S,%S)", sApplicName, *frm.sFunc, 
                         sTbl, frm.sCol.sQuote(), frm.sViaKey.sQuote(), frm.sWeightCol);
      }
      String sCmd("%S = %S", frm.sOutCol.sQuote(), sExpr);
			char* str = sCmd.sVal();
			parent->SendMessage(ILWM_EXECUTE, 1, (LPARAM)str);
    }

	return 1;
}

class JoinInputColumnPP : public FormBaseWizardPage
{
public:
	JoinInputColumnPP(JoinWizard* jnwz) : FormBaseWizardPage(SJWTitleInputCol) 
	{
		jw = jnwz;

		FormEntry* root = feRoot();
		(new StaticText(root, SJWIntroInputCol, true))->SetIndependentPos();
		new FieldBlank(root);
		jw->ftbl = new FieldDataType(root, STBUiTbl, &jw->sTbl, 
			new TableAsLister(".TBT.HIS.HSA.HSS.HSP.RPR.MPP"), true);
		jw->ftbl->SetCallBack((NotifyProc)&JoinWizard::TblCallBack, jw);
		jw->fcCol = new FieldColumn(root, STBUiCol, Table(), &jw->sCol);
		jw->fcCol->SetCallBack((NotifyProc)&JoinWizard::ColCallBack, jw);
		jw->stNone1 = new StaticText(root,SJWRemNone1);
		jw->stNone1->SetIndependentPos();
		jw->stNone2 = new StaticText(root, SJWRemNone2);
		jw->stNone2->SetIndependentPos();
	}
	virtual BOOL OnSetActive() 
	{
		if ("" != jw->sCol) 
			jw->SetWizardButtons(PSWIZB_NEXT);
		else
			jw->SetWizardButtons(0);
		return FormBasePropertyPage::OnSetActive();
	}
	JoinWizard* jw;
};

class JoinJoinTypePP : public FormBaseWizardPage
{
public:
	JoinJoinTypePP(JoinWizard* jnwz) : FormBaseWizardPage(SJWTitleJoinMethod) 
	{
		jw = jnwz;

		FormEntry* root = feRoot();
		jw->iJoinType = 1;
		(new StaticText(root, SJWIntroJoinMethod1, true))->SetIndependentPos();
		(new StaticText(root, SJWIntroJoinMethod2, true))->SetIndependentPos();
		new FieldBlank(root);
		jw->rgJoinType = new RadioGroup(root, "", &jw->iJoinType);
		jw->rgJoinType->SetCallBack((NotifyProc)&JoinWizard::JoinTypeCallBack, jw);
		jw->rbTblTbl = new RadioButton(jw->rgJoinType, SJWJoinType1);
		jw->rbColTbl = new RadioButton(jw->rgJoinType, SJWJoinType2);
		jw->rbTblCol = new RadioButton(jw->rgJoinType, SJWJoinType3);
		jw->rbColCol = new RadioButton(jw->rgJoinType, SJWJoinType4);
		new FieldBlank(root);
		String sDummy('X', 20);

		FieldGroup* fg1 = new FieldGroup(root);
		fg1->SetIndependentPos();
		fg1->SetBevelStyle(FormEntry::bsLOWERED);
		StaticText* st1 = new StaticText(fg1, SJWCurrentTable);  
		StaticText* st2 = new StaticText(fg1, jw->view->sName(jw->view->fnObj.sExt != ".tbt"));
		st2->Align(st1, AL_AFTER);
		StaticText* st5 = new StaticText(fg1, "");  // dummy
		st5->Align(st1, AL_UNDER);
		StaticText* st9 = new StaticText(fg1, SJWKey1);  
		st9->Align(st5, AL_UNDER);
		StaticText* st10 = new StaticText(fg1, sDummy);  
		st10->Align(st9, AL_AFTER);

		FieldGroup* fg2 = new FieldGroup(root);
		fg2->SetIndependentPos();
		fg2->SetBevelStyle(FormEntry::bsLOWERED);
		fg2->Align(fg1, AL_AFTER);
		StaticText* st3 = new StaticText(fg2, SJWSelectedTable);  
		StaticText* st4 = new StaticText(fg2, sDummy);  
		st4->Align(st3, AL_AFTER);
		StaticText* st7 = new StaticText(fg2, SJWSelectedJoinCol);
		st7->Align(st3, AL_UNDER);
		StaticText* st8 = new StaticText(fg2, sDummy);  
		st8->Align(st7, AL_AFTER);
		StaticText* st11 = new StaticText(fg2, SJWKey2);  
		st11->Align(st7, AL_UNDER);
		StaticText* st12 = new StaticText(fg2, sDummy);  
		st12->Align(st11, AL_AFTER);

		jw->stSelectedTable = st4;
		jw->stSelectedColumn = st8;
		jw->stKey1 = st10;
		jw->stKey2 = st12;
	}
	virtual BOOL OnSetActive() 
	{
		jw->fAggregate = false;
		jw->stSelectedTable->SetVal(jw->tblinf.fnTbl().sRelative(jw->tblinf.fnTbl().sExt != ".tbt", jw->view->fnObj.sPath()));
		jw->stSelectedColumn->SetVal(jw->sCol);
		jw->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		jw->rgJoinType->SetVal(jw->iJoinType);
		jw->JoinTypeCallBack(0);
		return FormBaseWizardPage::OnSetActive();
	}
	JoinWizard* jw;
};

class JoinKeysPP : public FormBaseWizardPage
{
public:
	JoinKeysPP(JoinWizard* jnwz) : FormBaseWizardPage(SJWTitleLink) 
	{
		jw = jnwz;

		FormEntry* root = feRoot();
		(new StaticText(root, SJWIntroJoinMethod1, true))->SetIndependentPos();

		asIntro1.push_back(SJWIntroLink1A);
		asIntro2.push_back(SJWIntroLink1B);
		asIntro1.push_back(SJWIntroLink2A);
		asIntro2.push_back(SJWIntroLink2B);
		asIntro1.push_back(SJWIntroLink3A);
		asIntro2.push_back(SJWIntroLink3B);
		// determine longest string
		String s;
		for (int i=0; i < 3; i++) {
			if (asIntro1[i].length() > s.length())
				s = asIntro1[i];
			if (asIntro2[i].length() > s.length())
				s = asIntro2[i];
		}
		stIntro1 = new StaticText(root, s, true);
		stIntro1->SetIndependentPos();
		stIntro2 = new StaticText(root, s, true);
		stIntro2->SetIndependentPos();
		new FieldBlank(root);
		String sDummy('X', 20);

		FieldGroup* fg1 = new FieldGroup(root);
		fg1->SetIndependentPos();
		fg1->SetBevelStyle(FormEntry::bsLOWERED);
		StaticText* st1 = new StaticText(fg1, SJWCurrentTable);  
		StaticText* st2 = new StaticText(fg1, jw->view->sName(jw->view->fnObj.sExt != ".tbt"));
		st2->Align(st1, AL_AFTER);
		StaticText* st5 = new StaticText(fg1, "");  // dummy
		st5->Align(st1, AL_UNDER);
		StaticText* st9 = new StaticText(fg1, SJWKey1);  
		st9->Align(st5, AL_UNDER);
		st10 = new StaticText(fg1, sDummy);  
		st10->Align(st9, AL_AFTER);
		jw->fcKeyCol = new FieldColumn(fg1, "", jw->view, &jw->sKeyCol, dmCLASS|dmIDENT|dmIMAGE|dmBOOL|dmBIT);
		jw->fcKeyCol->SetCallBack((NotifyProc)&JoinWizard::KeyColCallBack, jw);
		jw->fcKeyCol->Align(st9, AL_AFTER);

		FieldGroup* fg2 = new FieldGroup(root);
		fg2->SetIndependentPos();
		fg2->SetBevelStyle(FormEntry::bsLOWERED);
		fg2->Align(fg1, AL_AFTER);
		StaticText* st3 = new StaticText(fg2, SJWSelectedTable);  
		st4 = new StaticText(fg2, sDummy);  
		st4->Align(st3, AL_AFTER);
		StaticText* st7 = new StaticText(fg2, SJWSelectedJoinCol);  
		st7->Align(st3, AL_UNDER);
		st8 = new StaticText(fg2, sDummy);  
		st8->Align(st7, AL_AFTER);
		StaticText* st11 = new StaticText(fg2, SJWKey2);  
		st11->Align(st7, AL_UNDER);
		st12 = new StaticText(fg2, sDummy);  
		st12->Align(st11, AL_AFTER);
		jw->fcViaKey = new FieldColumn(fg2, "", Table(), &jw->sViaKey, dmCLASS|dmIDENT|dmIMAGE|dmBOOL|dmBIT);
		jw->fcViaKey->SetCallBack((NotifyProc)&JoinWizard::ViaKeyCallBack, jw);
		jw->fcViaKey->Align(st11, AL_AFTER);

		FieldBlank* fb = new FieldBlank(root);
		fb->Align(st9, AL_UNDER);

		jw->stAggr1 = new StaticText(root, SJWRemAggr1);
		jw->stAggr1->SetIndependentPos();
		s = String(SJWRemAggr2_SS.scVal(), String('X', 15), String('X', 15));
		jw->stAggr2 = new StaticText(root, s);
		jw->stAggr2->SetIndependentPos();
	}
	virtual BOOL OnSetActive() 
	{
		stIntro1->SetVal(asIntro1[jw->iJoinType-1]);
		stIntro2->SetVal(asIntro2[jw->iJoinType-1]);
		st4->SetVal(jw->tblinf.fnTbl().sRelative(jw->tblinf.fnTbl().sExt != ".tbt", jw->view->fnObj.sPath()));
		st8->SetVal(jw->sCol);
		jw->SetWizardButtons(PSWIZB_BACK);
		jw->stAggr1->Hide();
		jw->stAggr2->Hide();
		bool f = FormBaseWizardPage::OnSetActive() != 0;
		switch (jw->iJoinType) {
case 0 :
	assert(0==1);// can't get here
	break;
case 1 : 
	st10->Hide();
	jw->fcKeyCol->FillWithColumns(jw->view->fnTbl(), jw->tblinf.dm());
	jw->fcKeyCol->Show();
	jw->fcViaKey->Hide();
	st12->SetVal(String(SJWTableDomain_S.scVal(), jw->tblinf.dm()->sName()));
	st12->Show();
	jw->KeyColCallBack(0);
	break;
case 2 : 
	st12->Hide();
	jw->fcViaKey->FillWithColumns(jw->tblinf.fnTbl(), jw->view->dm());
	jw->fcKeyCol->Hide();
	jw->fcViaKey->Show();
	st10->SetVal(String(SJWTableDomain_S.scVal(), jw->view->dm()->sName()));
	st10->Show();
	jw->ViaKeyCallBack(0);
	break;
case 3 : 
	st10->Hide();
	st12->Hide();
	jw->fcViaKey->FillWithColumns(jw->tblinf.fnTbl(), jw->view->dm());
	jw->fcKeyCol->FillWithColumns(jw->view->fnTbl());
	jw->fcKeyCol->Show();
	jw->fcViaKey->Show();
	jw->KeyColCallBack(0);
	jw->ViaKeyCallBack(0);
		}
		return f;
	}
	JoinWizard* jw;
	StaticText *st4, *st8, *st10, *st12;
	StaticText *stIntro1, *stIntro2;
	vector<String> asIntro1, asIntro2;
};


class JoinAggrPP : public FormBaseWizardPage
{
public:
	JoinAggrPP(JoinWizard* jnwz) : FormBaseWizardPage(SJWTitleAggrMethod) 
	{
		jw = jnwz;

		FormEntry* root = feRoot();
		(new StaticText(root, SJWRemAggr1, true))->SetIndependentPos();
		String sFill('X', 20);
		String s(SJWRemAggr2_SS.scVal(), sFill, sFill);
		stGroupBy = new StaticText(root, s, true);
		stGroupBy->SetIndependentPos();
		new FieldBlank(root);
		jw->sFunc = 0;
		m_sDefault = String();
		jw->faf = new FieldAggrFunc(root, STBUiFunction, &jw->sFunc, m_sDefault);
		jw->faf->SetCallBack((NotifyProc)&JoinAggrPP::FuncCallBack, this);
		FieldBlank* fb = new FieldBlank(root);
		stWeight = new StaticText(root,SJWRemUseWeight);
		stWeight->SetIndependentPos();
		stWeight->Align(fb, AL_UNDER);
		jw->fWeight = false;
		cbWeight = new CheckBox(root, STBUiWeight, &jw->fWeight);
		fcWeight = new FieldColumn(cbWeight, "", Table(), &jw->sWeightCol, dmVALUE);
	}
	virtual BOOL OnSetActive() 
	{
		jw->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		bool f = FormBaseWizardPage::OnSetActive() != 0;
		String s(SJWRemAggr2_SS.scVal(), jw->sCol, jw->sViaKey);
		stGroupBy->SetVal(s);
		fcWeight->FillWithColumns(jw->tblinf.fnTbl());
		FuncCallBack(0);
		return f;
	}
	int FuncCallBack(Event*) 
	{
		jw->faf->StoreData();
		if (0 == jw->sFunc || -1 == (long)jw->sFunc)
			return 0;
		String sFnc = *(jw->sFunc);
		if (sFnc == "Std" || sFnc == "Avg" ||
			sFnc == "Prd" || sFnc == "Med") 
		{
			stWeight->Show();
			cbWeight->Show();
			jw->fWeight = true;
		}
		else {
			jw->fWeight = false;
			stWeight->Hide();
			cbWeight->Hide();
		}
		return 0;
	}
	JoinWizard* jw;
	StaticText *stWeight, *stGroupBy;
	FieldColumn *fcWeight;
	CheckBox* cbWeight;
	String m_sDefault;
};

class JoinOutputColumnPP : public FormBaseWizardPage
{
public:
	JoinOutputColumnPP(JoinWizard* jnwz) : FormBaseWizardPage(SJWTitleOutputCol) 
	{
		jw = jnwz;

		FormEntry* root = feRoot();
		(new StaticText(root, SJWIntroOutputCol, true))->SetIndependentPos();
		new FieldBlank(root);
		jw->fsOutCol = new FieldString(root, STBUiOutCol, &jw->sOutCol, Domain(), false);
		jw->fsOutCol->SetCallBack((NotifyProc)&JoinWizard::OutColCallBack, jw);
		jw->stAlreadyExists = new StaticText(root, SJWRemColExists );
	}
	virtual BOOL OnSetActive() 
	{
		jw->SetWizardButtons(PSWIZB_BACK);
		jw->fsOutCol->SetVal(jw->sCol);  
		jw->fsOutCol->StoreData();
		jw->stAlreadyExists->Hide();
		if (jw->sOutCol.length() != 0) {
			// check if column name already exists
			Column col = jw->view->col(jw->sOutCol);
			if (col.fValid())
				jw->stAlreadyExists->Show();
			else
				jw->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
		}
		return FormBaseWizardPage::OnSetActive();
	}
	JoinWizard* jw;
};

JoinWizard::JoinWizard(CWnd* wnd) 
: CPropertySheet("Join Wizard", wnd), view(TableForm::getView(wnd)), sFunc(0), 
fKeyCol(false), fViaKey(false), fAggregate(false), fWeight(false)
{
	SetWizardMode();

	// input column prop
	ppInputColumn = new JoinInputColumnPP(this);
	ppInputColumn->create();
	ppInputColumn->SetMenHelpTopic(htpJoinWizardInput);
	AddPage(ppInputColumn);

	// input select type of join
	ppJoinType = new JoinJoinTypePP(this);
	ppJoinType->create();
	ppJoinType->SetMenHelpTopic(htpJoinWizardType);
	AddPage(ppJoinType);

	ppKeys = new JoinKeysPP(this);
	ppKeys->create();
	ppKeys->SetMenHelpTopic(htpJoinWizardKeys);
	AddPage(ppKeys);

	ppAggr = new JoinAggrPP(this);
	ppAggr->create();
	AddPage(ppAggr);
	ppAggr->SetMenHelpTopic(htpJoinWizardAggr);
	ppAggr->SetPPDisable();

	// output column prop
	ppOutputColumn = new JoinOutputColumnPP(this);
	ppOutputColumn->create();
	ppOutputColumn->SetMenHelpTopic(htpJoinWizardOutput);
	AddPage(ppOutputColumn);
}

JoinWizard::~JoinWizard()
{
	delete ppInputColumn;
	delete ppJoinType;
	delete ppKeys;
	delete ppAggr;
	delete ppOutputColumn;
}


int JoinWizard::TblCallBack(Event*) 
{
	ftbl->StoreData();
	fcCol->FillWithColumns((TablePtr*)0);
	if (sTbl == "") {
		SetWizardButtons(0);
		stNone1->Hide();
		stNone2->Hide();
		return 0;
	}
	try {
		FileName fnTbl(sTbl, ".tbt", false);
		tblinf = TableInfo(fnTbl);
		jk = jkDetermine();
		if (0 == jk) {
			stNone1->Show();
			stNone2->Show();
			fcCol->Hide();
			SetWizardButtons(0); 
		}
		else {
			stNone1->Hide();
			stNone2->Hide();
			fcCol->Show();
			fcCol->FillWithColumns(fnTbl);
			ColCallBack(0);
		}
	}
	catch (ErrorObject& err) {
		err.Show();
	}
	tblInp = Table();
	return 0;
}

int JoinWizard::ColCallBack(Event*) 
{
	fcCol->StoreData();
	if ("" != sCol) {
		if (jk == jkTblTbl) {
			// disable all pages, go directly to output name
			ppJoinType->SetPPDisable();
			ppKeys->SetPPDisable();
			ppAggr->SetPPDisable();
			fKeyCol = fViaKey = false;
		}
		else {
			// enable all pages
			ppJoinType->SetPPEnable();
			ppKeys->SetPPDisable();
			ppAggr->SetPPEnable();
			if (jkColCol & jk)
				iJoinType = 3;
			if (jkTblCol & jk)
				iJoinType = 2;
			if (jkColTbl & jk)
				iJoinType = 1;
			if (jkTblTbl & jk)
				iJoinType = 0;
		}
		SetWizardButtons(PSWIZB_NEXT);
	}
	else 
		SetWizardButtons(0);
	return 0;
}

int JoinWizard::JoinTypeCallBack(Event*) 
{
	rgJoinType->StoreData();
	// enable only usefull radio buttons, disable the others
	if (jkTblTbl & jk)
		rbTblTbl->Enable();
	else 
		rbTblTbl->Disable();
	if (jkTblCol & jk)
		rbTblCol->Enable();
	else 
		rbTblCol->Disable();
	if (jkColTbl & jk)
		rbColTbl->Enable();
	else
		rbColTbl->Disable();
	if (jkColCol & jk)
		rbColCol->Enable();
	else
		rbColCol->Disable();
	SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	ppAggr->SetPPDisable();
	switch (iJoinType) {
case 0 : 
	// go directly to output column
	ppKeys->SetPPDisable();
	fKeyCol = false;
	fViaKey = false;
	stKey1->SetVal("-");
	stKey2->SetVal("-");
	break;
case 1 : {
	// enable only key column (skip via key and aggregation)
	ppKeys->SetPPEnable();
	fKeyCol = true;
	fViaKey = false;
	stKey1->SetVal(SJWToBeSelected);
	String sTblInpDom(SJWTableDomain_S.scVal(), tblinf.dm()->sName());
	stKey2->SetVal(sTblInpDom);
			}
		 break;
case 2 : {
	// skip key column
	ppKeys->SetPPEnable();
	fKeyCol = false;
	fViaKey = true;
	String sTblDom(SJWTableDomain_S.scVal(), view->dm()->sName());
	stKey1->SetVal(sTblDom);
	stKey2->SetVal(SJWToBeSelected);
			}
		 break;
case 3 : {
	// first ask key column and then via key and aggregation
	ppKeys->SetPPEnable();
	fKeyCol = true;
	fViaKey = true;
	stKey1->SetVal(SJWToBeSelected);
	stKey2->SetVal(SJWToBeSelected);
			}
		 break;
	}
	return 0;
}

int JoinWizard::KeyColCallBack(Event*) 
{
	SetWizardButtons(PSWIZB_BACK);
	fcKeyCol->StoreData();
	iIndexKeyColumn = -1;
	Column col = view->col(sKeyCol);
	if (col.fValid()) {
		for (int i=0; i < view->iCols(); i++) {
			if (view->col(i)->sName() == sKeyCol) {
				iIndexKeyColumn = i;
				break;
			}
		}
		if (iJoinType == 3) { // also a via key (key 2) should be selected
			if (iIndexKeyColumn >= 0) {
				fcViaKey->FillWithColumns(tblinf.fnTbl(), view->col(iIndexKeyColumn)->dm());
				ViaKeyCallBack(0);
			}
		}
		else
			SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	}
	return 0;
}

int JoinWizard::ViaKeyCallBack(Event*) 
{
	stAggr1->Hide();
	stAggr2->Hide();
	ppAggr->SetPPDisable(); 
	SetWizardButtons(PSWIZB_BACK);
	if (fcViaKey->fShow()) {
		fcViaKey->StoreData();
		if (sViaKey.length() > 0 ) {
			try { 
				if (!tblInp.fValid()) {
					CWaitCursor wc;
					tblInp = Table(tblinf.fnTbl());
				}
			}
			catch (const ErrorObject& err) {
				err.Show();
			}
			if (tblInp.fValid()) {
				Column col = tblInp->col(sViaKey);
				fAggregate = !col->fUniqueValues();
				if (fAggregate) {
					// show aggregate/group by page only if not all values are unique 
					// and the input column contains values

					// search for domain of input column
					for (int i=0; i < tblinf.iCols(); i++)
						if (tblinf.aci[i].sName() == sCol) {
							bool fValues = tblinf.aci[i].dminf().fValues();
							ppAggr->SetPPEnable(fValues); 
							if (!fValues) { // use predominant as aggregation function
								if (0 != sFunc)
									delete sFunc;
								sFunc = new String("Prd");
								stAggr1->Show();
								String s(SJWRemAggr3_S.scVal(), sCol);
								stAggr2->SetVal(s);
								stAggr2->Show();
							}
							else {
								stAggr1->Show();
								String s(SJWRemAggr2_SS.scVal(), sCol, sViaKey);
								stAggr2->SetVal(s);
								stAggr2->Show();
							}
							break;
						}
				}
				SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
			}
		}
	}
	return 0;
}


int JoinWizard::OutColCallBack(Event*) 
{
	fsOutCol->StoreData();
	stAlreadyExists->Hide();
	if (sOutCol.length() != 0) {
		Column col = view->col(sOutCol);
		if (col.fValid()) {
			stAlreadyExists->Show();
			SetWizardButtons(PSWIZB_BACK);
		}
		else
			SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	}
	else
		SetWizardButtons(PSWIZB_BACK);
	return 0;
}

JoinWizard::JoinKeys JoinWizard::jkDetermine()
{
	int jk = jkNone;
	if (view->dm() == tblinf.dm()) 
		jk |= jkTblTbl;
	int i, j;	
	// check if a column in the table has the domain of the table to be joined 
	for (i=0; i < view->iCols(); i++) {
		if (tblinf.dm() == view->col(i)->dm()) {
			jk |= jkColTbl;
			break;
		}
	}
	// check if a column in the table to be joined has the domain of the table 
	for (i=0; i < tblinf.iCols(); i++) {
		if (tblinf.aci[i].dm() == view->dm()) {
			jk |= jkTblCol;
			break;
		}
	}
	// check if a column in the table has the domain of a column in the table to be joined 
	for (i=0; i < view->iCols(); i++){
		for (j=0; j < tblinf.iCols(); j++) {
			if (view->col(i)->dm() == tblinf.aci[j].dm()) {
				Domain dm = view->col(i)->dm();
				if (0 != dm->pdsrt() || 0 != dm->pdbool() || 0 != dm->pdbit() ||  0 != dm->pdi()) {
					jk |= jkColCol;
					break;
				}
			}
		}
	}
	return (JoinKeys)jk;
}

class ResultForm:public FormWithDest 
{
public:
	ResultForm(CWnd* parent, const String sMsg);
private:
	String m_s;
};

ResultForm::ResultForm(CWnd* parent, const String sMsg)
:FormWithDest(parent, "Datum shifts")
{
	m_s = sMsg;
	FieldStringMulti* fsm = new FieldStringMulti(root, &m_s, true);
	fsm->SetHeight(270);
	fsm->SetWidth(200);
	create();
}

LRESULT Cmdclminvmoldensky(CWnd *parent, const String& dummy)
{
	new InverseMolodenskyForm(parent);

	return -1;
}

InverseMolodenskyForm::InverseMolodenskyForm(CWnd* parent)
: TableForm(parent, STBTitleInverseMolodensky)
{
	setView(parent);
	int iFirstCoordCol = iUNDEF;
	int i=0;
	for (; i < view->iCols(); i++) {
		if (view->cv(i)->fCoords()) {
			iFirstCoordCol = i;
			break;
		}
	}
	if (iFirstCoordCol >= 0) 
		sCrdCol1 = view->cv(iFirstCoordCol)->sName();
	int iSecondCoordCol = iUNDEF;
	for (i++; i < view->iCols(); i++) {
		if (view->cv(i)->fCoords()) {
			iSecondCoordCol = i;
			break;
		}
	}
	if (iSecondCoordCol >= 1)
		sCrdCol2 = view->cv(iSecondCoordCol)->sName();

	int iFirstHeightsCol = iUNDEF;
	for ( i=0; i < view->iCols(); i++) {
		if (view->cv(i)->fRealValues()) {
			iFirstHeightsCol = i;
			break;
		}
	}
	if (iFirstHeightsCol >= 0) 
		sHghtCol1 = view->cv(iFirstHeightsCol)->sName();
	int iSecondHeightsCol = iUNDEF;
	for (i++; i < view->iCols(); i++) {
		if (view->cv(i)->fRealValues()) {
			iSecondHeightsCol = i;
			break;
		}
	}
	if (iSecondHeightsCol >= 1)
		sHghtCol2 = view->cv(iSecondHeightsCol)->sName();
	new FieldColumn(root, STBUiFirstCrdColumn, view, &sCrdCol1, dmCOORD);
	new FieldColumn(root, STBUiSecondCrdColumn, view, &sCrdCol2, dmCOORD);
	new FieldColumn(root, STBUiFirstHghtColumn, view, &sHghtCol1, dmVALUE);
	new FieldColumn(root, STBUiSecondHghtColumn, view, &sHghtCol2, dmVALUE);
	//SetMenHelpTopic(htpInverseMolodensky);
	create();
}

double InverseMolodenskyForm::det(double a, double b, double c,
								  double d, double e, double f,
								  double g, double h, double i) {
									  return a*e*i + b*f*g + c*d*h - g*e*c - d*b*i - a*h*f;
}

#define EPS18  1.0e-18

int InverseMolodenskyForm::exec()
{
	FormWithDest::exec();
	long iRec = view->iRecs();
	Column colCrd1 = view->col(sCrdCol1);
	DomainCoord*  pdomcrd = colCrd1->dvrs().dm()->pdcrd();
	CoordSystem cs1 = pdomcrd->cs();
	Column colH1 = view->col(sHghtCol1);

	Column colCrd2 = view->col(sCrdCol2);
	pdomcrd = colCrd2->dvrs().dm()->pdcrd();
	CoordSystem cs2 = pdomcrd->cs();
	Column colH2 = view->col(sHghtCol2);

	Column colActive = view->col("Active");
	String sCoordSys1, sCoordSys2;

	CoordSystemViaLatLon* csvll1 = cs1->pcsViaLatLon();
	CoordSystemViaLatLon* csvll2 = cs2->pcsViaLatLon();
	double a1 = csvll1->ell.a;
	double f1 = csvll1->ell.f;
	double a = csvll2->ell.a;
	double f = csvll2->ell.f;


	//============input from Table columns ========
	//RealArray arLon1(iRec);
	//RealArray arLat1(iRec);
	Coord* crdLatLon1 = new Coord[iRec];
	Coord* crdLatLon2 = new Coord[iRec];
	double* rHeight1 = new double[iRec];
	double* rHeight2 = new double[iRec];

	Array<bool> afActive(iRec);
	long iActivePnt= 0;
	long iNrOfValidPnts = 0;
	double rNTmp = 0, rH1Tmp, rH2Tmp;
	Coord crdTemp;
	long i = 0;
	for (; i < iRec; i++) {
		crdTemp = colCrd1->cValue(i+1);
		if ( crdTemp == crdUNDEF) continue;
		crdLatLon1[iActivePnt] = crdTemp;
		crdTemp = colCrd2->cValue(i+1);
		if ( crdTemp == crdUNDEF) continue;
		crdLatLon2[iActivePnt] = crdTemp;
		// convert from degrees to radians:
		crdLatLon1[iActivePnt].x *= M_PI/180.0;
		crdLatLon1[iActivePnt].y *= M_PI/180.0;
		crdLatLon2[iActivePnt].x *= M_PI/180.0;
		crdLatLon2[iActivePnt].y *= M_PI/180.0;
		rH1Tmp = colH1->rValue(i+1);
		rH2Tmp = colH2->rValue(i+1);
		if (rH1Tmp == rUNDEF || rH2Tmp == rUNDEF) continue;
		rHeight1[iActivePnt] = rH1Tmp;
		rHeight2[iActivePnt] = rH2Tmp;
		iNrOfValidPnts++;
		afActive[i] = colActive.fValid() && (colActive->iValue(i+1) != 0);
		if (afActive[i])
			iActivePnt++;
	}
	/*
	CALL:
	using output parameters:
	CVector vDatumSh;
	real rMaxLatErr, rMaxLonErr, rHeiErr;
	long iNrMaxLatErr, iNrMaxLonErr, iNrMaxHeiErr;

	iFindShiftsFromCoordArrays (5, arLatLon1, asH1, arLatLon2, arH2,
	vDatumSh, rMaxLatErr, rMaxLonErr, rMaxHeiErr,
	iNrMaxLatErr, iNrMaxLonErr, iNrMaxHeiErr)
	DEFINITION:
	int InverseMolodenskyForm::iFindShiftsFromCoordArrays(
	int iActivePnt,
	const Coord* LatLon1,  
	const double* rH1, // 3D coors in csy1
	const Coord* LatLon2,  
	const double* rH2, // 3D coors in csy2
	CVector& vecDatumShift, 
	double& rMaxLatErr,
	double& rMaxLonErr,
	double& rMaxHeightErr,
	int iLatNr, iLonNr, iHeiNr)
	{

	return 0;
	}
	*/
	RealArray dPhi(iRec);
	RealArray dLam(iRec);
	RealArray dHei(iRec);
	RealArray sinPhi(iRec);
	RealArray cosPhi(iRec);
	RealArray sinLam(iRec);
	RealArray cosLam(iRec);
	RealArray sin2Phi(iRec);
	RealArray Hei(iRec);
	for (i = 0; i < iActivePnt; i++) {
		dPhi[i] = crdLatLon1[i].y - crdLatLon2[i].y;
		dLam[i] = crdLatLon1[i].x - crdLatLon2[i].x;
		dHei[i] = rHeight1[i] - rHeight2[i];
		sinPhi[i] = sin(crdLatLon2[i].y);
		cosPhi[i] = cos(crdLatLon2[i].y);
		sinLam[i] = sin(crdLatLon2[i].x);
		cosLam[i]= cos(crdLatLon2[i].x);
		Hei[i] = rHeight2[i];
		sin2Phi[i] = sinPhi[i] * sinPhi[i];
		rNTmp += sin2Phi[i];
	}
	delete crdLatLon1;
	delete crdLatLon2;
	delete rHeight1;
	delete rHeight2;

	if (iActivePnt == 0) return 0;
	long iNrOfConditionEquations = 3 * iActivePnt;
	double mA[300][3]; // overdet system of 300 equations with 3 unknowns
	double cB[300];		// i.e. maximum of 100 control points
	double AtA[3][3]; // matrix of normal equations
	double AtB[3];    // its right-hand vector

	// ======== Solve Molodensky eqs for unknowns dx dy dz 
	// ======== and compute RMS errors for dPhi,dLam and dHei
	double da = a1 - a;
	double df = f1 - f;
	double ee = 2*f - f*f;
	double rTmp = 1 - ee * rNTmp/iActivePnt;
	// N = average radius of curvature in the prime vertical of Local ellipsoid
	double N = a / sqrt(rTmp);
	// M = average radius of curvature in the meridian of Local ellipsoid
	double M = a * (1 - ee) / sqrt(rTmp * rTmp * rTmp);
	double b = (1 - f) * a;

	/* Molodensky equations:
	dPhi = - dx * sinPhi * cosLam - dy * sinPhi * sinLam + dz * cosPhi 
	+ da * (N * ell.e2 * sinPhi * cosPhi) / ell.a
	+ df * (M * ell.a / ell.b + N * ell.b / ell.a) * sinPhi * cosPhi;
	dPhi /= M + H;

	dLam = (-dx * sinLam + dy * cosLam) / ((N + H) * cosPhi);

	dH =   dx * cosPhi * cosLam + dy * cosPhi * sinLam + dz * sinPhi
	- da * ell.a / N + df * N * sin2Phi * ell.b / ell.a;

	Abridged Molodensky equations:
	dPhi = - dx * sinPhi * cosLam - dy * sinPhi * sinLam + dz * cosPhi 
	+ da * (N * ell.e2 * sinPhi * cosPhi) / ell.a         
	dPhi /= M;

	dLam = (-dx * sinLam + dy * cosLam) / (N * cosPhi);

	dH = dx * cosPhi * cosLam + dy * cosPhi * sinLam + dz * sinPhi + (a * df + f * da) * sin2Phi - da

	*/
	// Formulated with dx, dy and dz as unknowns:
	// 	mA(i,0)   * dx  + mA(i,1)   * dy +  mA(i,2)   * dz	 = cBi
	// 	mA(i+1,0) * dx  + mA(i+1,1) * dy          	        = cBi+1
	//  mA(i+2,0) * dx  + mA(i+2,1) * dy +  mA(i+2,2) * dz  = cBi+2
	// i = 0 to 3 X iNrOfActivePnts yields equations with 3 unknowns
	double dx,dy,dz;
	for ( i = 0; i < iActivePnt; i++) 
	{
		mA[3*i][0] = sinPhi[i] * cosLam[i];
		mA[3*i][1] = sinPhi[i] * sinLam[i];
		mA[3*i][2] = -cosPhi[i];
		cB[3*i] = da*(N * ee * sinPhi[i] * cosPhi[i])/a + df*(M*a/b + N*b/a)*sinPhi[i]*cosPhi[i]
		- dPhi[i]*(M + Hei[i]);//* sinOneSec; 
		//dPhi[i], equation for latitude difference in point [i]
		mA[3*i+1][0] = sinLam[i];
		mA[3*i+1][1] = -cosLam[i];
		mA[3*i+1][2] = 0;
		cB[3*i+1] = -(N + Hei[i]) * cosPhi[i] * dLam[i];// * sinOneSec; 
		//dLam[i], equation for longitude difference in point [i]
		mA[3*i+2][0] = cosPhi[i] * cosLam[i];
		mA[3*i+2][1] = cosPhi[i] * sinLam[i];
		mA[3*i+2][2] = sinPhi[i];
		cB[3*i+2] = dHei[i] + da* a / N - df * b * N * sinPhi[i] * sinPhi[i] / a; 
		// dHei[i], equation for height difference in point [i]
	}
	for (i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			AtA[i][j] = 0;
		AtB[i] = 0;
	}
	for (i = 0; i < 3; i++) {
		for (int j = i; j < 3; j++) 
			for (int k = 0; k < iNrOfConditionEquations; k++) {
				AtA[j][i] += mA[k][i]*mA[k][j]; 
				AtA[i][j] = AtA[j][i];
			}
			for (int k = 0; k < iNrOfConditionEquations; k++)
				AtB[i] += mA[k][i]*cB[k];
	}
	// Cramer's Rule for dx and dy in "dlam - equations":
	double rDetXYZ = det( AtA[0][0],AtA[0][1],AtA[0][2], 
		AtA[1][0],AtA[1][1],AtA[1][2],
		AtA[2][0],AtA[2][1],AtA[2][2]);
	if  (  rDetXYZ < EPS18)
		//printf("the system is singular");
		return 0;
	else 
	{
		double rDetX = det( AtB[0],AtA[0][1],AtA[0][2],
			AtB[1],AtA[1][1],AtA[1][2],
			AtB[2],AtA[2][1],AtA[2][2]);
		double rDetY = det( AtA[0][0],AtB[0],AtA[0][2],
			AtA[1][0],AtB[1],AtA[1][2],
			AtA[2][0],AtB[2],AtA[2][2]);
		double rDetZ = det( AtA[0][0],AtA[0][1],AtB[0],
			AtA[1][0],AtA[1][1],AtB[1],
			AtA[2][0],AtA[2][1],AtB[2]);
		dx = rDetX / rDetXYZ;
		dy = rDetY / rDetXYZ;
		dz = rDetZ / rDetXYZ;
	}
	// Determine the accuracy of shift calculation:
	double mAX_B;//[300];//matrix mA into solut vector dxdydz minus righthandCol cB
	double RMS_Phi = 0; // rootMeansSquare error in latitude shift
	double RMS_Lam = 0; // rootMeansSquare error in longitude shift
	double RMS_Hei = 0; // rootMeansSquare error in height shift
	//colPhiErr = Column();
	double maxPhiErr = -1;
	double maxLamErr = -1;
	double maxHeiErr = -1;
	long iPhi, iLam, iHei;
	i = 0;
	for ( long j = 0; j < iRec; j++) 
	{
		if (afActive[j]) {
			// compute first the discrepancies mA*X - cB:
			mAX_B//[3*i] = 
				= (mA[3*i][0] * dx + mA[3*i][1] * dy + 
				mA[3*i][2] * dz - cB[3*i])/(M + Hei[i])
				* 180 * 3600 / M_PI;
			//colPhiErr->PutVal(iRec, mAX_B[3*i]);
			if (abs(mAX_B) > maxPhiErr) {
				maxPhiErr = abs(mAX_B);
				iPhi = j + 1;
			}
			RMS_Phi += mAX_B * mAX_B;
			mAX_B//[3*i+1] = 
				= (mA[3*i+1][0] * dx + mA[3*i+1][1] * dy - 
				cB[3*i+1])/(N + Hei[i])/cosPhi[i]
			* 180 * 3600 / M_PI;
			//colLamErr->PutVal(iRec, mAX_B[3*i+1]);
			if (abs(mAX_B) > maxLamErr) {
				maxLamErr = abs(mAX_B);
				iLam = j + 1;
			}
			RMS_Lam += mAX_B * mAX_B;
			mAX_B//[3*i+2] = 
				= (mA[3*i+2][0] * dx + mA[3*i+2][1] * dy + 
				mA[3*i+2][2] * dz - cB[3*i+2]);
			//colHeightErr->PutVal(iRec, mAX_B[3*i+2]);
			if (abs(mAX_B) > maxHeiErr) {
				maxHeiErr = abs(mAX_B);
				iHei = j + 1;
			}
			RMS_Hei += mAX_B * mAX_B;
			i++;
		}
	}	
	RMS_Phi = sqrt(RMS_Phi / iActivePnt);
	RMS_Lam = sqrt(RMS_Lam / iActivePnt);
	RMS_Hei = sqrt(RMS_Hei / iActivePnt);
	double RMS_Phi_m = RMS_Phi * 111111 / 3600;//convert from arcsec to meter
	double RMS_Lam_m = RMS_Lam * cosPhi[0] * 111111 / 3600;
	double maxPhiErr_m = maxPhiErr * 111111 / 3600;
	double maxLamErr_m = maxLamErr * cosPhi[0] * 111111 / 3600;

	// display these results in a message
	String sMsg = String("dX = %.3f m\r\ndY = %.3f m\r\ndZ = %.3f m\r\n",dx, dy, dz);
	sMsg &= String("\r\n\r\nAccuracy for Lat, Lon and Height:\r\n");
	sMsg &= String("using %li active points.\r\n",iActivePnt);
	sMsg &= String("out of  %li valid control points.\r\n",iNrOfValidPnts);
	sMsg &= String("\r\nRMS_Lat = %.3f arcsec (= ~ %.2f m)",RMS_Phi, RMS_Phi_m);
	sMsg &= String("\r\nRMS_Lon = %.3f arcsec (= ~ %.2f m)",RMS_Lam, RMS_Lam_m);
	sMsg &= String("\r\nRMS_Height = %.3f m",RMS_Hei);
	sMsg &= String("\r\nMax Lat Error = %.3f arcsec (~ %.2f m) at pnt %li",maxPhiErr, maxPhiErr_m, iPhi);
	sMsg &= String("\r\nMax Lon Error = %.3f arcsec (~ %.2f m) at pnt %li",maxLamErr, maxLamErr_m, iLam);
	sMsg &= String("\r\nMax Height Error = %.3f m at pnt %li",maxHeiErr, iHei);

	ResultForm frm(wnd(),sMsg);
	return 0;
}

LRESULT Cmdclmfindmoldensky(CWnd *parent, const String& dummy)
{
	new FindMolodenskyShiftsForm(parent);

	return -1;
}

FindMolodenskyShiftsForm::FindMolodenskyShiftsForm(CWnd* parent)
: TableForm(parent, STBTitleInverseMolodensky)
{
	setView(parent);
	int iFirstCoordCol = iUNDEF;
	int i=0;
	for (; i < view->iCols(); i++) {
		if (view->cv(i)->fCoords()) {
			iFirstCoordCol = i;
			break;
		}
	}
	if (iFirstCoordCol >= 0) 
		sCrdCol1 = view->cv(iFirstCoordCol)->sName();
	int iSecondCoordCol = iUNDEF;
	for (i++; i < view->iCols(); i++) {
		if (view->cv(i)->fCoords()) {
			iSecondCoordCol = i;
			break;
		}
	}
	if (iSecondCoordCol >= 1)
		sCrdCol2 = view->cv(iSecondCoordCol)->sName();

	int iFirstHeightsCol = iUNDEF;
	for ( i=0; i < view->iCols(); i++) {
		if (view->cv(i)->fRealValues()) {
			iFirstHeightsCol = i;
			break;
		}
	}
	if (iFirstHeightsCol >= 0) 
		sHghtCol1 = view->cv(iFirstHeightsCol)->sName();
	int iSecondHeightsCol = iUNDEF;
	for (i++; i < view->iCols(); i++) {
		if (view->cv(i)->fRealValues()) {
			iSecondHeightsCol = i;
			break;
		}
	}
	if (iSecondHeightsCol >= 1)
		sHghtCol2 = view->cv(iSecondHeightsCol)->sName();
	new FieldColumn(root, STBUiFirstCrdColumn, view, &sCrdCol1, dmCOORD);
	new FieldColumn(root, STBUiSecondCrdColumn, view, &sCrdCol2, dmCOORD);
	new FieldColumn(root, STBUiFirstHghtColumn, view, &sHghtCol1, dmVALUE);
	new FieldColumn(root, STBUiSecondHghtColumn, view, &sHghtCol2, dmVALUE);

	create();
}


int FindMolodenskyShiftsForm::exec()
{
	FormWithDest::exec();
	long iRec = view->iRecs();

	Column colCrd1 = view->col(sCrdCol1);
	DomainCoord*  pdomcrd = colCrd1->dvrs().dm()->pdcrd();
	CoordSystem cs1 = pdomcrd->cs();
	Column colH1 = view->col(sHghtCol1);

	Column colCrd2 = view->col(sCrdCol2);
	pdomcrd = colCrd2->dvrs().dm()->pdcrd();
	CoordSystem cs2 = pdomcrd->cs();
	Column colH2 = view->col(sHghtCol2);

	Column colActive = view->col("Active");
	Column colPlNames = view->col("plnaam");
	String sPlNames = String(" ");
	Column colDx;

	CoordSystemViaLatLon* csvll1 = cs1->pcsViaLatLon();
	CoordSystemViaLatLon* csvll2 = cs2->pcsViaLatLon();
	CoordSystemProjection* cspr1 = cs1->pcsProjection();
	CoordSystemProjection* cspr2 = cs2->pcsProjection();
	bool fProj1 = (0 != cspr1);
	bool fProj2 = (0 != cspr2);
	Ellipsoid ell1 = csvll1->ell;
	Ellipsoid ell2 = csvll2->ell;

	//============input from Table columns ========
	LatLon* lalo1 = new LatLon[iRec];
	LatLon* lalo2 = new LatLon[iRec];
	double* rHeight1 = new double[iRec];
	double* rHeight2 = new double[iRec];

	Array<bool> afActive(iRec);
	long iActivePnt= 0;
	long iNrOfValidPnts = 0;
	double rNTmp = 0, rH1Tmp, rH2Tmp;
	Coord cTmp;
	LatLon llTmp;
	for (long i = 0; i < iRec; i++) {
		cTmp = colCrd1->cValue(i+1);
		if (cTmp == crdUNDEF) continue;
		if (fProj1)
			llTmp = cspr1->llConv(cTmp);
		else
			llTmp = LatLon(cTmp.y, cTmp.x);		
		lalo1[iActivePnt] = llTmp;

		cTmp = colCrd2->cValue(i+1);
		if (cTmp == crdUNDEF) continue;
		if (fProj2)
			llTmp = cspr2->llConv(cTmp);
		else
			llTmp = LatLon(cTmp.y, cTmp.x);
		lalo2[iActivePnt] = llTmp;

		rH1Tmp = colH1->rValue(i+1);
		rH2Tmp = colH2->rValue(i+1);
		if (rH1Tmp == rUNDEF || rH2Tmp == rUNDEF) continue;
		rHeight1[iActivePnt] = rH1Tmp;
		rHeight2[iActivePnt] = rH2Tmp;
		iNrOfValidPnts++;
		afActive[i] = colActive.fValid() && (colActive->iValue(i+1) != 0);
		if (afActive[i])
			iActivePnt++;
		if (afActive[i] && iActivePnt < 5) {
			if (colPlNames.fValid()) {
				if (iActivePnt == 0) sPlNames &= String("near: ");
				sPlNames &= String(colPlNames->sValue(i+1));
				if (iActivePnt == 4) sPlNames &= String(".....");
			}
			else
				sPlNames &= String("at LatLon %.3f , %.3f \r\n",llTmp.Lat, llTmp.Lon);
		}
	}

	if (iActivePnt == 0) { 
		String sMsg = String("\r\nNo valid active control points found");
		MessageBox(sMsg.scVal(), "Datum Shifts", MB_OK);
		return 0; // no valid active ctrl points
	}
	CoordCTS cts1, cts2; // cartesian Geocentric coords
	LatLonHeight llh1, llh2;
	double rAvgDx = 0, rAvgDy = 0, rAvgDz = 0;
	double* rDxEr = new double[iActivePnt];//x-shift for point j 
	double* rDyEr = new double[iActivePnt];
	double* rDzEr = new double[iActivePnt];
	double rDxErr = 0, rDyErr = 0, rDzErr = 0;
	for ( long j = 0; j < iActivePnt; j++) {
		llh1 = LatLonHeight(lalo1[j], rHeight1[j]);
		cts1 = ell1.ctsConv(llh1);
		llh2 = LatLonHeight(lalo2[j], rHeight2[j]);
		cts2 = ell2.ctsConv(llh2);
		rAvgDx += rDxEr[j] = cts1.x - cts2.x;
		rAvgDy += rDyEr[j] = cts1.y - cts2.y;
		rAvgDz += rDzEr[j] = cts1.z - cts2.z;
	}
	rAvgDx /= iActivePnt;
	rAvgDy /= iActivePnt;
	rAvgDz /= iActivePnt;

	long j = 0;
	for (long j = 0; j < iActivePnt; j++) {
		rDxEr[j] -= rAvgDx; // deviation from mean x-shift
		rDyEr[j] -= rAvgDy;
		rDzEr[j] -= rAvgDz;
		rDxErr += rDxEr[j] * rDxEr[j];//deviation squared and summed
		rDyErr += rDyEr[j] * rDyEr[j];
		rDzErr += rDzEr[j] * rDzEr[j];
	}
	rDxErr = sqrt(rDxErr / iActivePnt);// RMS computed x-shift
	rDyErr = sqrt(rDyErr / iActivePnt);
	rDzErr = sqrt(rDzErr / iActivePnt);

	// Check accuracy of datumshift for all ctrl points:
	double diff;
	double RMS_Phi = 0; // rootMeansSquare error in latitude shift
	double RMS_Lam = 0; // rootMeansSquare error in longitude shift
	double RMS_Hei = 0; // rootMeansSquare error in height shift
	double maxPhiErr = -1;
	double maxLamErr = -1;
	double maxHeiErr = -1;
	long iPhi, iLam, iHei;
	LatLon llCentroid;
	llCentroid = LatLon(0,0);
	int i = 0;  // i iterates through the active point nrs
	for ( j = 0; j < iRec; j++) 
	{
		if (afActive[j]) {
			LatLonHeight llhDatumShifted;
			llh1 = LatLonHeight(lalo1[i], rHeight1[i]);
			llCentroid.Lat += llh1.Lat;
			cts1 = ell1.ctsConv(llh1);
			cts2.x = cts1.x - rAvgDx;
			cts2.y = cts1.y - rAvgDy;
			cts2.z = cts1.z - rAvgDz;
			llhDatumShifted = ell2.llhConv(cts2);
			diff = (llhDatumShifted.Lat - lalo2[i].Lat);
			if (abs(diff) > maxPhiErr) {
				maxPhiErr = abs(diff);
				iPhi = j + 1;
			}
			RMS_Phi += diff * diff;
			diff = (llhDatumShifted.Lon - lalo2[i].Lon);
			if (abs(diff) > maxLamErr) {
				maxLamErr = abs(diff);
				iLam = j + 1;
			}
			RMS_Lam += diff * diff;
			diff = llhDatumShifted.rHeight - rHeight2[i];
			if (abs(diff) > maxHeiErr) {
				maxHeiErr = abs(diff);
				iHei = j + 1;
			}
			RMS_Hei += diff * diff;
			i++;
		}
	}

	delete lalo1;
	delete lalo2;
	delete rHeight1;
	delete rHeight2;
	delete rDxEr;
	delete rDyEr;
	delete rDzEr;
	llCentroid.Lat /= iActivePnt; // average latitude
	double cosPhi0 = cos(llCentroid.Lat*M_PI/180);
	RMS_Phi = sqrt(RMS_Phi / iActivePnt) * 3600;//convert from degr to arcsec
	RMS_Lam = sqrt(RMS_Lam / iActivePnt) * 3600;
	RMS_Hei = sqrt(RMS_Hei / iActivePnt);
	double RMS_Phi_m = RMS_Phi * 111111 / 3600;//convert from arcsec to meter
	double RMS_Lam_m = RMS_Lam * cosPhi0 * 111111 / 3600;
	maxPhiErr *= 3600; // conv from degr to arcsec
	maxLamErr *= 3600;
	double maxPhiErr_m = maxPhiErr * 111111/3600; //conv from arcsec to meter
	double maxLamErr_m = maxLamErr * cosPhi0 * 111111/3600;

	//==================================================================
	//========== display these results in a message box ================
	//==================================================================
	String sMsg = String("dX = %.3f m\t +/- %.3f m RMS",	rAvgDx,rDxErr);
	sMsg &= String("\r\ndY = %.3f m\t +/- %.3f m RMS",	rAvgDy, rDyErr);
	sMsg &= String("\r\ndZ = %.3f m\t +/- %.3f m RMS\r\n",	rAvgDz, rDzErr);
	sMsg &= String("\r\n\r\n\r\nAccuracy for Lat, Lon and Height:\r\n");
	sMsg &= String("using %li active points, ",iActivePnt);
	sMsg &= sPlNames;
	sMsg &= String("\r\nout of  %li valid control points.\r\n",iNrOfValidPnts);
	sMsg &= String("by retransforming the active points through CTS conversion.\r\n");
	sMsg &= String("\r\nRMS_Lat = %.6f arcsec (= ~ %.3f m)",RMS_Phi, RMS_Phi_m);
	sMsg &= String("\r\nRMS_Lon = %.6f arcsec (= ~ %.3f m)",RMS_Lam, RMS_Lam_m);
	sMsg &= String("\r\nRMS_Height = %.3f m",RMS_Hei);
	sMsg &= String("\r\nMax Lat Error = %.6f arcsec (~ %.3f m) at pnt %li",maxPhiErr, maxPhiErr_m, iPhi);
	sMsg &= String("\r\nMax Lon Error = %.6f arcsec (~ %.3f m) at pnt %li",maxLamErr, maxLamErr_m, iLam);
	sMsg &= String("\r\nMax Height Error = %.3f m at pnt %li",maxHeiErr, iHei);

	ResultForm frm(wnd(),sMsg);
	return 0;
}

LRESULT Cmdclmfind7parameters(CWnd *parent, const String& dummy)
{
	new Find7ParametersForm(parent);

	return -1;
}


Find7ParametersForm::Find7ParametersForm(CWnd* parent)
: TableForm(parent, STBTitleFind7Parameters)
, iActivePnt(0)
{
	setView(parent);
	int iFirstCoordCol = iUNDEF;
	int i=0;
	for (; i < view->iCols(); i++) {
		if (view->cv(i)->fCoords()) {
			iFirstCoordCol = i;
			break;
		}
	}
	if (iFirstCoordCol >= 0) 
		sCrdCol1 = view->cv(iFirstCoordCol)->sName();
	int iSecondCoordCol = iUNDEF;
	for (i++; i < view->iCols(); i++) {
		if (view->cv(i)->fCoords()) {
			iSecondCoordCol = i;
			break;
		}
	}

	if (iSecondCoordCol >= 0)
		sCrdCol2 = view->cv(iSecondCoordCol)->sName();

	int iFirstHeightsCol = iUNDEF;
	for ( i=0; i < view->iCols(); i++) {
		if (view->cv(i)->fRealValues()) {
			iFirstHeightsCol = i;
			break;
		}
	}
	if (iFirstHeightsCol >= 0) 
		sHghtCol1 = view->cv(iFirstHeightsCol)->sName();
	int iSecondHeightsCol = iUNDEF;
	for (i++; i < view->iCols(); i++) {
		if (view->cv(i)->fRealValues()) {
			iSecondHeightsCol = i;
			break;
		}
	}
	if (iSecondHeightsCol >= 1)
		sHghtCol2 = view->cv(iSecondHeightsCol)->sName();

	FieldGroup *fgCols = new FieldGroup(root, true);
	fcFirst = new FieldColumn(fgCols, STBUiFirstCrdColumn, view, &sCrdCol1, dmCOORD);
	fcFirst->SetCallBack((NotifyProc)&Find7ParametersForm::CoordColumnsCallBack);
	fcSecond = new FieldColumn(fgCols, STBUiSecondCrdColumn, view, &sCrdCol2, dmCOORD);
	fcSecond->SetCallBack((NotifyProc)&Find7ParametersForm::CoordColumnsCallBack);

	cbUseHghtCol1 = new CheckBox(fgCols, STBUiFirstHghtColumn, &fUseHghtCol1);
	cbUseHghtCol1->Align(fcSecond, AL_UNDER);
	fcFirstHgt = new FieldColumn(cbUseHghtCol1, String() , view, &sHghtCol1, dmVALUE);
	fcFirstHgt->SetCallBack((NotifyProc)&Find7ParametersForm::UseHghtColumnsCallBack);
	cbUseHghtCol2 = new CheckBox(fgCols, STBUiSecondHghtColumn, &fUseHghtCol2);
	cbUseHghtCol2->Align(cbUseHghtCol1, AL_UNDER);
	fcSecondHgt = new FieldColumn(cbUseHghtCol2, String(), view, &sHghtCol2, dmVALUE);
	fcSecondHgt->SetCallBack((NotifyProc)&Find7ParametersForm::UseHghtColumnsCallBack);

	fUserDefinedPiv = true;
	iDefPivot = 0; // default entered by user
	rXpiv = 0.0; //defaults
	rYpiv = 0.0;
	rZpiv = 0.0;
	rAvgX1 = 0.0; //defaults
	rAvgY1 = 0.0;
	rAvgZ1 = 0.0;
	StaticText* st = new StaticText(root, STBUiRotationPivot);
	st->Align(cbUseHghtCol2, AL_UNDER);
	st->psn->SetBound(0,0,0,0);

	rgPivotDefinition = new RadioGroup(root, "", &iDefPivot, true);
	rgPivotDefinition->SetCallBack((NotifyProc)&Find7ParametersForm::PivotDefinitionCallBack);
	RadioButton* rbU = new RadioButton(rgPivotDefinition, STBUiUserDefP);
	RadioButton* rbC = new RadioButton(rgPivotDefinition, STBUiCentroidAsP);
	rbU->SetIndependentPos();
	rbC->SetIndependentPos();

	m_fgUserDefP = new FieldGroup(root, true);
	m_fgUserDefP->Align(rgPivotDefinition, AL_UNDER);
	frXCoord = new FieldReal(m_fgUserDefP, STBUiXCoord, &rXpiv, ValueRange(-1e7,1e7,0.001));
	frYCoord = new FieldReal(m_fgUserDefP, STBUiYCoord, &rYpiv, ValueRange(-1e7,1e7,0.001));
	frYCoord->Align(frXCoord, AL_AFTER);
	frZCoord = new FieldReal(m_fgUserDefP, STBUiZCoord, &rZpiv, ValueRange(-1e7,1e7,0.001));
	frZCoord->Align(frYCoord, AL_AFTER);

	m_fgCentroidP = new FieldGroup(root, true);
	m_fgCentroidP->Align(rgPivotDefinition, AL_UNDER);
	String s3Dcrds = String(STBMsgCentroidCoords_rrr.scVal(), rAvgX1, rAvgY1, rAvgZ1);
	m_fsCentroidP = new FieldString(m_fgCentroidP, &s3Dcrds, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
	m_fsCentroidP->SetWidth(150);
	String sActivePnts = String(STBMsgActivePnts_i.scVal(), iActivePnt);
	m_fsGeocentricActivePnts = new FieldString(root, &sActivePnts, ES_READONLY);
	iRec = view->iRecs();
	create();
}

void Find7ParametersForm::RecomputeCentroid()
{
	long iActiv = iFindActiveValidCoordsAndHeights();
	if (0 < iActiv)
	{
		iActivePnt = iActiv;
		FindCentroid();
		m_fsCentroidP->SetVal(String(STBMsgCentroidCoords_rrr.scVal(), rAvgX1, rAvgY1, rAvgZ1));
	}
}

int Find7ParametersForm::CoordColumnsCallBack(Event*)
{
	fcFirst->StoreData();
	fcSecond->StoreData();
	colCrd1 = view->col(sCrdCol1);
	if (!colCrd1.fValid()) return 0;
	colCrd2 = view->col(sCrdCol2);
	if (!colCrd2.fValid()) return 0;
	DomainCoord*  pdomcrd1 = colCrd1->dvrs().dm()->pdcrd();
	DomainCoord*  pdomcrd2 = colCrd2->dvrs().dm()->pdcrd();
	cs1 = pdomcrd1->cs();
	cs2 = pdomcrd2->cs();
	RecomputeCentroid();
	return 0;
}

int Find7ParametersForm::UseHghtColumnsCallBack(Event*)
{
	cbUseHghtCol1->StoreData();
	cbUseHghtCol2->StoreData();
	fcFirst->StoreData();
	fcSecond->StoreData();
	colH1 = view->col(sHghtCol1);
	if (!colH1.fValid()) return 0;
	colH2 = view->col(sHghtCol2);
	if (!colH2.fValid()) return 0;
	RecomputeCentroid();
	return 0;
}

int Find7ParametersForm::PivotDefinitionCallBack(Event*)
{ 
	rgPivotDefinition->StoreData();
	switch (iDefPivot) {  // lots of fall throughs
case 0:
	m_fgUserDefP->StoreData();
	fUserDefinedPiv = true;
	m_fgCentroidP->Hide();
	m_fgUserDefP->Show();
	break;
case 1:
	m_fsCentroidP->StoreData();
	fUserDefinedPiv = false;
	RecomputeCentroid();
	m_fgCentroidP->Show();
	m_fgUserDefP->Hide();
	break;
	}
	return 0;
}

long Find7ParametersForm::iFindActiveValidCoordsAndHeights()
{
	if (!cs1.fValid() || !cs2.fValid())
		return false;
	csvll1 = cs1->pcsViaLatLon();
	csvll2 = cs2->pcsViaLatLon();
	cspr1 = cs1->pcsProjection();
	cspr2 = cs2->pcsProjection();
	if ((0 == csvll1 && 0 == cspr1) || (0 == csvll2 && 0 == cspr2))
		return false;
	fProj1 = (0 != cspr1);
	fProj2 = (0 != cspr2);
	if (0 != csvll1) 
		ell1 = csvll1->ell;
	if (0 != csvll2) 
		ell2 = csvll2->ell;

	//============input from Table columns ========
	lalo1 = new LatLon[iRec];
	lalo2 = new LatLon[iRec];
	rHeight1 = new double[iRec];
	rHeight2 = new double[iRec];
	colActive = view->col("Active");
	afActive.Resize(iRec);
	long iActiveP= 0;
	iNrOfValidPnts = 0;
	double rNTmp = 0, rH1Tmp, rH2Tmp;
	Coord cTmp;
	LatLon llTmp;
	for (long i = 0; i < iRec; i++) {
		if (!colCrd1.fValid()) return false;
		String s = colCrd1->sName(); 
		cTmp = colCrd1->cValue(i+1);
		if (cTmp == crdUNDEF) continue;
		if (fProj1)
			llTmp = cspr1->llConv(cTmp);
		else
			llTmp = LatLon(cTmp.y, cTmp.x);		
		lalo1[iActiveP] = llTmp;

		if (!colCrd2.fValid()) return false;
		s = colCrd2->sName(); 
		s &= s;
		cTmp = colCrd2->cValue(i+1);
		if (cTmp == crdUNDEF) continue;
		if (fProj2)
			llTmp = cspr2->llConv(cTmp);
		else
			llTmp = LatLon(cTmp.y, cTmp.x);
		lalo2[iActiveP] = llTmp;
		if (fUseHghtCol1 && !colH1.fValid()) return false;
		if (fUseHghtCol2 && !colH2.fValid()) return false;
		if (fUseHghtCol1)
			rH1Tmp = colH1->rValue(i+1);
		else 
			rH1Tmp = 0;
		if (fUseHghtCol2)
			rH2Tmp = colH2->rValue(i+1);
		else 
			rH2Tmp = 0;
		if (rH1Tmp == rUNDEF || rH2Tmp == rUNDEF) continue;
		rHeight1[iActiveP] = rH1Tmp;
		rHeight2[iActiveP] = rH2Tmp;
		iNrOfValidPnts++;

		afActive[i] = colActive.fValid() && (colActive->iValue(i+1) != 0);
		if (afActive[i])
			iActiveP++;
	}
	return iActiveP; 
}

void Find7ParametersForm::FindCentroid()
{
	CoordCTS cts1, cts2; // cartesian Geocentric coords
	LatLonHeight llh1, llh2;
	if (iActivePnt > 0)
	{
		rAvgX1 = 0;
		rAvgY1 = 0;
		rAvgZ1 = 0;
	}
	else
	{
		rAvgX1 = rUNDEF;
		rAvgY1 = rUNDEF;
		rAvgZ1 = rUNDEF;
		return;
	}
	rX1.Resize(iActivePnt);	
	rY1.Resize(iActivePnt);
	rZ1.Resize(iActivePnt);
	rX2.Resize(iActivePnt);	
	rY2.Resize(iActivePnt);
	rZ2.Resize(iActivePnt);
	for ( long j = 0; j < iActivePnt; j++) {
		llh1 = LatLonHeight(lalo1[j], rHeight1[j]);
		cts1 = ell1.ctsConv(llh1);
		llh2 = LatLonHeight(lalo2[j], rHeight2[j]);
		cts2 = ell2.ctsConv(llh2);
		rX1[j] = cts1.x;
		rY1[j] = cts1.y;
		rZ1[j] = cts1.z;
		rAvgX1 += rX1[j];
		rAvgY1 += rY1[j];
		rAvgZ1 += rZ1[j];
		rX2[j] = cts2.x;
		rY2[j] = cts2.y;
		rZ2[j] = cts2.z;
	}
	rAvgX1 /= iActivePnt;
	rAvgY1 /= iActivePnt;
	rAvgZ1 /= iActivePnt;
}

int Find7ParametersForm::exec()
{
	FormWithDest::exec();
	iRec = view->iRecs();
	if (iRec < 3)
	{
		String sMsg = String("Invalid input, not enough records in table");
		ResultForm frm(wnd(),sMsg);
		return 0;
	}
	colCrd1 = view->col(sCrdCol1);
	DomainCoord*  pdomcrd = colCrd1->dvrs().dm()->pdcrd();
	cs1 = pdomcrd->cs();
	colH1 = view->col(sHghtCol1);

	colCrd2 = view->col(sCrdCol2);
	pdomcrd = colCrd2->dvrs().dm()->pdcrd();
	cs2 = pdomcrd->cs();
	colH2 = view->col(sHghtCol2);
	colActive = view->col("Active");

	//============input from Table columns ========
	lalo1 = new LatLon[iRec];
	lalo2 = new LatLon[iRec];
	rHeight1 = new double[iRec];
	rHeight2 = new double[iRec];

	if (iFindActiveValidCoordsAndHeights() < 3)
	{
		String sMsg = String("Invalid input, less than 3 valid and active records in table");
		ResultForm frm(wnd(),sMsg);
		return 0;
	}

	rX1.Resize(iActivePnt);	
	rY1.Resize(iActivePnt);
	rZ1.Resize(iActivePnt);
	rX2.Resize(iActivePnt);	
	rY2.Resize(iActivePnt);
	rZ2.Resize(iActivePnt);

	// G. Strang v Hees 'Globale en Locale Geod Systemen Delft 3e druk
	// Hfdsk 4 vergelijking (14) en (15) Gelineariseerde gelijkv transf.
	if (!fUserDefinedPiv) {
		rXpiv =	rAvgX1;
		rYpiv = rAvgY1;
		rZpiv = rAvgZ1;
	}
	double dxj, dyj, dzj, dx12j, dy12j, dz12j;
	double rDx = 0, rDy = 0, rDz = 0;
	double rDxDy = 0, rDxDz = 0, rDyDz = 0;
	double rDx2Dy2 = 0, rDx2Dz2 = 0, rDy2Dz2 = 0;
	double rDx2Dy2Dz2 = 0;
	RealMatrix AtA(7,7);
	CVector Atb(7); 
	bool fAtASingular;
	AtA(0,0) = AtA(1,1) = AtA(2,2) = iActivePnt; 
	AtA(0,1) = AtA(0,2) = AtA(1,2) = AtA(1,4) = AtA(2,1) = AtA(2,5) = 0;
	AtA(1,0) = AtA(2,0) = AtA(3,0) = AtA(3,6) = AtA(4,1) = AtA(4,6) = 0;
	AtA(0,3) = AtA(5,2) = AtA(5,6) = AtA(6,3) = AtA(6,4) = AtA(6,5) = 0;
	Atb(0)=Atb(1)=Atb(2)=Atb(3)=Atb(4)=Atb(5)=Atb(6)= 0;
	long j = 0;
	for (; j < iActivePnt; j++) {
		dxj = rX1[j] - rXpiv; rDx += dxj;//add reduced diffs
		dyj = rY1[j] - rYpiv; rDy += dyj;
		dzj = rZ1[j] - rZpiv; rDz += dzj;
		rDxDy += dxj * dyj;
		rDxDz += dxj * dzj;
		rDyDz += dyj * dzj;
		rDx2Dy2 += dxj * dxj + dyj * dyj;
		rDx2Dz2 += dxj * dxj + dzj * dzj;
		rDy2Dz2 += dyj * dyj + dzj * dzj;
		rDx2Dy2Dz2 += dxj * dxj + dyj * dyj + dzj * dzj;
		dx12j = rX2[j] - rX1[j];
		dy12j = rY2[j] - rY1[j];
		dz12j = rZ2[j] - rZ1[j];
		Atb(0) += dx12j;
		Atb(1) += dy12j;
		Atb(2) += dz12j;
		Atb(3) += dy12j * dzj - dz12j * dyj;
		Atb(4) += dz12j * dxj - dx12j * dzj;
		Atb(5) += dx12j * dyj - dy12j * dxj;
		Atb(6) += dx12j*dxj + dy12j*dyj + dz12j*dzj;
	}
	AtA(1,3)= AtA(2,6)= AtA(3,1)= AtA(6,2)= rDz;
	AtA(0,4)= AtA(4,0)= -rDz;
	AtA(0,5)= AtA(1,6)= AtA(5,0)= AtA(6,1)= rDy;
	AtA(2,3)= AtA(3,2)= -rDy;
	AtA(0,6)= AtA(2,4)= AtA(4,2)= AtA(6,0)= rDx;
	AtA(1,5)= AtA(5,1)= -rDx;
	AtA(3,4)= AtA(4,3)= -rDxDy;
	AtA(3,5)= AtA(5,3)= -rDxDz;
	AtA(4,5)= AtA(5,4)= -rDyDz;
	AtA(5,5) = rDx2Dy2;
	AtA(4,4) = rDx2Dz2;
	AtA(3,3) = rDy2Dz2;
	AtA(6,6) = rDx2Dy2Dz2;
	fAtASingular = AtA.fSingular();
	CVector v(7);
	if (fAtASingular){
		String sMsg = String("Singular system ");
		ResultForm frm(wnd(),sMsg);
		return 0;
	}
	else 
		v = LeastSquares(AtA, Atb);
	String sMsg = String("X0 = %7.6f", rXpiv);
	sMsg &= String("\r\nY0 = %7.6f", rYpiv);
	sMsg &= String("\r\nZ0 = %7.6f", rZpiv);
	sMsg &= String("\r\n\r\ndX = %.6f m", v(0));
	sMsg &= String("\r\ndY = %.6f", v(1));
	sMsg &= String("\r\ndZ = %.6f", v(2));
	sMsg &= String("\r\ndalpha = %.12f rad = %.2f nano-rad = %.3f arcsec",
		v(3), v(3)*1000000000, v(3)*180*3600/M_PI);
	sMsg &= String("\r\ndbeta  = %.12f rad = %.2f nano-rad = %.3f arcsec", 
		v(4), v(4)*1000000000, v(4)*180*3600/M_PI);
	sMsg &= String("\r\ndgamma = %.12f rad = %.2f nano-rad = %.3f arcsec", 
		v(5), v(5)*1000000000, v(5)*180*3600/M_PI);
	sMsg &= String("\r\ndscale = %.12f = %.6f ppm",v(6), v(6)*1000000);

	///////////
	/////////// Check accuracy of datumshift for all ctrl points:
	double diff;
	double RMS_Phi = 0; // rootMeansSquare error in latitude shift
	double RMS_Lam = 0; // rootMeansSquare error in longitude shift
	double RMS_Hei = 0; // rootMeansSquare error in height shift
	double maxPhiErr = -1;
	double maxLamErr = -1;
	double maxHeiErr = -1;
	long iPhi, iLam, iHei;
	LatLon llCentroid;
	llCentroid = LatLon(0,0);
	CoordCTS ctsIn, ctsOut, ctsPivot;
	ctsPivot.x = rAvgX1;
	ctsPivot.y = rAvgY1;
	ctsPivot.z = rAvgZ1;
	double tx = v(0);
	double ty = v(1);
	double tz = v(2);
	double Rx = v(3);
	double Ry = v(4);
	double Rz = v(5);
	double s = v(6);
	long i = 0;  // i iterates through the active point nrs
	for ( j = 0; j < iRec; j++) 
	{
		if (afActive[j]) {
			LatLonHeight llh1, llhDatumShifted;
			llh1 = LatLonHeight(lalo1[i], rHeight1[i]);
			llCentroid.Lat += llh1.Lat;
			ctsIn = ell1.ctsConv(llh1);
			ctsOut = ell2.ctsConv(ctsIn, ctsPivot, tx, ty, tz, Rx, Ry, Rz, s);  
			llhDatumShifted = ell2.llhConv(ctsOut);
			diff = (llhDatumShifted.Lat - lalo2[i].Lat);
			if (abs(diff) > maxPhiErr) {
				maxPhiErr = abs(diff);
				iPhi = j + 1;
			}
			RMS_Phi += diff * diff;
			diff = (llhDatumShifted.Lon - lalo2[i].Lon);
			if (abs(diff) > maxLamErr) {
				maxLamErr = abs(diff);
				iLam = j + 1;
			}
			RMS_Lam += diff * diff;
			diff = llhDatumShifted.rHeight - rHeight2[i];
			if (abs(diff) > maxHeiErr) {
				maxHeiErr = abs(diff);
				iHei = j + 1;
			}
			RMS_Hei += diff * diff;
			i++;
		}
	}

	delete lalo1;
	delete lalo2;
	delete rHeight1;
	delete rHeight2;
	llCentroid.Lat /= iActivePnt; // average latitude
	double cosPhi0 = cos(llCentroid.Lat*M_PI/180);
	RMS_Phi = sqrt(RMS_Phi / iActivePnt) * 3600;//convert from degr to arcsec
	RMS_Lam = sqrt(RMS_Lam / iActivePnt) * 3600;
	RMS_Hei = sqrt(RMS_Hei / iActivePnt);
	double RMS_Phi_m = RMS_Phi * 111111 / 3600;//convert from arcsec to meter
	double RMS_Lam_m = RMS_Lam * cosPhi0 * 111111 / 3600;
	maxPhiErr *= 3600; // conv from degr to arcsec
	maxLamErr *= 3600;
	double maxPhiErr_m = maxPhiErr * 111111/3600; //conv from arcsec to meter
	double maxLamErr_m = maxLamErr * cosPhi0 * 111111/3600;
	sMsg &= String("\r\n\r\nAccuracy for Lat, Lon and Height:\r\n");
	sMsg &= String("using %li active points, ",iActivePnt);

	sMsg &= String("\r\nout of  %li valid control points.\r\n",iNrOfValidPnts);
	sMsg &= String("by retransforming the active points through CTS conversion.\r\n");
	sMsg &= String("\r\nRMS_Lat = %.6f arcsec (= ~ %.3f m)",RMS_Phi, RMS_Phi_m);
	sMsg &= String("\r\nRMS_Lon = %.6f arcsec (= ~ %.3f m)",RMS_Lam, RMS_Lam_m);
	sMsg &= String("\r\nRMS_Height = %.3f m",RMS_Hei);
	sMsg &= String("\r\nMax Lat Error = %.6f arcsec (~ %.3f m) at pnt %li",maxPhiErr, maxPhiErr_m, iPhi);

	ResultForm frm(wnd(),sMsg);
	return 0;
}
//=============ctsConv======Polar to Cartesian========
//====i.e. from Ellipsoidal to Conventional Terrestrial System (CTS) coords====
//=============llhConv======Cartesian to Polar========
//====i.e. from Conventional Terrestrial System (CTS) to Ellipsoidal coords====

LRESULT Cmdclmmoledensky(CWnd *parent, const String& dummy)
{
	new ColMolodenskyForm(parent);

	return -1;
}


ColMolodenskyForm::ColMolodenskyForm(CWnd* parent)
: TableForm(parent, STBTitleInverseMolodensky)
{
	setView(parent);
	int iInputCoordCol = iUNDEF;
	for (int i=0; i < view->iCols(); i++) {
		if (view->cv(i)->fCoords()) {
			iInputCoordCol = i;
			break;
		}
	}
	if (iInputCoordCol >= 0) 
		sInpCrdCol = view->cv(iInputCoordCol)->sName();
	new FieldString(root, STBUiOutCol, &sOutpCrdCol, Domain(), false);

	new FieldColumn(root, STBUiFirstCrdColumn, view, &sInpCrdCol, dmCOORD);
	create();
}

int ColMolodenskyForm::exec()
{
	FormWithDest::exec();
	long iRec = view->iRecs();
	Column colInpCrd = view->col(sInpCrdCol);
	DomainCoord*  pdomcrd = colInpCrd->dvrs().dm()->pdcrd();
	CoordSystem cs1 = pdomcrd->cs();
	return 0;
}

LRESULT Cmdclmhelmert7parms(CWnd *parent, const String& dummy)
{
	new ColHelmert7ParmsForm(parent);

	return -1;
}

ColHelmert7ParmsForm::ColHelmert7ParmsForm(CWnd* parent)
: TableForm(parent, STBTitleFind7Parameters)
{
	view = getView(parent);
	int iInputCoordCol = iUNDEF;
	for (int i=0; i < view->iCols(); i++) {
		if (view->cv(i)->fCoords()) {
			iInputCoordCol = i;
			break;
		}
	}
	if (iInputCoordCol >= 0) 
		sInpCrdCol = view->cv(iInputCoordCol)->sName();
	new FieldString(root, STBUiOutCol, &sOutpCrdCol, Domain(), false);

	new FieldColumn(root, STBUiFirstCrdColumn, view, &sInpCrdCol, dmCOORD); 
	create();
}

int ColHelmert7ParmsForm::exec()
{
	FormWithDest::exec();
	long iRec = view->iRecs();
	Column colInpCrd = view->col(sInpCrdCol);
	DomainCoord*  pdomcrd = colInpCrd->dvrs().dm()->pdcrd();
	CoordSystem cs1 = pdomcrd->cs();
	return 0;
}

LRESULT Cmdclmfindazimuthdistance(CWnd *parent, const String& dummy)
{
	new FindAzimuthAndDistanceForm(parent);

	return -1;
}

FindAzimuthAndDistanceForm::FindAzimuthAndDistanceForm(CWnd* parent)
: TableForm(parent, "Find Azimuth and Geodetic Distance")
{
	view = getView(parent);
	int iFirstCoordCol = iUNDEF;
	int i=0;
	for ( ;i < view->iCols(); i++) {
		if (view->cv(i)->fCoords()) {
			iFirstCoordCol = i;
			break;
		}
	}
	if (iFirstCoordCol >= 0) 
		sCrdCol1 = view->cv(iFirstCoordCol)->sName();
	int iSecondCoordCol = iUNDEF;
	for (i++; i < view->iCols(); i++) 
		if (view->cv(i)->fCoords()) {
			iSecondCoordCol = i;
			break;
		}
		if (iSecondCoordCol >= 1)
			sCrdCol2 = view->cv(iSecondCoordCol)->sName();

		new FieldColumn(root, STBUiFirstCrdColumn, view, &sCrdCol1, dmCOORD);
		new FieldColumn(root, STBUiSecondCrdColumn, view, &sCrdCol2, dmCOORD);
		create();
}


int FindAzimuthAndDistanceForm::exec()
{
	FormWithDest::exec();
	long iRec = view->iRecs();
	const double rRadius = 6371007.1809185; //Default Earth Radius
	Column colCrd1 = view->col(sCrdCol1);
	DomainCoord*  pdomcrd = colCrd1->dvrs().dm()->pdcrd();
	CoordSystem cs1 = pdomcrd->cs();

	Column colCrd2 = view->col(sCrdCol2);
	pdomcrd = colCrd2->dvrs().dm()->pdcrd();
	CoordSystem cs2 = pdomcrd->cs();

	Column colActive = view->col("Active");
	Column colactive = view->col("active");

	CoordSystemViaLatLon* csvll1 = cs1->pcsViaLatLon();
	CoordSystemViaLatLon* csvll2 = cs2->pcsViaLatLon();
	CoordSystemProjection* cspr1 = cs1->pcsProjection();
	CoordSystemProjection* cspr2 = cs2->pcsProjection();
	if (csvll1 == 0 || csvll2 == 0) { 
		String sMsg = String("\r\nGeographic or projected coordinates needed");
		MessageBox(sMsg.scVal(), "Azimuths and Distances", MB_OK);
		return 0; // no valid active ctrl points
	}
	bool fProj1 = (0 != cspr1);
	bool fProj2 = (0 != cspr2);
	Ellipsoid ell1 = csvll1->ell;
	Ellipsoid ell2 = csvll2->ell;

	//============input from Table columns ========
	LatLon* lalo1 = new LatLon[iRec];
	LatLon* lalo2 = new LatLon[iRec];

	Array<bool> afActive(iRec);
	Array<double> rAzimuth(iRec);
	Array<double> rDistance(iRec);
	long iActivePnt= 0;
	long iNrOfValidPnts = 0;
	double rNTmp = 0;
	Coord cTmp1, cTmp2;
	LatLon llFrom, llTo;
	bool fActColExists = colActive.fValid() || colactive.fValid();
	if (!colActive.fValid() && !colactive.fValid()) { 
		String sMsg = String("\r\nNo column named active or Active found");
		MessageBox(sMsg.scVal(), "Azimuths and Distances", MB_OK);
		return 0; // no active points can be set
	}
	for (long i = 0; i < iRec; i++) {
		cTmp1 = colCrd1->cValue(i+1);
		if (cTmp1 == crdUNDEF) continue;
		if (fProj1)
			llFrom = cspr1->llConv(cTmp1);
		else
			llFrom = LatLon(cTmp1.y, cTmp1.x);		
		lalo1[iActivePnt] = llFrom;

		cTmp2 = colCrd2->cValue(i+1);
		if (cTmp2 == crdUNDEF) continue;
		if (fProj2)
			llTo = cspr2->llConv(cTmp2);
		else
			llTo = LatLon(cTmp2.y, cTmp2.x);
		lalo2[iActivePnt] = llTo;

		iNrOfValidPnts++;
		afActive[i] = (colActive->iValue(i+1) != 0);
		if (afActive[i]) {
			iActivePnt++;
			rAzimuth[i] = csvll1->ell.rEllipsoidalAzimuth(llFrom, llTo);
			double di = csvll1->ell.rEllipsoidalDistance(llFrom, llTo);
			rDistance[i] = di;
			if (rDistance[i] > 800000) {
				Distance dist(cs1 , Distance::distSPHERE, rRadius);
				di = dist.rDistance(cTmp1, cTmp2);
				//di = csvll1-> rSphericalDistance(llFrom, llTo);
				rDistance[i] = di;
			}
		}
	}

	if (iActivePnt == 0) { 
		String sMsg = String("\r\nNo valid active points found");
		MessageBox(sMsg.scVal(), "Azimuths and Distances", MB_OK);
		return 0; // no valid active ctrl points
	}


	//==================================================================
	//========== display these results in a message box ================
	//==================================================================


	String sOutputString("LatLonFrom  LatLonTo			Azimuth   Geodetic Distance: ");
	for ( long j = 0; j < iActivePnt; j++) {
		sOutputString &= String("\r\n (%.5f, %.5f)=>",	lalo1[j].Lat, lalo1[j].Lon);
		sOutputString &= String(" (%.5f, %.5f) ",	lalo2[j].Lat , lalo2[j].Lon);
		sOutputString &= String("  %.7f deg %.3f m ",	rAzimuth[j], rDistance[j]);
		if (rDistance[j] > 800000)
			sOutputString &= String(" (spherical values if dist > 800 km.)");
	}
	delete lalo1;
	delete lalo2;
	ResultForm frm(wnd(),sOutputString);
	return 0;
}
