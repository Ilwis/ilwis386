/*
// $Log: /ILWIS 3.0/PointMap/PNTTRNSF.cpp $
 * 
 * 8     28-07-07 15:01 Hendrikse
 * funcions
 *  LatLon(crd.y, crd.x) and 
 * Coord(llTmp.Lon , llTmp.Lat) 
 * have now their input parameters in the correct order
 * 
 * 7     24-07-07 17:16 Hendrikse
 * debugged xy mistake in last else of datum transformation loop
 * 
 * 6     24-07-07 13:13 Hendrikse
 * Addede fUseReals() condition at the and of the pointwise height
 * transormation. Not helpful yet
 * 
 * 5     20-04-06 15:25 Hendrikse
 * corrected  radian-> degree conversion
 * 
 * 4     5-10-04 22:03 Hendrikse
 * Added functionality from Datum classes to allow 3D coord
 * transformations (heights in value maps or in attrib table) only
 * possible for pointmap havin coordsystem with user-defined BursaWolf or
 * Molodensky-Badekas Datum.
 * Syntax on cmdline: PointMapTransform(pntmap,coordsys
 * [,th|transformheights])
 * Output range is defined by input minmax height with a margin of +/- 200
 * m to cver the range of the geoid ondulation.
 * The transf algorithm is equal to that of the Interactive Applic Form
 * 'Transform Coordinate Heights, implemented 1 month ago (aug 2004)
 * 
 * 3     9/08/99 11:59a Wind
 * comments
 * 
 * 2     9/08/99 10:22a Wind
 * adpated to use of quoted file names
*/
// Revision 1.3  1998/09/16 17:26:27  Wim
// 22beta2
//
// Revision 1.2  1997/08/05 10:39:36  Wim
// sSyntax() corrected
//
/* PointMapTransform
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  WK    5 Aug 97   12:39 pm
*/                                                                      

#include "PointApplications\PNTTRNSF.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\point.hs"
#include "Engine\SpatialReference\csviall.h"
#include "Engine\SpatialReference\Csproj.h"

const char* PointMapTransform::sSyntax() {
  return "PointMapTransform(pntmap,coordsys [,th|transformheights])";
}

IlwisObjectPtr * createPointMapTransform(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapTransform::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapTransform(fn, (PointMapPtr &)ptr);
}

PointMapTransform* PointMapTransform::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms > 3 || iParms < 2)
    throw ErrorExpression(sExpr, sSyntax());
	bool fTransfH = false;
  PointMap pmp(as[0], fn.sPath());
  CoordSystem csy(as[1], fn.sPath());
	if (iParms == 3 && (fCIStrEqual("TH", as[2]) // if user types 'th' or 'TH' etc, height will be used and transformed
				|| fCIStrEqual("transformheights", as[2]))) // or if user types 'transformheights' ,case-insensitive
		fTransfH = true;
  return new PointMapTransform(fn, p, pmp, csy, fTransfH);
}

PointMapTransform::PointMapTransform(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("PointMapTransform", "PointMap", pmp);
    ReadElement("PointMapTransform", "CoordSys", csy);
		ReadElement("PointMapTransform", "Transform Heights", m_fTransfHeights);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(pmp.ptr());
  objdep.Add(csy.ptr());
}

PointMapTransform::PointMapTransform(const FileName& fn, PointMapPtr& p, const PointMap& pm, 
                                   const CoordSystem& cs, bool fTransfHeights)
: PointMapVirtual(fn, p, cs,cs->cb,pm->dvrs()), pmp(pm), csy(cs)
{
  if (!cs->fConvertFrom(pmp->cs()))
    IncompatibleCoordSystemsError(cs->sName(true, fnObj.sPath()), pmp->cs()->sName(true, fnObj.sPath()), sTypeName(), errPointMapTransform);
  fNeedFreeze = true;
	m_fTransfHeights = fTransfHeights;
	if (m_fTransfHeights) {
		DomainValueRangeStruct dvrs(vrDefault(pmp));
		SetDomainValueRangeStruct(dvrs);
	}
  Init();
  objdep.Add(pmp.ptr());
  objdep.Add(csy.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  if (pmp->fTblAttSelf())
    SetAttributeTable(pmp->tblAtt());
  SetCoordBounds(csy->cbConv(pmp->cs(), pmp->cb()));
}

void PointMapTransform::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapTransform");
  WriteElement("PointMapTransform", "PointMap", pmp);
  WriteElement("PointMapTransform", "CoordSys", csy);
	WriteElement("PointMapTransform", "Transform Heights", m_fTransfHeights);
}

PointMapTransform::~PointMapTransform()
{
}

String PointMapTransform::sExpression() const
{
	String sTransfHeights;
	if (m_fTransfHeights)
		sTransfHeights = String(",TH");
	else
		sTransfHeights = String("");
  return String("PointMapTransform(%S,%S%S)", pmp->sNameQuoted(false, fnObj.sPath()), 
                                              csy->sNameQuoted(false, fnObj.sPath()),
																							sTransfHeights);
}


void PointMapTransform::Init()
{
  htpFreeze = "ilwisapp\\transform_point_map_functionality_algorithm.htm";
  sFreezeTitle = "PointMapTransform";
	if (m_fTransfHeights)
		if (!f3DDatumTransformationPossible(pmp->cs(), csy))
			 IncompatibleCoordSystemsError(pmp->cs()->sName(true, fnObj.sPath()), csy->sName(true, fnObj.sPath()), sTypeName(), errPointMapTransform+1);
}

bool PointMapTransform::f3DDatumTransformationPossible(CoordSystem cs1, const CoordSystem& cs2)
{
	csvll1 = pmp->cs()->pcsViaLatLon();
	csvll2 = csy->pcsViaLatLon();
	cspr1 = pmp->cs()->pcsProjection();
	cspr2 = csy->pcsProjection();
	fFromWGS = false; 
	fToWGS = false;
	if (csvll1 != 0 && csvll2 != 0 && pmp->fValues())
  {
		Datum* datum1 = csvll1->datum;
		Datum* datum2 = csvll2->datum;
		bwdat1 = dynamic_cast<BursaWolfDatum*>(datum1);
		bwdat2 = dynamic_cast<BursaWolfDatum*>(datum2);
		badat1 = dynamic_cast<BadekasDatum*>(datum1);
		badat2 = dynamic_cast<BadekasDatum*>(datum2);
  
		MolodenskyDatum* mold1 = dynamic_cast<MolodenskyDatum*>(datum1);
		MolodenskyDatum* mold2 = dynamic_cast<MolodenskyDatum*>(datum2);
		if (mold1) {
			fFromWGS = (mold1->dx == 0 && mold1->dy == 0 || mold1->dz == 0);
			fFromWGS &= (mold1->ell.sName == String("WGS 84"));
		}  // shift-free WGS molodensky-datum (is permitted)
		if (mold2) {
			fToWGS = (mold2->dx == 0 && mold2->dy == 0 || mold2->dz == 0);
			fToWGS &= (mold2->ell.sName == String("WGS 84"));
		}  // shift-free WGS molodensky-datum (is permitted)
	}	
	else
		return false; 

	return ((fFromWGS || bwdat1 != 0 || badat1 != 0) && (fToWGS || bwdat2 != 0 || badat2 == 0));
}

ValueRange PointMapTransform::vrDefault(const PointMap& pmap)
{
	RangeReal rr = pmap->rrMinMax(BaseMapPtr::mmmNOCALCULATE);
	if (!rr.fValid())
		rr = pmap->dvrs().rrMinMax();
	double rRangeWidth = abs(rr.rHi() - rr.rLo());
	double rLowestOut = rr.rLo() - 200.0;//rRangeWidth; replaced by 200m which is below minimum geoid height
	double rHighestOut = rr.rHi() + 200.0;//rRangeWidth;replaced by 200m which is above maximum geoid height
	double rStep = pmap->dvrs().rStep();
	return ValueRange(rLowestOut, rHighestOut, rStep);
}

bool PointMapTransform::fFreezing()
{
  trq.SetText(String(TR("Transforming '%S'").c_str(), sName(true, fnObj.sPath())));
  pms->iAdd(pmp->iFeatures());
  Coord crd;
	/// additions to enable Datum-height transf in pointmaps
	if (m_fTransfHeights)
	{
		LatLonHeight llhIn;
		LatLonHeight llhWGS;
		LatLonHeight llhOut;
		LatLon llTmp;
		Coord cTmp = crd; //input {both formats, XY and LL)
		for (long i=1; i <= pmp->iFeatures(); ++i ) 
		{
			if (trq.fUpdate(i, pmp->iFeatures()))
				return false; 
			crd = pmp->cValue(i);
			if (crd.fUndef())
				continue;
			CoordCTS ctsIn, ctsOut;
			llTmp = LatLon(crd.y, crd.x); //if inut crds not projection
			if (0 != cspr1)
				llTmp = cspr1->llConv(crd);
			llhIn = LatLonHeight(llTmp, pmp->rValue(i));
			if (badat1 != 0)
				llhWGS = badat1->llhToWGS84(llhIn);
			else if (bwdat1 != 0)
				llhWGS = bwdat1->llhToWGS84(llhIn);
			else
				llhWGS = llhIn;
			if (badat2 != 0)
				llhOut = badat2->llhFromWGS84(llhWGS);
			else if (bwdat2 != 0)
				llhOut = bwdat2->llhFromWGS84(llhWGS);
			else
				llhOut = llhWGS;
			llTmp = LatLon(llhOut.Lat, llhOut.Lon);
			if (0 != cspr2)
				crd = cspr2->cConv(llTmp);
			else
				crd = Coord(llTmp.Lon , llTmp.Lat);
			if (crd.fUndef())
				continue;
			pms->PutVal(i,crd);
			pms->PutVal(i, llhOut.rHeight);
		}
	}
	else
	{
		for (long i=1; i <= pmp->iFeatures(); ++i ) 
		{
			if (trq.fUpdate(i, pmp->iFeatures()))
				return false; 
			crd = pmp->cValue(i);
			if (crd.fUndef())
				continue;
			crd = csy->cConv(pmp->cs(), pmp->cValue(i));
			if (crd.fUndef())
				continue;
			pms->PutVal(i,crd);
			if (fUseReals())
				pms->PutVal(i, pmp->rValue(i));
			else
				pms->PutRaw(i, pmp->iRaw(i));
		}
	}
  trq.fUpdate(pmp->iFeatures(), pmp->iFeatures());
  _iPoints = pms->iPnt();
  return true;
}




