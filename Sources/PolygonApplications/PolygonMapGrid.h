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
// PolygonMapGrid.h: interface for the PolygonMapGrid class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ILWPOLIDGRID_H
#define ILWPOLIDGRID_H

#include "Engine\Applications\POLVIRT.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Point\PNT.H"


IlwisObjectPtr * createPolygonMapGrid(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT PolygonMapGrid : public PolygonMapVirtual  
{
	friend class PolygonMapVirtual;
public:
	PolygonMapGrid(const FileName&, PolygonMapPtr& p);
	PolygonMapGrid(const FileName& fn, PolygonMapPtr& p, 
				   const CoordSystem& cs, const Domain& dm,
				   Coord crdLL,
				   double rWidth, double rHeight, 
				   long iNrRows,long iNrCols,
				   Coord crdUR,
				   Coord crdP,
				   bool fDomainExist,
					 String sTbl,
				   String sLabels,
				   String sOdLbl,
				   String sScLbl,
				   long iSnLbl,
				   long iStepsLbl,
				   String sPrefix,
				   String sPostfix);
	virtual ~PolygonMapGrid();
	static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
	static PolygonMapGrid* create(const FileName&, PolygonMapPtr& p, const String& sExpr);
  void Init();
private:
	bool m_fDomainExist;
	CoordBounds CalcGridDim(Coord& crdIncX,Coord& crdIncY,Coord& crdSc);
	void InitParam(Coord& crdIncX,Coord& crdIncY,Coord& crdSc);
	String sGetLabel(long iRow, long iCol,DomainSort* gdsrt);
	bool PutLabels(DomainSort* gdsrt);
	long m_iNrRows,m_iNrCols,m_iSnLbl,m_iStepsLbl;
	String m_sOdLbl,m_sScLbl,m_sPrefix,m_sPostfix,m_sLabels,m_sTbl;
	enum OrderLabeling {olHORZ, olHORZ_CONT, olVERT, olVERT_CONT} m_odLbl;
	enum StartCornerLabeling {slLOW_LEFT,slLOW_RIGHT,slUPPER_LEFT,slUPPER_RIGHT} m_scLbl;
	double m_rWidth, m_rHeight;
	Coord m_crdLL,m_crdUR,m_crdP;
	vector<String> m_vsDom;
};

#endif 
