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
/* $Log: /ILWIS 3.0/TableWindow/Tblforms.h $
 * 
 * 15    23-12-05 16:31 Hendrikse
 * New form that comes when Tablecommand "findazimdist" is given on the
 * table comm line.
 * It is to compute geodetic (ellipsoidal) distance and azimuth between
 * latlons (or projected points) from 2 given columns with coordsystem
 * domains (CoordsysViaLatLon required)
 * 
 * 14    24-10-03 16:50 Willem
 * - Changed: The table commands Join, Aggregate and LSQ now let their
 * forms handle the default aggregate/regression selection instead of by
 * the field. This prevents losing references to temporaries
 * 
 * 13    27-03-03 18:09 Hendrikse
 * added better Callbacks  for the change of selected columns in
 * (undocumented Find7parms function in TableForms project (geodetic datum
 * transform)
 * 
 * 12    12-03-03 10:30 Hendrikse
 * changed statictext into FieldString to allow to copy from it
 * 
 * 11    10-03-03 11:57 Hendrikse
 * changediFindActiveValidCoordsAndHeights to return bool and splitted off
 * the function void FindCentroid from it, fir callback purposes.
 * Removed redundant members rXcentr etc and their FieldReals
 * 
 * 10    27-02-03 22:04 Hendrikse
 * added variables to construct the Find7ParametersForm
 * 
 * 9     5-02-03 13:57 Hendrikse
 * added Find7ParametersForm and column forms
 * 
 * 9	20-01-03 8:58 Hendrikse
 * added classes to make forms for finding 7 parameters of
 * Helmert transformation from cts1 to cts2 (geocentric systems
 * Find7ParametersForm
 *
 * 8     9-05-01 8:58 Hendrikse
 * added classes to make forms for Datum shift finding:
 * InverseMolodenskyForm finds dx,dy,da from inverted equations
 * FindMolodenskyShiftsForm from LatLonH to CTS conversions
 * 
 * 7     7/12/00 2:42p Wind
 * adapted JoinWizard
 * 
 * 6     5/22/00 11:55a Wind
 * adapted form cumulative to sortby column
 * 
 * 5     5/17/00 10:22a Wind
 * allow more flexibility when both tables have matching domains
 * 
 * 4     4/18/00 3:51p Wind
 * finished JoinWizard
 * 
 * 3     4/12/00 6:04p Wind
 * working on JoinWizard (not yet finished)
 * 
 * 2     1-07-99 11:10 Koolhoven
 * 
 * 1     30-06-99 15:31 Koolhoven
 * Interface for Table Forms
   by Wim Koolhoven, october 1996
   (c) Computer Department ITC
	Last change:  WK   18 May 99    5:20 pm
*/

#include "Engine\Table\tblview.h"
#include "Engine\SpatialReference\Ellips.h"
#ifndef TBLFORMS_H
#define TBLFORMS_H

LRESULT Cmdclmaggregate(CWnd *parent, const String& dummy);
LRESULT Cmdjoin(CWnd *parent, const String& s);
LRESULT Cmdclmstatistiscs(CWnd *parent, const String& dummy);
LRESULT Cmdclmcumulative(CWnd *parent, const String& dummy);
LRESULT Cmdclmsemivar(CWnd *parent, const String& dummy);
LRESULT Cmdclmconfusionmatrix(CWnd *parent, const String& dummy);
LRESULT Cmdclmleastsquares(CWnd *parent, const String& dummy);
LRESULT Cmdclmsort(CWnd *parent, const String& s);
LRESULT Cmdclmslicing(CWnd *parent, const String& dummy);
LRESULT Cmdclminvmoldensky(CWnd *parent, const String& dummy);
LRESULT Cmdclmfindmoldensky(CWnd *parent, const String& dummy);
LRESULT Cmdclmfind7parameters(CWnd *parent, const String& dummy);
LRESULT Cmdclmmoledensky(CWnd *parent, const String& dummy);
LRESULT Cmdclmhelmert7parms(CWnd *parent, const String& dummy);
LRESULT Cmdclmfindazimuthdistance(CWnd *parent, const String& dummy);
LRESULT Cmdtimefromcolumns(CWnd *parent, const String& dummy);
LRESULT Cmdaddrecord(CWnd *parent, const String& command);
LRESULT CmdColMan(CWnd *parent, const String& s);

LRESULT CmdUpdateAllColumns(CWnd *parent, const String& s);


class TableForm : public FormWithDest {
public:
	TableForm(CWnd *parent, const String& sTitle);
	static TableView *getView(CWnd* parent);
	void setView(CWnd *parent);

protected:
	TableDoc* tbldoc;
	TableView* view;
};


class ColSortOnForm : public TableForm {

public:
	ColSortOnForm(CWnd* parent, int* iSort, String* sCol, int* iSortOrder);
	int exec();
};

class FieldAggrFunc;
class AggregateForm: public TableForm
{
public:
  AggregateForm(CWnd* parent);
  int exec();
private:
  String sCol, sGroupCol, sWeightCol, sTable, sOutCol;
  String* sFunc;
  bool fGroupBy, fWeight, fTable;
  int ColFuncCallBack(Event*);
  int FuncCallBack(Event*);
  FieldColumn* fcol;
  FieldAggrFunc* faf;
  CheckBox *cbGroup, *cbWeight, *cbTable;
  StaticText* stRemark;
  String m_sDefault;
};

class ColSlicingForm : public TableForm {
public:
	ColSlicingForm(CWnd *parent);
	String sInpCol, sOutCol, sDomain;
    int exec();
};

class ConfusionMatrixForm: public TableForm
{
public:
  int exec();
  ConfusionMatrixForm(CWnd* paren);
private:
  String sCol1, sCol2, sColFreq;
};

class CumulativeForm: public TableForm
{
public:
  int exec();
  CumulativeForm(CWnd* parent);
  String sCol, sColSortBy, sOutCol;
	bool fSortBy;
};

class FieldRegressionFunc;
class LeastSquaresForm: public TableForm
{
public:
  int exec();
  LeastSquaresForm(CWnd* parent);
  String sOutCol, sColX, sColY, *sFunc;
  int iTerms;
private:  
  int FuncCallBack(Event*);
  FieldRegressionFunc* frf;
  FieldInt* fiTerms;
  String m_sDefault;
};

class FieldTblStatsFunc;
class TblStatsForm: public TableForm
{
public:
	int exec();
  TblStatsForm(CWnd* parent);
  String sExpr;
private:
  int FuncCallBack(Event*);
  FieldTblStatsFunc* fsf;
  FieldReal *fr;
  FieldColumn *fc1, *fc2, *fcExp;
  String *sFunc;
  double rVal;
  String sCol1, sCol2;
};

class ColSemiVarForm: public TableForm
{
public:
  int exec();
  ColSemiVarForm(CWnd* parent);
  String sOutCol, sColDist;
  SemiVariogram smv;
};

class FormBaseWizardPage;

class JoinWizard: public CPropertySheet
{
public: // everything public because wizard pages need access
  JoinWizard(CWnd* wnd);
	~JoinWizard();
	FormBaseWizardPage *ppInputColumn, *ppJoinType, /**ppKeyColumn, *ppViaKey,*/ *ppKeys, *ppAggr, *ppOutputColumn;
  String sTbl, sCol, sKeyCol, sViaKey, sWeightCol, sOutCol;
  String* sFunc;
  bool fKeyCol, fViaKey, fAggregate, fWeight;
	enum JoinKeys { jkNone = 0, jkTblTbl = 1, jkColTbl = 2, jkTblCol = 4, jkColCol = 8 };
	JoinKeys jkDetermine();
	int iJoinType; // for radio buttons
	int jk; // for 'or'-ing of JoinKeys enum
	int iIndexKeyColumn; // index in view
  int TblCallBack(Event*);
  int ColCallBack(Event*);
  int KeyColCallBack(Event*);
  int ViaKeyCallBack(Event*);
  int OutColCallBack(Event*);
  int JoinTypeCallBack(Event*);
  FieldAggrFunc *faf;
  FieldDataType *ftbl;
  FieldColumn *fcKeyCol, *fcCol, *fcViaKey;
  FieldString *fsOutCol;
	RadioGroup *rgJoinType;
	RadioButton *rbTblTbl, *rbColTbl, * rbTblCol, *rbColCol;
	StaticText *stNone1, *stNone2, *stNoWeight, *stAlreadyExists;
	StaticText *stSelectedTable, *stSelectedColumn, *stKey1, *stKey2, *stAggr1, *stAggr2;
  TableView* view;
  TableInfo tblinf;
	Table tblInp;
};

class InverseMolodenskyForm: public TableForm
{
public:
  InverseMolodenskyForm(CWnd* parent);
	String sExpr;
private:
  int FuncCallBack(Event*);
	String sCrdCol1, sCrdCol2, sHghtCol1, sHghtCol2, sActiveCol;
	double det(double a, double b, double c,
					 double d, double e, double f,
					 double g, double h, double i); 
  int exec();
	//Column colPhiErr, colLamErr, colHeightErr;
	//ArrayLarge<bool> fActive;
	TableView* view;
};

class FindMolodenskyShiftsForm: public TableForm
{
public:
  FindMolodenskyShiftsForm(CWnd* parent);
	String sExpr;
private:
  int FuncCallBack(Event*);
	String sCrdCol1, sCrdCol2, sHghtCol1, sHghtCol2, sActiveCol;
	double det(double a, double b, double c,
					 double d, double e, double f,
					 double g, double h, double i); 
  int exec();
	//Column colPhiErr, colLamErr, colHeightErr;
	//ArrayLarge<bool> fActive;
	TableView* view;
};

class Find7ParametersForm: public TableForm
{
public:
  Find7ParametersForm(CWnd* parent);
	String sExpr;

private:
	long iFindActiveValidCoordsAndHeights();
	void FindCentroid();
	int InitReadOnly(Event *);
	
	String sCrdCol1, sCrdCol2, sHghtCol1, sHghtCol2, sActiveCol;
	Column colCrd1, colCrd2, colH1, colH2, colActive;
	long iRec, iNrOfValidPnts, iActivePnt; //nr of records, valid recs, active recs  in table
	Array<bool> afActive;
	Array<double> rX1; //array of used geocentric coords
	Array<double> rY1;
	Array<double> rZ1;
	Array<double> rX2;
	Array<double> rY2;
	Array<double> rZ2;
	CoordSystem cs1, cs2;
  CoordSystemViaLatLon* csvll1;
  CoordSystemViaLatLon* csvll2;
	CoordSystemProjection* cspr1;
	CoordSystemProjection* cspr2;
	bool fProj1;
	bool fProj2;
	Ellipsoid ell1;
	Ellipsoid ell2; 
		//============input from Table columns ========
	LatLon* lalo1;
	LatLon* lalo2;
	double* rHeight1;
	double* rHeight2;
	FieldColumn *fcFirst;
	FieldColumn *fcSecond;
	FieldColumn *fcFirstHgt; 
	FieldColumn	*fcSecondHgt;
	bool fUseHghtCol1, fUseHghtCol2;
	CheckBox *cbUseHghtCol1, *cbUseHghtCol2;
	int CoordColumnsCallBack(Event*);
	//int SecondColumnCallBack(Event*);
	int UseHghtColumnsCallBack(Event*);
	//int UseHghtCol2CallBack(Event*);
	void RecomputeCentroid();
	double rAvgX1, rAvgY1, rAvgZ1;
	double rXpiv, rYpiv, rZpiv;
	RadioGroup* rgPivotDefinition;
	bool fUserDefinedPiv;
	int iDefPivot;
  int PivotDefinitionCallBack(Event*);
	FieldGroup *m_fgUserDefP;
	FieldGroup *m_fgCentroidP;
	FieldString *m_fsCentroidP;
	FieldReal  *frXCoord;
  FieldReal  *frYCoord;
  FieldReal  *frZCoord;
	FieldString *m_fsGeocentricActivePnts;
  int exec();
	//Column colPhiErr, colLamErr, colHeightErr;
	//ArrayLarge<bool> fActive;
	TableView* view;
};

class ColMolodenskyForm: public TableForm
{
public:
  ColMolodenskyForm(CWnd* parent);
  String sInpCrdCol, sOutpCrdCol;
  Ellipsoid ell;
  double rDx, rDy, rDz; 
private:
  int exec();
  TableView* view;
};

class ColHelmert7ParmsForm: public TableForm
{
public:
	int exec();
	ColHelmert7ParmsForm(CWnd* parent);
	String sInpCrdCol, sOutpCrdCol;
	Ellipsoid ell;
	double rDx, rDy, rDz, rDalpha, rDbeta, rDgamma, rDscale; 
private:
	TableView* view;
};

class FindAzimuthAndDistanceForm: public TableForm
{
	public:
  FindAzimuthAndDistanceForm(CWnd* parent);
	String sExpr;
private:
  int FuncCallBack(Event*);
	String sCrdCol1, sCrdCol2, sActiveCol;	
  int exec();
	TableView* view;
};

class TimeColumnFromOtherColsForm : public TableForm {
public:
	TimeColumnFromOtherColsForm(CWnd *);
	int exec();
private:
	FormEntry *CheckData();

	int source;
	String year;
	bool useYear;
	String month;
	bool useMonth;
	String day;
	bool useDay;
	String hour;
	bool useHour;
	String minutes;
	bool useMinutes;
	String seconds;
	bool useSeconds;
	String stringColumn;
	bool useStringColumn;
	String output;
	String templ;

};

#endif // TBLFORMS_H

