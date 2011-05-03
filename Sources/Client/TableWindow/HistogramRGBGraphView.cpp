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
// HistogramRGBGraphView.cpp: implementation of the HistogramRGBGraphView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\HistogramRGBGraphView.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Client\GraphWindow\GraphLegend.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ILWIS;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HistogramRGBGraphView::HistogramRGBGraphView(SpatialDataDrawer *drw)
{
  Map mpRed, mpGreen, mpBlue;
  grdrw = cgd = new CartesianGraphDrawer(this);
  grdrw->grleg->fShow = false;
	mp[0] = mpRed;
	mp[1] = mpGreen;
	mp[2] = mpBlue;
	RangeReal rr;
	for (int i = 0; i < 3; ++i) {
		hi[i] = TableHistogramInfo(mp[i]);
		if (!hi[i].fValid())
			return;
		colPix[i] = hi[i].colNPix();
		colVal[i] = hi[i].colValue();
		if (colVal[i].fValid()) {
			cgd->gaxX->Set(colVal[i]->dvrs());
			cgd->gaxX->dvrs = colVal[i]->dvrs();
		}
		else
			cgd->gaxX->Set(hi[i].tbl()->dm());
		ColumnGraphLayer* gl = new ColumnGraphLayer(cgd, hi[i].tbl(), colVal[i], colPix[i]);
    gl->cgt = cgtContineous;
		if (i==0) 
  		gl->color = Color(255,0,0);
		else if (i == 1)
  		gl->color = Color(0, 255,0);
		else if (i == 2)
  		gl->color = Color(0, 0, 255);
		cgd->agl.push_back(gl);
 		cgd->gaxYLeft->Set(colPix[i]->dvrs());
 		cgd->gaxYLeft->dvrs = colPix[i]->dvrs();
 		cgd->gaxYRight->Set(colPix[i]->dvrs());
 		cgd->gaxYRight->dvrs = colPix[i]->dvrs();
		if (i==0)
			rr = colPix[i]->rrMinMax();
		else {
			RangeReal rrc = colPix[i]->rrMinMax();
			rr = RangeReal(min(rr.rLo(),rrc.rLo()), max(rr.rHi(), rrc.rHi())); 
		}
	}
 	cgd->gaxYLeft->SetMinMax(rr);
 	cgd->gaxYRight->SetMinMax(rr);
}

HistogramRGBGraphView::~HistogramRGBGraphView()
{
  delete cgd;
}


