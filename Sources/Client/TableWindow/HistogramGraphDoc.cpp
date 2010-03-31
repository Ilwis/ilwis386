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
// HistogramGraphDoc.cpp: implementation of the HistogramGraphDoc class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\TableWindow\HistogramGraphDoc.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphLegend.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HistogramGraphDoc::HistogramGraphDoc()
{
}

HistogramGraphDoc::~HistogramGraphDoc()
{
}

BOOL HistogramGraphDoc::OnOpenDocument(const Table& table)
{
  tbl = table;
  colX = Column();
  if (tbl->dm()->pdnone())
    colX = tbl->col("value");  
  Column colPix = tbl->col("npix");
  Column colPCumPct = tbl->col("npcumpct");

  if (!colPix.fValid())
    colPix = tbl->col("Area");
  if (!colPix.fValid())
    colPix = tbl->col("Length");
  if (!colPix.fValid())
    colPix = tbl->col("NrPnt");
  if (!colPix.fValid())
    return FALSE;

  grdr = cgd = new CartesianGraphDrawer(0); 

  ColumnGraphLayer* gl = new ColumnGraphLayer(cgd, tbl, colX, colPix);
  gl->fYAxisLeft = true;
  cgd->agl.push_back(gl);
  if (colPCumPct.fValid()) 
  {
    ColumnGraphLayer* glPct = new ColumnGraphLayer(cgd, tbl, colX, colPCumPct);
  	glPct->fYAxisLeft = false;
  	cgd->agl.push_back(glPct);
    glPct->cgt = cgtContineous;
    glPct->color = Color(255,0,0); // red
    glPct->line.clrLine() = Color(255,0,0); // red
  }

	gl->color = Color(48,128,76); // forest green
  if (colX.fValid())
  {
		gl->cgt = cgtNeedle;
	}
	else {
		gl->cgt = cgtBar;
    if (tbl->dm()->pdc()) 
    {
      gl->fRprColor = true;
      gl->rpr = tbl->dm()->rpr();
    }
	}

  // X-axis
  if (colX.fValid())
  {
		cgd->gaxX->Set(colX->dvrs());
		cgd->gaxX->dvrsData = colX->dvrs();
		if (colX->fValues()) 
		{
			RangeReal rr = colX->rrMinMax();
			double rWidth = rr.rWidth();
			rr.rHi() += rWidth * 0.03;
			if (rr.rLo() >= 0 && rr.rLo() < rWidth * 0.3)
				rr.rLo() = 0;
			else 
				rr.rLo() -= rWidth * 0.05;
			cgd->gaxX->SetMinMax(rr);
		}
		cgd->gaxX->sTitle = colX->sName();
  }
	else {
		cgd->gaxX->Set(tbl->dm());
		if (tbl->dm()->pdnone())
			cgd->gaxX->SetMinMax(RangeReal(1, tbl->iRecs()));
	}
  // Y-axis
	cgd->gaxYLeft->Set(colPix->dvrs());
	cgd->gaxYLeft->dvrsData = colPix->dvrs();
 	cgd->gaxYRight->Set(ValueRange(0,100));
  cgd->gaxYRight->dvrsData = ValueRange(0,100);
	RangeReal rr = colPix->rrMinMax();
	double rWidth = rr.rWidth();
	rr.rHi() += rWidth * 0.03;
	rr.rLo() = 0;
	cgd->gaxYLeft->SetMinMax(rr);
	cgd->gaxYRight->SetMinMax(RangeReal(0,100));
	cgd->gaxYLeft->sTitle = colPix->sDescription;
  grdr->sTitle = "";
  grdr->grleg->fShow = false;
  return TRUE;
}
