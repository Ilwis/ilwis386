#include "PointMapLandAllocation.h"
#include "GA.h"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Base\Algorithm\Random.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Client\TableWindow\CartesianGraphDoc.h"
#include "LandAllocation.h"

IlwisObjectPtr * createPointMapLandAllocation(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PointMapLandAllocation::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PointMapLandAllocation(fn, (PointMapPtr &)ptr);
}

const char* PointMapLandAllocation::sSyntax() {
  return "PointMapLandAllocation(PotentialLocationPoints[,PotentialLocationTypes],DemandPoints[,DemandPreference],ODMatrix|,OptimalFacilities,capacitated|plain,StoppingCriteria,Generations,PopulationSize[,Nelite,Npareto],MutationPercent,CrossoverPercent)";
}

PointMapLandAllocation* PointMapLandAllocation::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 10 && iParms != 14)
	throw ErrorExpression(sExpr, sSyntax());
  bool fMultiObjective = (iParms == 14);
  if (fMultiObjective) {
	  if ((as[6].compare("plain") != 0) && (as[6].compare("capacitated") != 0))
		throw ErrorExpression(sExpr, sSyntax());
	  PointMap pmFacilities(as[0], fn.sPath());
	  String sColFacilitiesType(as[1]);
	  PointMap pmDemands(as[2], fn.sPath());
	  String sColDemandsPreference(as[3]);
	  String sODMatrix(as[4]);
	  int iOptimalFacilities(atoi(as[5].c_str()));
	  bool fCapacitated (as[6].compare("capacitated") == 0);
	  int iStoppingCriteria(atoi(as[7].c_str()));
	  long iGenerations(atoi(as[8].c_str()));
	  int iPopulationSize(atoi(as[9].c_str()));
	  int iNelite(atoi(as[10].c_str()));
	  int iNpareto(atoi(as[11].c_str()));
	  double rMutationPercent(atof(as[12].c_str()));
	  double rCrossoverPercent(atof(as[13].c_str()));
	  PointMap pmFacilitiesNoAttribute(fnGetSourceFile(pmFacilities, fn));
  	  PointMap pmDemandsNoAttribute(fnGetSourceFile(pmDemands, fn));
	  return new PointMapLandAllocation(fn, p, pmFacilities, pmFacilitiesNoAttribute, sColFacilitiesType, pmDemands, pmDemandsNoAttribute, sColDemandsPreference, sODMatrix, iOptimalFacilities, fCapacitated, iStoppingCriteria, iGenerations, iPopulationSize, iNelite, iNpareto, rMutationPercent, rCrossoverPercent);
  } else {
	  if ((as[4].compare("plain") != 0) && (as[4].compare("capacitated") != 0))
		throw ErrorExpression(sExpr, sSyntax());
	  PointMap pmFacilities(as[0], fn.sPath());
	  PointMap pmDemands(as[1], fn.sPath());
	  String sODMatrix(as[2]);
	  int iOptimalFacilities(atoi(as[3].c_str()));
	  bool fCapacitated (as[4].compare("capacitated") == 0);
	  int iStoppingCriteria(atoi(as[5].c_str()));
	  long iGenerations(atoi(as[6].c_str()));
	  int iPopulationSize(atoi(as[7].c_str()));
	  double rMutationPercent(atof(as[8].c_str()));
	  double rCrossoverPercent(atof(as[9].c_str()));
	  PointMap pmFacilitiesNoAttribute(fnGetSourceFile(pmFacilities, fn));
	  PointMap pmDemandsNoAttribute(fnGetSourceFile(pmDemands, fn));
	  return new PointMapLandAllocation(fn, p, pmFacilities, pmFacilitiesNoAttribute, "", pmDemands, pmDemandsNoAttribute, "", sODMatrix, iOptimalFacilities, fCapacitated, iStoppingCriteria, iGenerations, iPopulationSize, 0, 0, rMutationPercent, rCrossoverPercent);
  }
}

PointMapLandAllocation::PointMapLandAllocation(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
{
  fNeedFreeze = true;
  try {
    ReadElement("LandAllocation", "PointMapFacilities", pmFacilities);
    ReadElement("LandAllocation", "FacilitiesType", sColFacilitiesType);
	ReadElement("LandAllocation", "PointMapDemands", pmDemands);
	ReadElement("LandAllocation", "DemandsPreference", sColDemandsPreference);
	ReadElement("LandAllocation", "ODMatrix", sODMatrix);
	ReadElement("LandAllocation", "OptimalFacilities", iOptimalFacilities);
	ReadElement("LandAllocation", "Capacitated", fCapacitated);
	ReadElement("LandAllocation", "StoppingCriteria", iStoppingCriteria);
	ReadElement("LandAllocation", "Generations", iGenerations);
	ReadElement("LandAllocation", "PopulationSize", iPopulationSize);
	ReadElement("LandAllocation", "Nelite", iNelite);
	ReadElement("LandAllocation", "Npareto", iNpareto);
	ReadElement("LandAllocation", "MutationPercent", rMutationPercent);
	ReadElement("LandAllocation", "CrossoverPercent", rCrossoverPercent);
	pmFacilitiesNoAttribute = PointMap(fnGetSourceFile(pmFacilities, fn));
	pmDemandsNoAttribute = PointMap(fnGetSourceFile(pmDemands, fn));
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  fMultiObjective = (sColFacilitiesType.length() > 0) && (sColDemandsPreference.length() > 0);
  Init();
  objdep.Add(pmFacilities.ptr());
  objdep.Add(pmDemands.ptr());
}

PointMapLandAllocation::PointMapLandAllocation(const FileName& fn, PointMapPtr& p, const PointMap& _pmFacilities, const PointMap& _pmFacilitiesNoAttribute, const String& _sColFacilitiesType, const PointMap& _pmDemands, const PointMap& _pmDemandsNoAttribute, const String& _sColDemandsPreference,
							   const String& _sODMatrix, int _iOptimalFacilities, bool _fCapacitated, int _iStoppingCriteria, long _iGenerations, int _iPopulationSize, int _iNelite, int _iNpareto, double _rMutationPercent, double _rCrossoverPercent)
: PointMapVirtual(fn, p, _pmFacilitiesNoAttribute->cs(),_pmFacilitiesNoAttribute->cb(),Domain(fn, _iOptimalFacilities, dmtUNIQUEID, "feature"))
, pmFacilities(_pmFacilities)
, pmFacilitiesNoAttribute(_pmFacilitiesNoAttribute)
, sColFacilitiesType(_sColFacilitiesType)
, pmDemands(_pmDemands)
, pmDemandsNoAttribute(_pmDemandsNoAttribute)
, sColDemandsPreference(_sColDemandsPreference)
, sODMatrix(_sODMatrix)
, iOptimalFacilities(_iOptimalFacilities)
, fCapacitated(_fCapacitated)
, iStoppingCriteria(_iStoppingCriteria)
, iGenerations(_iGenerations)
, iPopulationSize(_iPopulationSize)
, iNelite(_iNelite)
, iNpareto(_iNpareto)
, rMutationPercent(_rMutationPercent)
, rCrossoverPercent(_rCrossoverPercent)
{
  fNeedFreeze = true;
  fMultiObjective = (sColFacilitiesType.length() > 0) && (sColDemandsPreference.length() > 0);
  Init();
  objdep.Add(pmFacilities.ptr());
  objdep.Add(pmDemands.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
}

void PointMapLandAllocation::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapLandAllocation");
  WriteElement("LandAllocation", "PointMapFacilities", pmFacilities);
  WriteElement("LandAllocation", "FacilitiesType", fMultiObjective ? sColFacilitiesType : "");
  WriteElement("LandAllocation", "PointMapDemands", pmDemands);
  WriteElement("LandAllocation", "DemandsPreference", fMultiObjective ? sColDemandsPreference : "");
  WriteElement("LandAllocation", "ODMatrix", sODMatrix);
  WriteElement("LandAllocation", "OptimalFacilities", iOptimalFacilities);
  WriteElement("LandAllocation", "Capacitated", fCapacitated);
  WriteElement("LandAllocation", "StoppingCriteria", iStoppingCriteria);
  WriteElement("LandAllocation", "Generations", iGenerations);
  WriteElement("LandAllocation", "PopulationSize", iPopulationSize);
  WriteElement("LandAllocation", "Nelite", iNelite);
  WriteElement("LandAllocation", "Npareto", iNpareto);
  WriteElement("LandAllocation", "MutationPercent", rMutationPercent);
  WriteElement("LandAllocation", "CrossoverPercent", rCrossoverPercent);
}

PointMapLandAllocation::~PointMapLandAllocation()
{
}

String PointMapLandAllocation::sExpression() const
{
	if (fMultiObjective)
		return String("PointMapLandAllocation(%S,%S,%S,%S,%S,%d,%s,%d,%d,%d,%d,%d,%f,%f)", pmFacilities->sNameQuoted(false, fnObj.sPath()), sColFacilitiesType, pmDemands->sNameQuoted(false, fnObj.sPath()), sColDemandsPreference, sODMatrix, iOptimalFacilities, fCapacitated ? "capacitated" : "plain", iStoppingCriteria, iGenerations, iPopulationSize, iNelite, iNpareto, rMutationPercent, rCrossoverPercent);
	else
		return String("PointMapLandAllocation(%S,%S,%S,%d,%s,%d,%d,%d,%f,%f)", pmFacilities->sNameQuoted(false, fnObj.sPath()), pmDemands->sNameQuoted(false, fnObj.sPath()), sODMatrix, iOptimalFacilities, fCapacitated ? "capacitated" : "plain", iStoppingCriteria, iGenerations, iPopulationSize, rMutationPercent, rCrossoverPercent);
}

bool PointMapLandAllocation::fDomainChangeable() const
{
  return false;
}

FileName PointMapLandAllocation::fnGetSourceFile(const PointMap & pm, const FileName & fnObj)
{
	if (pm->fnObj.fValid())
		return pm->fnObj;
	else
	{
		String sExpr =  pm->sExpression();
		char *p = sExpr.strrchrQuoted('.');
		// *p is not necessarily an extension's dot
		char *pMax = (sExpr.length() > 0) ? (const_cast<char*>(&(sExpr)[sExpr.length() - 1])) : 0; // last valid char in sExpr
		char *q = sExpr.strrchrQuoted('\\');
		// *p is definitely not an extension's dot if p<q
		if (p<q || p>=pMax)
			p = 0;
		// Now we're (only) sure that p points to the last dot in the filename
		if (p)
			*p = 0;
		const String sFile = sExpr.c_str();
		FileName fn(sFile, ".mpp", true);
		if (0 == strchr(sFile.c_str(), ':')) // no path set
			fn.Dir(fnObj.sPath()); 
		return fn;
	}
}

void PointMapLandAllocation::Init()
{
  htpFreeze = "ilwisapp\\mask_points_algorithm.htm";
  sFreezeTitle = "PointMapLandAllocation";
}

bool PointMapLandAllocation::fFreezing()
{
	long iPoints = pmFacilities->iFeatures();
	if (iPoints < 2)
		throw ErrorObject(TR("Not enough potential locations"));
	if (iOptimalFacilities > iPoints)
		throw ErrorObject(TR("Nr requested facilities is larger than the nr of available facilities"));
	if (iPopulationSize < 1)
		throw ErrorObject(TR("Bad population size"));
	Table2Dim talbeODmatrix;
	if (sODMatrix != "")
		talbeODmatrix = Table2Dim (sODMatrix);

	LandAllocation la (pmFacilities, pmFacilitiesNoAttribute, sColFacilitiesType, pmDemands, pmDemandsNoAttribute, sColDemandsPreference, talbeODmatrix,
							   iOptimalFacilities, fCapacitated, iStoppingCriteria, iGenerations, iPopulationSize, iNelite, iNpareto, rMutationPercent, rCrossoverPercent);
	GAChromosome * chromosome = la.PerformLandAllocation(trq);
	la.StoreChromosome(chromosome, &ptr);
	_iPoints = pms->iPnt();
	return true;
}


