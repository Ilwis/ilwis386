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
// PolygonMapGrid.cpp: implementation of the PolygonMapGrid class.
//
//////////////////////////////////////////////////////////////////////

#include "PolygonApplications\PolygonMapGrid.h"
#include "Engine\Table\tbl.h"
#include "Engine\Domain\dmsort.h"
//#include "Engine\Base\System\commandhandler.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\polygon.hs"
#include <math.h>

IlwisObjectPtr * createPolygonMapGrid(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapGrid::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapGrid(fn, (PolygonMapPtr &)ptr);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PolygonMapGrid::PolygonMapGrid(const FileName& fn, PolygonMapPtr& p)
	:PolygonMapVirtual(fn, p)					
{ 
	ReadElement("PolygonMapGrid", "Origin_X", m_crdLL.x);
	ReadElement("PolygonMapGrid", "Origin_Y", m_crdLL.y);
	ReadElement("PolygonMapGrid", "Direction_Point_X", m_crdP.x);
	ReadElement("PolygonMapGrid", "Direction_Point_Y", m_crdP.y);
	ReadElement("PolygonMapGrid", "Grid_Width", m_rWidth);
	ReadElement("PolygonMapGrid", "Grid_Height", m_rHeight);
	ReadElement("PolygonMapGrid", "Nr_Vertical_Grids", m_iNrRows);
	ReadElement("PolygonMapGrid", "Nr_Horizontal_Grids", m_iNrCols);
	ReadElement("PolygonMapGrid", "Opposite_X", m_crdUR.x);
	ReadElement("PolygonMapGrid", "Opposite_Y", m_crdUR.y);
	ReadElement("PolygonMapGrid", "ExistingDomain", m_fDomainExist);
	ReadElement("PolygonMapGrid", "AttributeTable", m_sTbl);
	ReadElement("PolygonMapGrid", "LabelPoints", m_sLabels);
	ReadElement("PolygonMapGrid", "Order_Labeling", m_sOdLbl);
	ReadElement("PolygonMapGrid", "Start_Corner_Labeling", m_sScLbl); 
	ReadElement("PolygonMapGrid", "Start_Number_Labeling", m_iSnLbl); 
	ReadElement("PolygonMapGrid", "Steps_Labeling", m_iStepsLbl); 
	ReadElement("PolygonMapGrid", "Prefix_Labeling", m_sPrefix); 
	ReadElement("PolygonMapGrid", "Postfix_Labeling", m_sPostfix); 
	Init();
	
}

PolygonMapGrid::PolygonMapGrid(const FileName& fn, PolygonMapPtr& p
							   ,const CoordSystem& cs, const Domain& dm,
							    Coord crdLL,
							    double rWidth, double rHeight, 
								long iNrRows,long iNrCols,
								Coord crdUR,
								Coord crdP,
								bool _fDomainExist,
								String sTbl,
								String sLabels,
								String sOdLbl,
								String sScLbl,
								long iSnLbl,
								long iStepsLbl,
								String sPrefix,
								String sPostfix)
	:PolygonMapVirtual(fn, p,cs,cs->cb, dm),
	m_crdLL(crdLL),
	m_rWidth(rWidth),m_rHeight(rHeight),
	m_iNrRows(iNrRows),m_iNrCols(iNrCols),
	m_crdUR(crdUR),
	m_crdP(crdP),
	m_fDomainExist(_fDomainExist), 
	m_sTbl(sTbl),
	m_sLabels(sLabels), 
	m_sOdLbl(sOdLbl),
	m_sScLbl(sScLbl),
	m_iSnLbl(iSnLbl),
	m_iStepsLbl(iStepsLbl),
	m_sPrefix(sPrefix),
	m_sPostfix(sPostfix)
{
	Init();
	if (!fnObj.fValid()) // 'inline' object
		objtime = objdep.tmNewest();
	objdep.Add(cs);
	if (_fDomainExist)
	{
		Table tbl(m_sTbl);
				if (tbl.fValid())
					p.SetAttributeTable(tbl);
				
	}	
	
	Store();
}

PolygonMapGrid::~PolygonMapGrid()
{

}

PolygonMapGrid* PolygonMapGrid::create(const FileName& fn, PolygonMapPtr& p,const String& sExpr)
{
	Array<String> as;
	// remove optional comma's at the end of the command string
	// this is needed because of a bug in iParseParam, which cannot handle
	// multiple commas at the end of an expression
	size_t i = sExpr.size() - 1;
	while (i > 0 && sExpr[i-1] == ',') --i;
	String sExp = sExpr.substr(0, i);
	sExp &= ')';
	
	long iParms = IlwisObjectPtr::iParseParm(sExp, as);
	iParms = (long)as.size();
	if (iParms < 7) //Requiring at least 7 parameters in the routine  
		throw ErrorExpression(SPOLErrParameters, sSyntax());
	
	FileName fnCSY(as[0],".csy");
	as[0] = fnCSY.sRelativeQuoted(false,fn.sPath());
	CoordSystem csy(as[0]);
	if (!csy.fValid())
	{
		throw ErrorExpression(String(SPOLErrCouldNotFind_S.scVal(), as[0]), sSyntax());
	}

	Domain dm;
	bool fDomainExist = false;
	String sTbl;
		
	if ((iParms > 11) && (as[11] != ""))
	{
			if (File::fExist(FileName(as[11], ".tbt")))
			{		
				//Table tbl(as[11],fn.sPath());
				Table tbl(as[11]);
				if (tbl.fValid())
				{
					dm = tbl->dm();
					if (dm.fValid()  && dm->pdsrt() != 0 )
						fDomainExist = dm.fValid();
					else
						throw ErrorExpression(String(SPOLErrDomainTypeInValid.scVal(), as[11]), sSyntax());
					sTbl = as[11].sVal();
					FileName fnTbl(sTbl,".tbt");
					sTbl = fnTbl.sRelativeQuoted(false,fn.sPath());
				}
			}
			else
				throw ErrorExpression(String(SPOLErrCouldNotFindTable_S.scVal(), as[11]), sSyntax());
	}
	
	long iNrRows, iNrCols;	
	double rWidth, rHeight;
	Coord crdLL,crdUR,crdP;
	
	rWidth =  rHeight = rUNDEF;
	crdUR.x =  crdUR.y = rUNDEF;
	iNrRows = iNrCols = iUNDEF;
	
	if ((as[1].rVal() != rUNDEF) && (as[2].rVal() != rUNDEF)){
		crdLL.x  = as[1].rVal();
		crdLL.y  = as[2].rVal();
	}	
	else
		throw ErrorExpression(SPOLErrDefineOrigin, sSyntax()); 

	if ((iParms > 10) && (as[9] != "") && (as[10] != "")){
		crdP.x = as[9].rVal();
		crdP.y = as[10].rVal();
	}	
	else{
		crdP.x = crdLL.x;
		crdP.y = crdLL.y;
	}

	//Direction point is on the positive Y-axis from origin(most left point) 
	if ((crdP.x < crdLL.x) || (crdP.y < crdLL.y)){
		throw ErrorExpression(SPOLErrDirectionPoint, sSyntax());
	}
		
	if (( as[3].rVal()  != rUNDEF)  && ( as[3].rVal()  != 0)  &&
		( as[4].rVal()  != rUNDEF)  && ( as[4].rVal()  != 0)  &&
		( as[5].iVal()  != iUNDEF) &&   
		( as[6].iVal()  != iUNDEF) )
		{
			rWidth = as[3].sTrimSpaces().rVal();
			rHeight = as[4].sTrimSpaces().rVal();
			iNrRows = as[5].sTrimSpaces().iVal();
			iNrCols = as[6].sTrimSpaces().iVal();
	} 
	
	else if (( iParms > 8 ) && 
		    (as[5].iVal() != iUNDEF)  && (as[5].iVal() != 0)  &&
			(as[6].iVal() != iUNDEF)  && (as[6].iVal() != 0)  &&
			(as[7].rVal() != rUNDEF) &&
			(as[8].rVal() != rUNDEF)){
			iNrRows = as[5].sTrimSpaces().iVal();
			iNrCols = as[6].sTrimSpaces().iVal();
			crdUR.x = as[7].sTrimSpaces().rVal();
			crdUR.y = as[8].sTrimSpaces().rVal();
			if (crdUR.x < crdLL.x)
					throw ErrorExpression(SPOLErrOppositePoint, sSyntax());
	
	}

	else if ( (iParms > 8 ) && 
			( as[3].rVal() != rUNDEF) && ( as[3].rVal() >0) &&
			( as[4].rVal() != rUNDEF) && ( as[4].rVal() >0) &&
			( as[7].rVal() != rUNDEF) &&
			( as[8].rVal() != rUNDEF)){
			rWidth = as[3].sTrimSpaces().rVal();
			rHeight = as[4].sTrimSpaces().rVal();
			crdUR.x = as[7].sTrimSpaces().rVal();
			crdUR.y = as[8].sTrimSpaces().rVal();
			if (crdUR.x < crdLL.x)
					throw ErrorExpression(SPOLErrOppositePoint, sSyntax());
	}
	else
			throw ErrorExpression(SPOLErrParameters, sSyntax());
	
	String sOdLbl,sLabels,sScLbl;
	if ((iParms > 12) && (fCIStrEqual(as[12], "LabelPoints")) )
		sLabels = "LabelPoints";
	else
		sLabels = "NoLabelPoints";
	
	if (iParms > 13)
		if (fCIStrEqual(as[13], "H") || 		
			fCIStrEqual(as[13], "HC") ||
			fCIStrEqual(as[13], "V") ||
			fCIStrEqual(as[13], "VC"))
			sOdLbl = as[13].sVal();
		else 
			sOdLbl = "H";
	else
		sOdLbl = "H";
    
	if (iParms > 14)
		if (fCIStrEqual(as[14], "LL") || 
			fCIStrEqual(as[14], "LR") ||
			fCIStrEqual(as[14], "UL") ||
			fCIStrEqual(as[14], "UR")) 
				sScLbl = as[14].sVal();
		else 
				sScLbl = "LL";
	else
		sScLbl = "LL";  

	long iSnLbl;	  
	if (iParms > 15)
		if (as[15].iVal() != iUNDEF)
			iSnLbl = as[15].iVal();
		else
			iSnLbl = 1;
	else
		iSnLbl = 1;  

	long iStepsLbl;
	if (iParms > 16)
		if (as[16].iVal() != iUNDEF)
			iStepsLbl = as[16].iVal();
		else
			iStepsLbl = 1;
	else
		iStepsLbl = 1;

	String sPrefix,sPostfix; 
	if (iParms > 17)
		sPrefix = as[17].sVal();
	else
  		sPrefix = "";

	if (iParms > 18)
		sPostfix = as[18].sVal();
	else
  		sPostfix = "";

	if (!fDomainExist)
	{
		FileName fnDom(fn, ".dom");
		//if (File::fExist(fnDom))
		//{
			//Domain dm(fnDom);
			//dm->fErase = true;
			//dm.SetPointer(0);
		//}
		fnDom = FileName::fnUnique(fnDom);
		dm = Domain(fnDom, 0, dmtID);
		DomainSort *pds = dm->pdsrt();
		pds->dsType = DomainSort::dsMANUAL;
	}

	return new PolygonMapGrid(fn , p, csy, dm,
							  crdLL,
							  rWidth,rHeight,
							  iNrRows,iNrCols,
							  crdUR,
							  crdP,
							  fDomainExist,
								sTbl,
							  sLabels,
							  sOdLbl,
							  sScLbl,
							  iSnLbl,
							  iStepsLbl,
							  sPrefix,
							  sPostfix);
  
}

void PolygonMapGrid::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapGrid");
  WriteElement("PolygonMapGrid", "Origin_X", m_crdLL.x);
  WriteElement("PolygonMapGrid", "Origin_Y", m_crdLL.y);
  if (m_rWidth != rUNDEF) WriteElement("PolygonMapGrid", "Grid_Width", m_rWidth);	
  if (m_rHeight != rUNDEF) WriteElement("PolygonMapGrid", "Grid_Height", m_rHeight);	
  if (m_iNrRows != iUNDEF) WriteElement("PolygonMapGrid", "Nr_Vertical_Grids", m_iNrRows);	
  if (m_iNrCols != iUNDEF) WriteElement("PolygonMapGrid", "Nr_Horizontal_Grids", m_iNrCols);	
  WriteElement("PolygonMapGrid", "Opposite_X", m_crdUR.x);	
  WriteElement("PolygonMapGrid", "Opposite_Y", m_crdUR.y);
  WriteElement("PolygonMapGrid", "Direction_Point_X", m_crdP.x);
  WriteElement("PolygonMapGrid", "Direction_Point_Y", m_crdP.y);
  WriteElement("PolygonMapGrid", "ExistingDomain", m_fDomainExist);
	if (m_sTbl.length()	> 0)
		WriteElement("PolygonMapGrid", "AttributeTable", m_sTbl);
	WriteElement("PolygonMapGrid", "LabelPoints", m_sLabels);	
  WriteElement("PolygonMapGrid", "Order_Labeling", m_sOdLbl);  
  WriteElement("PolygonMapGrid", "Start_Corner_Labeling",m_sScLbl);     
  WriteElement("PolygonMapGrid", "Start_Number_Labeling", m_iSnLbl);
  WriteElement("PolygonMapGrid", "Steps_Labeling", m_iStepsLbl);  
	if (m_sPrefix.length() > 0)
		WriteElement("PolygonMapGrid", "Prefix_Labeling", String("\"%S\"", m_sPrefix));
	if (m_sPostfix.length() > 0)
		WriteElement("PolygonMapGrid", "Postfix_Labeling", String("\"%S\"", m_sPostfix));
}  

void PolygonMapGrid::Init()
{
  fNeedFreeze = true;
  sFreezeTitle = "PolygonMapGrid";
	htpFreeze = htpPolygonMapGridT;
}

String PolygonMapGrid::sExpression() const
{
  
  String sWidth  = (m_rWidth <= 0) ? "" : String("%g",m_rWidth);
	String sHeight = (m_rHeight <= 0) ? "" : String("%g",m_rHeight);
	String sNrRows = (m_iNrRows <= 0) ? "" : String("%li",m_iNrRows);
	String sNrCols = (m_iNrCols <= 0) ? "" : String("%li",m_iNrCols);
	String sCrdURX = (m_crdUR.x == rUNDEF) ? "" : String("%g",m_crdUR.x);
  String sCrdURY = (m_crdUR.y == rUNDEF) ? "" : String("%g",m_crdUR.y);
	
	String	sTemp = String("PolygonMapGrid(%S,%g,%g,%S,%S,%S,%S,%S,%S,%g,%g,%S,%S,%S,%S,%li,%li,%S,%S)", 
		                          cs()->sNameQuoted(true),
									m_crdLL.x,m_crdLL.y,
									sWidth,sHeight,
									sNrRows,sNrCols,
									sCrdURX,sCrdURY,
									m_crdP.x, m_crdP.y,
									FileName(m_sTbl).sRelativeQuoted(),
									m_sLabels,m_sOdLbl,m_sScLbl,m_iSnLbl,m_iStepsLbl,m_sPrefix.sQuote(),m_sPostfix.sQuote());
	
	return sTemp;
	
}

bool PolygonMapGrid::fFreezing() 
{
	Coord crdIncX,crdIncY,crdSc;
	CoordBounds cbMap = CalcGridDim(crdIncX,crdIncY,crdSc);
	InitParam(crdIncX,crdIncY,crdSc);
	
	CoordBuf cBuf(5);
	Coord cNodeX, cNodeY;

	trq.SetTitle(sFreezeTitle);
	trq.SetText(SPOLTextCalculatingGridCells);
	trq.Start();

	DomainSort* gdsrt = dm()->pdsrt();
	if (!m_fDomainExist)
		if (!PutLabels(gdsrt))
			return false;
	
	PointMap pMap = PointMap( fnObj, cs(), cb(), dm());
	pMap->fErase = true;
	pMap->SetCoordBounds (cbMap);
	pMap->iAdd(m_iNrRows * m_iNrCols);
	ptr.SetCoordBounds(cbMap);
	
	cNodeY.x = crdSc.x;
	cNodeY.y = crdSc.y;
	for(long iRow=0; iRow<m_iNrRows ; ++iRow)
	{
		if (trq.fUpdate(iRow * m_iNrCols, m_iNrRows * m_iNrCols))
			return false;
		
		cNodeX.x = cNodeY.x;
		cNodeX.y = cNodeY.y;
		for(long iCol= 0; iCol < m_iNrCols; ++iCol)  
		{
			if (trq.fUpdate(iRow * m_iNrCols + iCol, m_iNrRows* m_iNrCols))
				return false;

			ILWIS::Polygon *pol = CPOLYGON(pms->newFeature());
			cBuf[0] = Coord(cNodeX.x,cNodeX.y)  ;
			cBuf[1] = Coord(cNodeX.x+crdIncX.x,cNodeX.y);
			cBuf[2]	= Coord(cNodeX.x+crdIncX.x,cNodeX.y + crdIncX.y);
			cBuf[3] = Coord(cNodeX.x,cNodeX.y + crdIncX.y);
			pol->addBoundary(new LinearRing(cBuf.clone(), new GeometryFactory()));
			pol->PutVal(iCol + iRow * m_iNrCols);
			Coord cLbl(cNodeX.x+crdIncY.x/2+crdIncX.x /2,
			           cNodeX.y+crdIncY.y/2+crdIncX.y/2);
			String sLbl = sGetLabel(iRow, iCol,gdsrt);	
			pMap->PutVal(iRow * m_iNrCols + iCol + 1, cLbl);
			pMap->PutVal(iRow * m_iNrCols + iCol + 1, sLbl);
//			pMap->iAddVal(cLbl, sLbl); //label point map
			cNodeX.x += crdIncX.x;
			cNodeX.y += crdIncX.y;
		}
		cNodeY.x += crdIncY.x;
		cNodeY.y += crdIncY.y;

	}
	
	trq.fUpdate(m_iNrRows, m_iNrRows);
		
	if (fCIStrEqual(m_sLabels, "LabelPoints") )
		pMap->fErase = false;
	
	return true;
}

const char* PolygonMapGrid::sSyntax() 
{
  char* sSt = "PolygonMapGrid(Input_coord_system,\n"
														"Origin_X,Origin_Y,\n"
														"Grid_Width,Grid_Height,\n"
														"Nr_Vertical_Grids,Nr_Vertical_Grids,\n"
														"[Opposite_X],[Opposite_Y],\n"
														"[Direction_Point_X],[Direction_Point_Y],\n"
														"[ExistingTable],\n"
														"[LabelPoints],\n"
														"[Order_Labeling],\n"
														"[Start_Corner_Labeling],\n"
														"[Start_Number_Labeling],\n"
														"[Steps_Labeling],\n"
														"[Prefix_Labeling],\n"
														"Postfix_Labeling\n";
	return sSt;
}


CoordBounds PolygonMapGrid::CalcGridDim(Coord &crdIncX, Coord &crdIncY, Coord &crdSc)
{
	double rAngle;
	Coord crdLR,crdUL;

	if ((m_crdLL.x  != rUNDEF) &&
		(m_crdLL.x  != rUNDEF) &&
		(m_crdP.x  != rUNDEF) && 
		(m_crdP.y != rUNDEF) &&
		(m_crdP.y-m_crdLL.y != 0))
	{
				rAngle = atan((m_crdP.x-m_crdLL.x)/ (m_crdP.y-m_crdLL.y));
				if (abs(rAngle - (M_PI / 4)) < 0.00001) 
					rAngle += 0.00001;  // Small correction needed, because the Point in Polygon routine cannot
		                      // handle exactly 45 degrees properly
	}		
	else 
				rAngle = 0;	

	
	if (( m_rWidth  != rUNDEF) && 
		( m_rHeight  != rUNDEF) &&
		( m_iNrRows  != iUNDEF) &&
		( m_iNrCols  != iUNDEF)) 

	{
		//double rR = (rAngle*M_PI)/180;
		crdIncY = Coord(m_rHeight * sin(rAngle),m_rHeight * cos(rAngle));
	  crdIncX = Coord(m_rWidth * cos(rAngle),m_rWidth * sin(rAngle));
		crdUL = Coord(m_crdLL.x+m_iNrRows * crdIncY.x, m_crdLL.y+m_iNrRows*crdIncY.y);
		m_crdUR = Coord(crdUL.x+m_iNrCols*crdIncX.x, crdUL.y-m_iNrCols*crdIncX.y);
		
	}
	else if (( m_crdUR.x  != rUNDEF) && ( m_crdUR.y  != rUNDEF)){
	
		Coord crdP0 = Coord(m_crdUR.x - m_crdLL.x, m_crdUR.y - m_crdLL.y );
		Coord crdD = Coord(m_crdP.x - m_crdLL.x, m_crdP.y - m_crdLL.y );
		double rT;
		if (abs(crdD.y) < 0.00001 && abs(crdD.x) < 0.00001 )
				crdD.y += 1;
		rT = (crdP0.y * crdD.x - crdP0.x * crdD.y ) / (crdD.y * crdD.y + crdD.x * crdD.x);   

		crdUL = Coord(rT*crdD.y + m_crdUR.x, -(rT*crdD.x) + m_crdUR.y);

		if ((m_rWidth != rUNDEF) && (m_rHeight != rUNDEF))
		{
			crdIncY = Coord(m_rHeight * sin(rAngle),m_rHeight * cos(rAngle));
			crdIncX = Coord(m_rWidth * cos(rAngle),m_rWidth * sin(rAngle));
			m_iNrRows = (long)((crdUL.y - m_crdLL.y)/crdIncY.y);
			m_iNrCols = (long)((m_crdUR.x - crdUL.x)/crdIncX.x); 
		}
		else if ((m_iNrCols != iUNDEF) && (m_iNrRows != iUNDEF))
		{
			crdIncY = Coord(abs(crdUL.x - m_crdLL.x)/m_iNrRows,abs(crdUL.y - m_crdLL.y)/m_iNrRows);
			crdIncX = Coord(abs(m_crdUR.x - crdUL.x)/m_iNrCols,abs(m_crdUR.y - crdUL.y)/m_iNrCols);
			m_rHeight = crdIncY.y/cos(rAngle);
			m_rWidth	= crdIncX.x/cos(rAngle);
		}
	}		
	if (m_iNrRows == 0) m_iNrRows++;
	if (m_iNrCols == 0) m_iNrCols++; 

	crdLR = Coord(m_crdLL.x+m_iNrCols*crdIncX.x, m_crdLL.y-m_iNrCols*crdIncX.y);

	if (fCIStrEqual(m_sOdLbl.toUpper(), "H") )
	 m_odLbl = olHORZ;
	else if (fCIStrEqual(m_sOdLbl.toUpper(), "HC") )
	  m_odLbl = olHORZ_CONT;
	else if (fCIStrEqual(m_sOdLbl.toUpper(), "V") )
	  m_odLbl = olVERT;
	else if (fCIStrEqual(m_sOdLbl.toUpper(), "VC") )
	  m_odLbl = olVERT_CONT;
	else 
	  m_odLbl = olHORZ;

	if (fCIStrEqual(m_sScLbl.toUpper(), "LL") )
	{
		m_scLbl = slLOW_LEFT;
		crdSc = m_crdLL; 
	}	
	else if (fCIStrEqual(m_sScLbl.toUpper(), "LR"))
	{
		m_scLbl = slLOW_RIGHT;
		crdSc = crdLR; 
	}	
	else if (fCIStrEqual(m_sScLbl.toUpper(), "UL"))
	{
		m_scLbl = slUPPER_LEFT;
		crdSc = crdUL; 
	}	
	else if (fCIStrEqual(m_sScLbl.toUpper(), "UR") )
	{
		m_scLbl = slUPPER_RIGHT;
		crdSc = m_crdUR; 
	}	
	else
	{
		m_scLbl	= slLOW_LEFT;
		crdSc = m_crdLL; 
	}

		CoordBounds cb;
		cb += m_crdLL;
		cb += crdLR;
		cb += crdUL;
		cb += m_crdUR;

		cb += Coord(cb.MinX() - cb.width() / 20, cb.MinY() - cb.height() / 20);
		cb += Coord(cb.MaxX() + cb.width() / 20, cb.MaxY() + cb.height() / 20);
    return cb;
}

void PolygonMapGrid::InitParam(Coord &crdIncX,Coord &crdIncY,Coord &crdSc)
{
	long iTmpRC;
	Coord crdTmpIncX;
	switch (m_scLbl)
	{
	  case slLOW_LEFT:
		switch (m_odLbl)
		{
			case olHORZ: case olHORZ_CONT:{
				crdIncX.y = -crdIncX.y;  
				
			}	
			break;
			case olVERT: case olVERT_CONT:{
				iTmpRC = m_iNrCols;
				m_iNrCols = m_iNrRows;
				m_iNrRows = iTmpRC;
				crdTmpIncX = crdIncY;
				crdIncY = Coord(crdIncX.x,-crdIncX.y);
				crdIncX = Coord(crdTmpIncX.x,crdTmpIncX.y);
			}
		}
		break;
	  case slLOW_RIGHT:
		switch (m_odLbl)
		{
			case olHORZ: case olHORZ_CONT:{
				crdIncX.x = -crdIncX.x;  
			}
			break;
			case olVERT: case olVERT_CONT:{
				iTmpRC = m_iNrCols;
				m_iNrCols = m_iNrRows;
				m_iNrRows = iTmpRC;
				crdTmpIncX = crdIncY;
				crdIncY = Coord(-crdIncX.x,crdIncX.y);
				crdIncX = Coord(crdTmpIncX.x,crdTmpIncX.y);
			}	
		}
		break;
	  case slUPPER_LEFT:
		switch (m_odLbl)
		{
			case olHORZ: case olHORZ_CONT:{
				crdIncX = Coord(crdIncX.x,-crdIncX.y);  
				crdIncY = Coord(-crdIncY.x,-crdIncY.y);  
			}
			break;
			case olVERT: case olVERT_CONT:{
				iTmpRC = m_iNrCols;
				m_iNrCols = m_iNrRows;
				m_iNrRows = iTmpRC;
				crdTmpIncX = Coord(crdIncY.x,crdIncY.y);
				crdIncY = Coord(crdIncX.x,-crdIncX.y);
				crdIncX = Coord(-crdTmpIncX.x,-crdTmpIncX.y);
			}	
		}
		break;
	  case slUPPER_RIGHT:
		switch (m_odLbl)
		{
			case olHORZ: case olHORZ_CONT:{
				crdIncX = Coord(-crdIncX.x,crdIncX.y);  
				crdIncY = Coord(-crdIncY.x,-crdIncY.y);  
			}
			break;	
			case olVERT: case olVERT_CONT:{
				iTmpRC = m_iNrCols;
				m_iNrCols = m_iNrRows;
				m_iNrRows = iTmpRC;
				crdTmpIncX = Coord(crdIncY.x,crdIncY.y);
				crdIncY = Coord(-crdIncX.x,crdIncX.y);
				crdIncX = Coord(-crdTmpIncX.x,-crdTmpIncX.y);
			}	
			break;
		}

	}
}
String PolygonMapGrid::sGetLabel(long iRow, long iCol, DomainSort* gdsrt)
{
	long iFirstLbl, iLbl;
	String sLbl;
	iFirstLbl = m_iSnLbl + iRow * m_iStepsLbl * m_iNrCols; 
	iLbl = iFirstLbl+iCol* m_iStepsLbl;
	switch (m_odLbl)
	{
		case olHORZ_CONT: case olVERT_CONT:{
		    if (!(iRow % 2 == 0)){
					iFirstLbl = m_iSnLbl + iRow * m_iStepsLbl * m_iNrCols + m_iStepsLbl * (m_iNrCols-1); 
					iLbl = iFirstLbl - iCol* m_iStepsLbl;
				}
											}
	}
	
	sLbl = String("%S%d%S", m_sPrefix, iLbl, m_sPostfix);
		// search labels in domain
		
	long iRaw = gdsrt->iOrd(sLbl);
	if (iRaw == iUNDEF)
			sLbl = sUNDEF;
	return sLbl;
}

bool PolygonMapGrid::PutLabels(DomainSort* gdsrt)
{	
	long iRow, iCol = 0;
	for(iRow=0; iRow<m_iNrRows ; ++iRow)
	{
			if (trq.fUpdate(iRow * m_iNrCols + iCol, m_iNrRows* m_iNrCols))
					return false;
		
			for(iCol= 0; iCol < m_iNrCols; ++iCol)
			{	
				long iFirstLbl = m_iSnLbl + iRow * m_iStepsLbl * m_iNrCols; 
				long iLbl = iFirstLbl+iCol* m_iStepsLbl;
				String sLbl = String("%S%d%S", m_sPrefix, iLbl, m_sPostfix);
				m_vsDom.push_back(sLbl);
			}	
	}
	gdsrt->AddValues(m_vsDom);
	return true;
}
