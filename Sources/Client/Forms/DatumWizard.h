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
#ifndef ILW_DATUMWIZARD_H
#define ILW_DATUMWIZARD_H

#include "Engine\SpatialReference\Ellips.h"
#include "Client\FormElements\FormBasePropertyPage.h"
#include "Engine\Map\Point\PNT.H"

class FieldColumn;
class Datum;

class PtMapDomCsy
{
public:
	bool fValid() { return pm.fValid(); }
	String sCol;
	Domain dm;
	CoordSystem cs;
	PointMap pm;
};

class PtCoordArrays
{
public:
	PtCoordArrays() : lalo(0), rHeight(0), ctsCtrPnt(0) {}
	bool fValid() 
		{ return lalo.size() > 0 && rHeight.size() > 0;}
	CoordCTS ctsFindCentroid(long iActivePnt);

	Array<LatLon> lalo;
	Array<double> rHeight;
	Array<CoordCTS> ctsCtrPnt;
	Ellipsoid ellSpheroid;
};

class CalculateDatumParms
{
public:
	CalculateDatumParms();
	static CalculateDatumParms* create(long iMethod);

	void Init(const PtMapDomCsy& pmdcA, const PtMapDomCsy& pmdcB, bool fHA, bool fHB);
	virtual bool fCalc(CoordCTS& ctsPivot, bool fScaleRotF);

	virtual String sResult();

	virtual void UpdateDatum();
	bool fUpdateDatum;
	CoordCTS ctsGetDefaultPivot(PtCoordArrays pca);	
	PtCoordArrays GetPtCoorsA();
	bool fScaleAndRotationFirst;
	long iGetValidPoints() { return iNrPntsCommon; }
	CoordCTS ctsUsedPivot;
	
protected:
	bool fGetCoordAndConvToLL(PtMapDomCsy& pmdc, CoordSystemProjection *cspr, long i, LatLon& ll);
	long iFindValidCoordsAndHeights();
	virtual Datum* datCreate() = 0;
	PtMapDomCsy m_pmdcA;
	PtMapDomCsy m_pmdcB;
	Ellipsoid m_ell1, m_ell2;
	PtCoordArrays m_pcarA;
	PtCoordArrays m_pcarB;
	
	Array<double> rX1; //array of used geocentric coords
	Array<double> rY1;
	Array<double> rZ1;
	Array<double> rX2;
	Array<double> rY2;
	Array<double> rZ2;

	long   iNrPntsCommon;
	bool   m_fUseHeightsA, m_fUseHeightsB;
	long   m_iMethod;

	// Output parameters
	double Dx; // shifts found
	double Dy;
	double Dz;
	double Rx, Ry, Rz; // rotations found 
	double dS; // scale difference
	double X0;
	double Y0;
	double Z0;

	void UseLocalCentersToScaleAndReduce(int iPoints, 
					CoordCTS ctsCentr1, CoordCTS ctsCentr2,
                    double& rScale2ov1, double& rScale1ov2);
	bool fFindRotations(int iPoints, double& rXRot, double& rYRot, double& rZRot);
	void FindTranslations(CoordCTS ctsCentr1, CoordCTS ctsCentr2,
												double rXRot, double rYRot, double rZRot,
												double& rTx, double& rTy, double& rTz,
												double rScale2ov1);
	Array<CoordCTS> ctsRedXYZ2;
	Array<CoordCTS> ctsRedXYZ1;//reduced to local centroid
	double rScale2over1, rScale1over2;
};

class CalculateDatumParmsGeoCenterShift:public CalculateDatumParms
{
public:
	CalculateDatumParmsGeoCenterShift();
	bool fSolveMeanGeoCenterShift(double& dx,	double& dy, double& dz); // returns true if success
	virtual bool fCalc(CoordCTS& ctsPivot, bool fScaleRotF);

	virtual String sResult();
protected:
	virtual Datum* datCreate();
};

class CalculateDatumParmsInverseMolodensky:public CalculateDatumParms
{
public:
	CalculateDatumParmsInverseMolodensky();
	bool fSolveInverseMolodSystem(double& dx,	double& dy, double& dz); // returns true if successful
	virtual bool fCalc(CoordCTS& ctsPivot, bool fScaleRotF);

	virtual String sResult();
protected:
	virtual Datum* datCreate();
};

class CalculateDatumParmsBursaWolf:public CalculateDatumParms
{
public:															 
	CalculateDatumParmsBursaWolf();
	virtual bool fCalc(CoordCTS& ctsPivot, bool fScaleRotF);
	virtual String sResult();
protected:
	bool fSolveBursaWolf(double& dx, double& dy, double& dz,
												double& Rx, double& Ry, double& Rz,
												double& dScale, bool fScaleRotF);// returns true if successful
	virtual Datum* datCreate();
};

class CalculateDatumParmsBadekas:public CalculateDatumParms
{
public:															 
	CalculateDatumParmsBadekas();
	bool fCalc(CoordCTS& ctsPivot, bool fScaleRotF);
	virtual String sResult();
protected:
	bool fSolveBadekas(double& dx, double& dy, double& dz,
											double& Rx, double& Ry, double& Rz,
											double& dScale, CoordCTS cts, bool fScaleRotF);// returns true if successful
	virtual Datum* datCreate();
};

class FormBaseWizardPage;

class _export DatumWizard : public CPropertySheet
{
	friend class SelectPointMapsPage;
	friend class SelectMethodPage;
	friend class UpdateDatumPage;
	
public:
	DatumWizard(CWnd* wnd, const char* sPar);
	~DatumWizard();
	void SwitchToNextPage(FormBaseWizardPage *page);
	bool fUpdateDatum;
	bool fScaleAndRotationFirst;
	double x0; // rotation pivot for Badekas transformation
	double y0;
	double z0;
protected:
	CoordCTS ctsWizPivot;
	
private:
	FormBaseWizardPage *ppSelectPointMaps, *ppSelectMethod, *ppUpdateDatum;

	CalculateDatumParms* cdp;
	PtMapDomCsy pmdcA, pmdcB;
	PtCoordArrays pcarA, pcarB;
	bool        fUseHeightsA, fUseHeightsB;
	int         iMethod;
	
	FileName fnPntA, fnPntB;
};

class SelectPointMapsPage : public FormBaseWizardPage
{
public:
	SelectPointMapsPage(DatumWizard* dw, const char* sPar);
	
	void SetDefaultPointMaps();  // Get optional pointmap parameters from the wizard
  virtual BOOL OnSetActive();
	LRESULT      OnWizardNext();

private:
  int MapACallBack(Event*);
	int MapBCallBack(Event*);

	PtMapDomCsy BuildAndCheckStruct(const String& sMap);
	bool        fCheckDomainEquality(PtMapDomCsy pmdcA, PtMapDomCsy pmdcB);
	long        iFindValidCoordsAndHeights();

private:
	DatumWizard    *m_dw;

	FieldGroup     *fgA, *fgB;
  FieldPointMap  *fldMapA; 
	FieldPointMap  *fldMapB;
  FieldColumn    *fldColA; 
	FieldColumn    *fldColB;
	CheckBox       *cbUseHeightsA;
	CheckBox       *cbUseHeightsB;
	StaticText    *stNrPntsA;
	StaticText    *stNrPntsB;
	StaticText    *stCsyNameA; 
	StaticText    *stCsyNameB;
	StaticText    *stDatumNameA; 
	StaticText    *stDatumNameB;
	StaticText     *stRemark;

	String         sMapA, sMapB, sColA, sColB;
	int            iNrPntsA, iNrPntsB;

	String         sCoordSysA, sCoordSysB;
	String			sEllipsoidA, sEllipsoidB;
	String			sDatumA, sDatumB;
};

class SelectMethodPage : public FormBaseWizardPage
{
public:
	SelectMethodPage(DatumWizard* dw);
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	LRESULT OnWizardNext();

private:
	DatumWizard* m_dw;

	int MethodCallBack(Event*);
	
	RadioGroup *rgMethod;
	FieldGroup* m_fgUserDefP;
	FieldReal* frXCoord;
	FieldReal* frYCoord;
	FieldReal* frZCoord;
	CheckBox *cbScaleAndRotationFirstBW;
	CheckBox *cbScaleAndRotationFirstMB;
	StaticText *stRemark;
};

class UpdateDatumPage : public FormBaseWizardPage
{
	DECLARE_MESSAGE_MAP()
public:
	UpdateDatumPage(DatumWizard* dw);

	void SetShiftString(const String& sShift);
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	LRESULT OnWizardBack();

protected:
	afx_msg void OnTimer(UINT nIDEvent);

private:
	DatumWizard* m_dw;
	CheckBox *cbUpdateDatum;
	StaticText *stCsyNameA1;
	StaticText *stCsyNameA2;
	FieldStringMulti* fsm;
	StaticText *stRemark;
	StaticText *stRemark2;
	StaticText *stRemark3;
	String m_s;
	UINT m_nTimer;
};

#endif
