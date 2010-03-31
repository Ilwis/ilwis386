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
#if !defined(AFX_BASEDRAWER_H__8A84267D_E359_11D2_B73E_00A0C9D5342F__INCLUDED_)
#define AFX_BASEDRAWER_H__8A84267D_E359_11D2_B73E_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseDrawer.h : header file
//

class Positioner;

/////////////////////////////////////////////////////////////////////////////
// BaseDrawer command target

class BaseDrawer : public CCmdTarget
{
protected:
	BaseDrawer();           // protected constructor used by dynamic creation

public:
  virtual ~BaseDrawer();
  CCriticalSection cs;
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop) = 0;
  virtual IlwisObject obj() const;
  virtual Domain dm() const;
  virtual Representation rpr() const;
  virtual DomainValueRangeStruct dvrs() const;
  virtual RangeReal rrStretchRange() const;
  virtual Color clrRaw(long iRaw) const;
  virtual Color clrVal(double rVal) const;
	virtual zIcon icon() const;
	virtual String sTitle() const;
  static Color _export clrPrimary(int iNr);
protected:
  CoordBounds cbRect(Positioner* psn) const;
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASEDRAWER_H__8A84267D_E359_11D2_B73E_00A0C9D5342F__INCLUDED_)
