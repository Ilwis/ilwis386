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
// SegmentMapDrawer.h: interface for the SegmentMapDrawer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SegmentMapDrawer)
#define AFX_SegmentMapDrawer

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SegmentMapDrawer : public BaseMapDrawer  
{
	friend class SegmentMapDrawerForm;
public:
  SegmentMapDrawer(MapCompositionDoc*, const SegmentMap&);
  SegmentMapDrawer(MapCompositionDoc*, const MapView&, const char* sSection);
  ~SegmentMapDrawer();
	//{{AFX_VIRTUAL(SegmentMapDrawer)
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop);
  virtual String sInfo(const Coord&);
// virtual Record rec(const Coord&);
//  virtual void Edit();
  virtual int Configure(bool fShow=true);
  virtual int Setup();
//virtual bool fProperty();
//  virtual void Prop();
  virtual void WriteLayer(MapView&, const char* sSection);
  virtual FormBaseDialog* wEditRpr(CWnd* wPar, const Coord& crd);
  virtual FormBaseDialog* wEditRpr(CWnd* wPar, int iRaw);
  virtual IlwisObject obj() const;
	virtual zIcon icon() const;
  virtual void DrawLegendRect(CDC*, CRect, int iRaw) const;
  virtual void DrawValueLegendRect(CDC*, CRect, double rValue) const;
  virtual void SetWidth(int _iWidth) {iWidth = _iWidth;} // meant for overruling iWidth's default value
  virtual int iGetWidth() const {return iWidth;} // get this to calculate a better default value - put result back with SetWidth
	//}}AFX_VIRTUAL
	//{{AFX_MSG(SegmentMapDrawer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
  bool fSegInMask(const ILWIS::Segment*) const;
  SegmentMap sm;
	int iWidth;
	bool fMask;
	Mask mask;
};

#endif // !defined(AFX_SegmentMapDrawer)
