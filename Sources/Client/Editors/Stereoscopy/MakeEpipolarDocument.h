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
// MakeEpipolarDocument.h: interface for the MakeEpipolarDocument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAKEEPIPOLARDOCUMENT_H__DAB26883_D701_4D0B_978D_8752DFD9FDDD__INCLUDED_)
#define AFX_MAKEEPIPOLARDOCUMENT_H__DAB26883_D701_4D0B_978D_8752DFD9FDDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PreStereoMateView;

class Element // used in MakeEpipolarDocument and PreStereoMateView
{
public:
	Element();
	Coord crd;
	String sDescription;
	int iType; // 0 = fiducial, 1 = pp, 2 = tpp, 3 = ofp, 4 = upp (user-supplied pp)
	Element(RowCol rcIn, int iTypeIn, String sDescriptionIn)
	{
		// iType : 0 = fiducial, 1 = pp, 2 = tpp, 3 = ofp, 4 = upp (user princpoint)
		if (!rcIn.fUndef())
			crd = Coord(rcIn.Row, rcIn.Col);
		else
			crd = crdUNDEF;
		iType = iTypeIn;
		sDescription = sDescriptionIn;
	}
	Element(Coord crdIn, int iTypeIn, String sDescriptionIn)
	{
		// iType : 0 = fiducial, 1 = pp, 2 = tpp, 3 = ofp, 4 = upp (user princpoint)
		crd = crdIn;
		iType = iTypeIn;
		sDescription = sDescriptionIn;
	}
	bool operator==(Element e) // we defined this for the list->remove operation
	{
		// type and description are sufficient for two elements to be equal
		return ((sDescription == e.sDescription) && (iType == e.iType));
	}
};

class MakeEpipolarDocument : public CDocument  
{
public:
	MakeEpipolarDocument();
	virtual ~MakeEpipolarDocument();

	void UpdateAllEpipolarViews(CView* pSender, LPARAM lHint = 0L, CObject* pHint = NULL);
	// calls UpdateAllViews and updates one extra view that is not associated with this document
	void SetView(PreStereoMateView*);
	// set the view that for the time being can't follow the doc/view principle
	enum iFormStateTP {ifsFIDUCIALS, ifsPP, ifsTPP, ifsOFFFLIGHTPTS}; // the "next input" state
	void SetState(iFormStateTP iState, int iSubState); // to set the state and substate
	void SetState(iFormStateTP iState); // set the state only; preserve the substate
	void SetSubState(int iSubState);
	void AdvanceSubState(); // to correctly browse through all possible substates (state never changes)
	int iGetHintFromState(); // returns a lHint that usually represents what has to be updated
	iFormStateTP ifsGetState();
	int iGetSubState();
	void SetRowCol(const RowCol&);
	void SetPP(const RowCol&);
	void SetTPP(const RowCol&);
	void SetScale(double);
	RowCol rcGetPP();
	int iGetNrFiducials();
	int iGetNrOffFlightPts();
	double rGetAngle();
	double rGetScale();
	list <Element> ElementList();
	Coord crdGetRCFromHint(int);
	Coord crdGetOldRC();
	bool fGetSelectedFromHint(int);
	void SetFiducial(bool, const RowCol&, int);
	void SetOffFlightLinePt(bool, const RowCol&, int);
	int Compute();
	GeoRefEpipolar * grEpi;
	void SetGrEpiMember(GeoRefEpipolar*);
	void SetStateFiducials();
	void SetStatePPoint();
	void SetStateTPPoint();
	void SetStateScalingPts();
	void OnUpdateFiducials(CCmdUI*);
	void OnUpdatePPoint(CCmdUI*);
	void OnUpdateTPPoint(CCmdUI*);
	void OnUpdateScalingPts(CCmdUI*);

private:
	bool fPPDefined;	// we can't do rcPP.fUndef() because we don't know for sure what
										// iFindPrincPointLocation does with rcPP
	iFormStateTP iFormState;
	int iFormSubState [4]; // current fiducial or off flight pt, 0 based
	PreStereoMateView *mpv;
	Coord oldRC;

	DECLARE_DYNCREATE(MakeEpipolarDocument)
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_MAKEEPIPOLARDOCUMENT_H__DAB26883_D701_4D0B_978D_8752DFD9FDDD__INCLUDED_)
