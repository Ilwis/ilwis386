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
                                                              

//#include <geos/algorithm/Union.h>
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "PointApplications\PointMapTripleCollocation.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\Point.hs"

IlwisObjectPtr * createPointMapTripleCollocation(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapTripleCollocation::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapTripleCollocation(fn, (PointMapPtr &)ptr);
}

const char* PointMapTripleCollocation::sSyntax() {
  return "PointMapTripleCollocation(inputmaplist1, inputmaplist2, pointmap)";
}

PointMapTripleCollocation* PointMapTripleCollocation::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
	Array<String> as(3);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());
	String sInputMapList1 = as[0];
	String sInputMapList2 = as[1];
	FileName fnInput(sInputMapList1);
	MapList mlist1(fnInput);
	fnInput = FileName(sInputMapList2);
	MapList mlist2(fnInput);
	String sInputPointMap(as[2]);
	FileName fnPmap(sInputPointMap);
	PointMap pmap(fnPmap);
    return new PointMapTripleCollocation(fn, p, mlist1, mlist2, pmap);
}

PointMapTripleCollocation::PointMapTripleCollocation(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	FileName fn;
    ReadElement("PointMapTripleCollocation", "InputMapList1", fn);
	maplist1 = MapList(fn);
    ReadElement("PointMapTripleCollocation", "InputMapList2", fn);
	maplist2 = MapList(fn);
	ReadElement("PointMapTripleCollocation", "PointMap", fn);
	pointmap = PointMap(fn);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(maplist1.ptr());
  objdep.Add(maplist2.ptr());
  objdep.Add(pointmap.ptr());
}

PointMapTripleCollocation::PointMapTripleCollocation(const FileName& fn, PointMapPtr& p, const MapList& mlist1, const MapList& mlist2, const PointMap& pmap)
: PointMapVirtual(fn, p, pmap->cs(), pmap->cb(), pmap->dm()),
	maplist1(mlist1),
	maplist2(mlist2),
	pointmap(pmap)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(maplist1);
  objdep.Add(maplist2);
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  FileName fnTable(ptr.fnObj,".tbt");
   Table tbl;
  //if ( pmap->fTblAtt()) {
	//Table tblatt = pmap->tblAtt();
	//String cmd("copy %S %S;", tblatt->fnObj.sRelativeQuoted(), fnTable.sRelativeQuoted());
	//getEngine()->Execute(cmd);
	//tbl = Table(fnTable);
  //} else {
    tbl = Table(fnTable, ptr.dm());
  //}
  Column colW1(tbl,"w1",ValueRange(-9999999.0,9999999.0,0));
  Column colW2(tbl,"w2",ValueRange(-9999999.0,9999999.0,0));
  Column colW3(tbl,"w3",ValueRange(-9999999.0,9999999.0,0));
  Column colR1(tbl,"r1",ValueRange(0,10,1));
  Column colR2(tbl,"r2",ValueRange(0,10,1));
  Column colR3(tbl,"r3",ValueRange(0,10,1));
  Column colRhosq1(tbl,"rhosq1",ValueRange(-9999999.0,9999999.0,0));
  Column colRhosq2(tbl,"rhosq2",ValueRange(-9999999.0,9999999.0,0));
  Column colRhosq3(tbl,"rhosq3",ValueRange(-9999999.0,9999999.0,0));
  Column colErrVar1(tbl,"errvar1",ValueRange(-9999999.0,9999999.0,0));
  Column colErrVar2(tbl,"errvar2",ValueRange(-9999999.0,9999999.0,0));
  Column colErrVar3(tbl,"errvar3",ValueRange(-9999999.0,9999999.0,0));
  tbl->AddCol(colW1);
  tbl->AddCol(colW2);
  tbl->AddCol(colW3);
  tbl->AddCol(colR1);
  tbl->AddCol(colR2);
  tbl->AddCol(colR3);
  tbl->AddCol(colRhosq1);
  tbl->AddCol(colRhosq2);
  tbl->AddCol(colRhosq3);
  tbl->AddCol(colErrVar1);
  tbl->AddCol(colErrVar2);
  tbl->AddCol(colErrVar3);
  SetAttributeTable(tbl); 
}

void PointMapTripleCollocation::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapTripleCollocation");
  WriteElement("PointMapTripleCollocation", "InputMapList1", maplist1->fnObj);
  WriteElement("PointMapTripleCollocation", "InputMapList2", maplist2->fnObj);
  WriteElement("PointMapTripleCollocation", "PointMap", pointmap->fnObj);
}

PointMapTripleCollocation::~PointMapTripleCollocation()
{
}

String PointMapTripleCollocation::sExpression() const
{
	return String("PointMapTripleCollocation(%S,%S,%S)", maplist1->fnObj.sRelativeQuoted(false), maplist2->fnObj.sRelativeQuoted(false), pointmap->fnObj.sRelativeQuoted(false));
}

bool PointMapTripleCollocation::fDomainChangeable() const
{
  return false;
}

void PointMapTripleCollocation::Init()
{
  sFreezeTitle = "PointMapTripleCollocation";
}

bool PointMapTripleCollocation::fFreezing()
{
	trq.SetText(TR("Performing triple collocation on triple sensor inputs"));
	Table tblAtt = ptr.tblAtt();
	if ( maplist1->iSize() == 0 || maplist2->iSize() == 0 || pointmap->iFeatures() == 0 || !pointmap->fTblAtt())
		return true;
	Table tblAttInput = pointmap->tblAtt();
	long iNrBands = min(maplist1->iSize(), maplist2->iSize());
	int iCols = tblAttInput->iCols();
	if (iCols < iNrBands)
		return true;

	bool fCoordConvert1 = maplist1->map(0)->cs() != pointmap->cs();
	bool fCoordConvert2 = maplist2->map(0)->cs() != pointmap->cs();
	if ( tblAtt->iRecs() < pointmap->iFeatures())
		tblAtt->iRecNew(pointmap->iFeatures() - tblAtt->iRecs());

	for(int f = 0; f < pointmap->iFeatures(); ++f) {
		ILWIS::Point *p = CPOINT(pointmap->getFeature(f));
		if ( !p  || !p->fValid())
			continue;
		ILWIS::Point *newPoint = CPOINT(ptr.newFeature(p));
		newPoint->PutVal(p->rValue());
		Coord c1(*(newPoint->getCoordinate()));
		if (fCoordConvert1)
			c1 = maplist1->map(0)->cs()->cConv(pointmap->cs(), c1);
		Coord c2(c1);
		if (fCoordConvert2)
			c2 = maplist2->map(0)->cs()->cConv(pointmap->cs(), c2);
		RealMatrix matrix (iNrBands, 3);
		for(int i=0; i < iNrBands; ++i) {
			Map mp1 = maplist1->map(i);
			Map mp2 = maplist2->map(i);
			Column col = tblAttInput->col(iCols - iNrBands + i);
			double rVal1 = mp1->rValue(c1);
			double rVal2 = mp2->rValue(c2);
			double rVal3 = col->rValue(f + 1);
			matrix(i, 0) = rVal1;
			matrix(i, 1) = rVal2;
			matrix(i, 2) = rVal3;
		}
		double w1, w2, w3, rhosq1, rhosq2, rhosq3, errVar1, errVar2, errVar3;
		long r1, r2, r3;
		tripleCollocation(matrix, w1, w2, w3, r1, r2, r3, rhosq1, rhosq2, rhosq3, errVar1, errVar2, errVar3);
		tblAtt->Updated();
		Column colW1 = tblAtt->col("w1");
		Column colW2 = tblAtt->col("w2");
		Column colW3 = tblAtt->col("w3");
		Column colR1 = tblAtt->col("r1");
		Column colR2 = tblAtt->col("r2");
		Column colR3 = tblAtt->col("r3");
		Column colRhosq1 = tblAtt->col("rhosq1");
		Column colRhosq2 = tblAtt->col("rhosq2");
		Column colRhosq3 = tblAtt->col("rhosq3");
		Column colErrVar1 = tblAtt->col("errvar1");
		Column colErrVar2 = tblAtt->col("errvar2");
		Column colErrVar3 = tblAtt->col("errvar3");
		colW1->PutVal(f + 1,w1);
		colW2->PutVal(f + 1,w2);
		colW3->PutVal(f + 1,w3);
		colR1->PutVal(f + 1,r1);
		colR2->PutVal(f + 1,r2);
		colR3->PutVal(f + 1,r3);
		colRhosq1->PutVal(f + 1,rhosq1);
		colRhosq2->PutVal(f + 1,rhosq2);
		colRhosq3->PutVal(f + 1,rhosq3);
		colErrVar1->PutVal(f + 1,errVar1);
		colErrVar2->PutVal(f + 1,errVar2);
		colErrVar3->PutVal(f + 1,errVar3);
	}
	if ( tblAtt->fChanged)
		tblAtt->Store();
	return true;
}

void varcov_matrix(const RealMatrix & matrix, RealMatrix & mtVarCov) {
	int iSamples = matrix.iRows();
	int iVariables = matrix.iCols();
	LongMatrix matNnij(iVariables);
	RealMatrix matSumX(iVariables);
	RealMatrix matSumY(iVariables);
	RealMatrix matSumXY(iVariables);
	mtVarCov = RealMatrix(iVariables); // square iVariables x iVariables matrix

	Tranquilizer trq;
	trq.SetTitle(TR("Calculate varcov matrix"));
	trq.SetText(TR("Calculating"));
	trq.Start();

	for (int c = 0; c < iSamples; ++c) {
		if (trq.fUpdate(c, iSamples)) {
			return;
		}
		for (int nr = 0; nr < iVariables; ++nr) {
			double ni = matrix(c, nr);
			if (ni == rUNDEF)
				continue;
			for (int nx = nr; nx < iVariables; ++nx) {
				double nj = matrix(c, nx);
				if (nj == rUNDEF)
					continue;
				double ninj = ni * nj;
				matNnij(nr, nx) += 1;
				matSumX(nr, nx) += ni;
				matSumY(nr, nx) += nj;
				matSumXY(nr, nx) += ninj;
			}
		}
	}
	for (int i = 0; i < iVariables; ++i) {
		for (int j = 0; j < iVariables; ++j) {
			if (j >= i)
				mtVarCov(i, j) =
				(matNnij(i, j) > 1 ?
				matSumXY(i, j) / (matNnij(i, j) - 1) -
				matSumX(i, j) / matNnij(i, j) *
				matSumY(i, j) / matNnij(i, j) *
				matNnij(i, j) / (matNnij(i, j) - 1)
				: 0.0);
			else
				mtVarCov(i, j) = mtVarCov(j, i);
		}
	}
}

void PointMapTripleCollocation::tripleCollocation(const RealMatrix & matrix, double & w1, double & w2, double & w3, long & r1, long & r2, long & r3, double & rhosq1, double & rhosq2, double & rhosq3, double & errVar1, double & errVar2, double & errVar3)
{
	RealMatrix Qhat;
	varcov_matrix(matrix, Qhat);
	w1 = sqrt((Qhat(0,1)*Qhat(0,2))/(Qhat(0,0)*Qhat(1,2)));
	w2 = sign(Qhat(0,2)*Qhat(1,2))*sqrt((Qhat(0,1)*Qhat(1,2))/(Qhat(1,1)*Qhat(0,2)));
	w3 = sign(Qhat(0,1)*Qhat(1,2))*sqrt((Qhat(0,2)*Qhat(1,2))/(Qhat(2,2)*Qhat(0,1)));
	std::vector<std::pair<double, int>> ranks;
	ranks.push_back(std::make_pair(w1, 1));
	ranks.push_back(std::make_pair(w2, 2));
	ranks.push_back(std::make_pair(w3, 3));
	std::sort(ranks.begin(), ranks.end());
	r1 = ranks[2].second;
	r2 = ranks[1].second;
	r3 = ranks[0].second;
	rhosq1 = sqr(w1);
	rhosq2 = sqr(w2);
	rhosq3 = sqr(w3);
	errVar1 = abs(Qhat(0,0)-Qhat(0,1)*Qhat(0,2)/Qhat(1,2));
	errVar2 = abs(Qhat(1,1)-Qhat(0,1)*Qhat(1,2)/Qhat(0,2));
	errVar3 = abs(Qhat(2,2)-Qhat(0,2)*Qhat(1,2)/Qhat(0,1));
}







