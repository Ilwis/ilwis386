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
/* PrincComp, PrincCompPtr
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind
	Last change:  WK   12 Aug 98   12:22 pm
*/

#ifndef ILWPRINCCOMP_H
#define ILWPRINCCOMP_H

#if !defined(AFX_MATRIXOBJECTVIRTUAL_H__C7EBC669_68F0_4D16_A382_795582137202__INCLUDED_)
#include "Engine\Matrix\MatrixObjectVirtual.h"
#endif

#ifndef ILWMAPLIST_H
#include "Engine\Map\Raster\MapList\maplist.h"
#endif


class PrincCompPtr: public MatrixObjectVirtual
{
	friend class MatrixObjectPtr;
public:
	virtual ~PrincCompPtr();
	virtual void Store();
	virtual String sType() const;
	virtual String sExpression() const;
	virtual void GetRowText(Array<String>& as) const;
	virtual void GetColumnText(Array<String>& as) const;
	String sSummary() const;

	virtual void Freeze();
	virtual void GetObjectDependencies(Array<FileName>& afnObjDep);
	virtual int iWidth() const;
	virtual String sValue(int iRow, int iCol, int iWidth) const;
protected:
	PrincCompPtr(const FileName&, MatrixObjectPtr& ptr);
private:  
	static PrincCompPtr* create(const FileName&, MatrixObjectPtr& ptr, const String& sExpr);
	PrincCompPtr(const FileName& fn, MatrixObjectPtr& ptr, const MapList& maplist, bool fFacAnal, int iOutBands=iUNDEF);
	
	MapList         m_mpl;
	ObjectTime            m_tmCalc;
	CVector         m_vecEigVal;
	void CalcEigen(RealMatrix*);
	bool            m_fFactorAnal;
	int             m_iOutBands;
	long            m_iInputBands;
	Array<String>   m_asInputBandNames;
	FileName        m_fnMpl;
};

#endif




