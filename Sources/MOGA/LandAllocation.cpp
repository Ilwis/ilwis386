#include "LandAllocation.h"
#include "GA.h"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Base\Algorithm\Random.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Client\TableWindow\CartesianGraphDoc.h"

IlwisObjectPtr * createLandAllocation(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)LandAllocation::create(fn, (PointMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new LandAllocation(fn, (PointMapPtr &)ptr);
}

const char* LandAllocation::sSyntax() {
  return "PointMapLandAllocation(PotentialLocationPoints[,PotentialLocationTypes],DemandPoints[,DemandPreference],OptimalFacilities,capacitated|plain,StoppingCriteria,Generations,PopulationSize,MutationPercent,CrossoverPercent)";
}

LandAllocation* LandAllocation::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 9 && iParms != 11)
	throw ErrorExpression(sExpr, sSyntax());
  bool fMultiObjective = (iParms == 11);
  if (fMultiObjective) {
	  if ((as[5].compare("plain") != 0) && (as[5].compare("capacitated") != 0))
		throw ErrorExpression(sExpr, sSyntax());
	  PointMap pmFacilities(as[0], fn.sPath());
	  String sColFacilitiesType(as[1]);
	  PointMap pmDemands(as[2], fn.sPath());
	  String sColDemandsPreference(as[3]);
	  int iOptimalFacilities(atoi(as[4].c_str()));
	  bool fCapacitated (as[5].compare("capacitated") == 0);
	  int iStoppingCriteria(atoi(as[6].c_str()));
	  long iGenerations(atoi(as[7].c_str()));
	  int iPopulationSize(atoi(as[8].c_str()));
	  double rMutationPercent(atof(as[9].c_str()));
	  double rCrossoverPercent(atof(as[10].c_str()));
	  PointMap pmFacilitiesNoAttribute(fnGetSourceFile(pmFacilities, fn));
	  return new LandAllocation(fn, p, pmFacilities, pmFacilitiesNoAttribute, sColFacilitiesType, pmDemands, sColDemandsPreference, iOptimalFacilities, fCapacitated, iStoppingCriteria, iGenerations, iPopulationSize, rMutationPercent, rCrossoverPercent);
  } else {
	  if ((as[3].compare("plain") != 0) && (as[3].compare("capacitated") != 0))
		throw ErrorExpression(sExpr, sSyntax());
	  PointMap pmFacilities(as[0], fn.sPath());
	  PointMap pmDemands(as[1], fn.sPath());
	  int iOptimalFacilities(atoi(as[2].c_str()));
	  bool fCapacitated (as[3].compare("capacitated") == 0);
	  int iStoppingCriteria(atoi(as[4].c_str()));
	  long iGenerations(atoi(as[5].c_str()));
	  int iPopulationSize(atoi(as[6].c_str()));
	  double rMutationPercent(atof(as[7].c_str()));
	  double rCrossoverPercent(atof(as[8].c_str()));
	  PointMap pmFacilitiesNoAttribute(fnGetSourceFile(pmFacilities, fn));
	  return new LandAllocation(fn, p, pmFacilities, pmFacilitiesNoAttribute, "", pmDemands, "", iOptimalFacilities, fCapacitated, iStoppingCriteria, iGenerations, iPopulationSize, rMutationPercent, rCrossoverPercent);
  }
}

LandAllocation::LandAllocation(const FileName& fn, PointMapPtr& p)
: PointMapVirtual(fn, p)
, cFacilities(0)
, cDemands(0)
, rDemand(0)
, rCapacity(0)
, rMinDistance(rUNDEF)
, rMaxDistance(rUNDEF)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("LandAllocation", "PointMapFacilities", pmFacilities);
    ReadElement("LandAllocation", "FacilitiesType", sColFacilitiesType);
	ReadElement("LandAllocation", "PointMapDemands", pmDemands);
	ReadElement("LandAllocation", "DemandsPreference", sColDemandsPreference);
	ReadElement("LandAllocation", "OptimalFacilities", iOptimalFacilities);
	ReadElement("LandAllocation", "Capacitated", fCapacitated);
	ReadElement("LandAllocation", "StoppingCriteria", iStoppingCriteria);
	ReadElement("LandAllocation", "Generations", iGenerations);
	ReadElement("LandAllocation", "PopulationSize", iPopulationSize);
	ReadElement("LandAllocation", "MutationPercent", rMutationPercent);
	ReadElement("LandAllocation", "CrossoverPercent", rCrossoverPercent);
	pmFacilitiesNoAttribute = PointMap(fnGetSourceFile(pmFacilities, fn));
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

LandAllocation::LandAllocation(const FileName& fn, PointMapPtr& p, const PointMap& _pmFacilities, const PointMap& _pmFacilitiesNoAttribute, const String& _sColFacilitiesType, const PointMap& _pmDemands, const String& _sColDemandsPreference,
							   int _iOptimalFacilities, bool _fCapacitated, int _iStoppingCriteria, long _iGenerations, int _iPopulationSize, double _rMutationPercent, double _rCrossoverPercent)
: PointMapVirtual(fn, p, _pmFacilitiesNoAttribute->cs(),_pmFacilitiesNoAttribute->cb(),_pmFacilitiesNoAttribute->dvrs())
, pmFacilities(_pmFacilities)
, pmFacilitiesNoAttribute(_pmFacilitiesNoAttribute)
, sColFacilitiesType(_sColFacilitiesType)
, pmDemands(_pmDemands)
, sColDemandsPreference(_sColDemandsPreference)
, iOptimalFacilities(_iOptimalFacilities)
, fCapacitated(_fCapacitated)
, iStoppingCriteria(_iStoppingCriteria)
, iGenerations(_iGenerations)
, iPopulationSize(_iPopulationSize)
, rMutationPercent(_rMutationPercent)
, rCrossoverPercent(_rCrossoverPercent)
, cFacilities(0)
, cDemands(0)
, rDemand(0)
, rCapacity(0)
, rMinDistance(rUNDEF)
, rMaxDistance(rUNDEF)
{
  fNeedFreeze = true;
  fMultiObjective = (sColFacilitiesType.length() > 0) && (sColDemandsPreference.length() > 0);
  Init();
  objdep.Add(pmFacilities.ptr());
  objdep.Add(pmDemands.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  if (pmFacilitiesNoAttribute->fTblAttSelf())
    SetAttributeTable(pmFacilitiesNoAttribute->tblAtt());
}

void LandAllocation::Store()
{
  PointMapVirtual::Store();
  WriteElement("PointMapVirtual", "Type", "PointMapLandAllocation");
  WriteElement("LandAllocation", "PointMapFacilities", pmFacilities);
  WriteElement("LandAllocation", "FacilitiesType", fMultiObjective ? sColFacilitiesType : "");
  WriteElement("LandAllocation", "PointMapDemands", pmDemands);
  WriteElement("LandAllocation", "DemandsPreference", fMultiObjective ? sColDemandsPreference : "");
  WriteElement("LandAllocation", "OptimalFacilities", iOptimalFacilities);
  WriteElement("LandAllocation", "Capacitated", fCapacitated);
  WriteElement("LandAllocation", "StoppingCriteria", iStoppingCriteria);
  WriteElement("LandAllocation", "Generations", iGenerations);
  WriteElement("LandAllocation", "PopulationSize", iPopulationSize);
  WriteElement("LandAllocation", "MutationPercent", rMutationPercent);
  WriteElement("LandAllocation", "CrossoverPercent", rCrossoverPercent);
}

LandAllocation::~LandAllocation()
{
	if (cFacilities)
		delete [] cFacilities;
	if (cDemands)
		delete [] cDemands;
	if (rDemand)
		delete [] rDemand;
	if (rCapacity)
		delete [] rCapacity;
}

String LandAllocation::sExpression() const
{
	if (fMultiObjective)
		return String("PointMapLandAllocation(%S,%S,%S,%S,%d,%s,%d,%d,%d,%f,%f)", pmFacilities->sNameQuoted(false, fnObj.sPath()), sColFacilitiesType, pmDemands->sNameQuoted(false, fnObj.sPath()), sColDemandsPreference, iOptimalFacilities, fCapacitated ? "capacitated" : "plain", iStoppingCriteria, iGenerations, iPopulationSize, rMutationPercent, rCrossoverPercent);
	else
		return String("PointMapLandAllocation(%S,%S,%d,%s,%d,%d,%d,%f,%f)", pmFacilities->sNameQuoted(false, fnObj.sPath()), pmDemands->sNameQuoted(false, fnObj.sPath()), iOptimalFacilities, fCapacitated ? "capacitated" : "plain", iStoppingCriteria, iGenerations, iPopulationSize, rMutationPercent, rCrossoverPercent);
}

bool LandAllocation::fDomainChangeable() const
{
  return false;
}

FileName LandAllocation::fnGetSourceFile(const PointMap & pm, const FileName & fnObj)
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

void LandAllocation::Init()
{
  htpFreeze = "ilwisapp\\mask_points_algorithm.htm";
  sFreezeTitle = "PointMapLandAllocation";

  int iNrDemandPoints = pmDemands->iFeatures();
  int iNrFacilities = pmFacilities->iFeatures();

  cDemands = new Coord [iNrDemandPoints];
  for (int i = 0; i < iNrDemandPoints; ++i)
	  cDemands[i] = pmDemands->cValue(i);
  cFacilities = new Coord [iNrFacilities];
  for (int i = 0; i < iNrFacilities; ++i)
	  cFacilities[i] = pmFacilities->cValue(i);
  
  rDistanceOD.resize(iNrDemandPoints);
  for (int demandIndex = 0; demandIndex < iNrDemandPoints; ++demandIndex) {
	  rDistanceOD[demandIndex].resize(iNrFacilities);
	  for (int facilityIndex = 0; facilityIndex < iNrFacilities; ++facilityIndex) {
		  double rDistance = rDist(cFacilities[facilityIndex], cDemands[demandIndex]);
		  rDistanceOD[demandIndex][facilityIndex] = rDistance;
		  if (rMinDistance == rUNDEF) {
			  rMinDistance = rDistance;
			  rMaxDistance = rDistance;
		  } else {
			  rMinDistance = min(rMinDistance, rDistance);
			  rMaxDistance = max(rMaxDistance, rDistance);
		  }
	  }
  }

  if (fMultiObjective) {
	  FileName fnDemandsPreference (fnGetSourceFile(pmDemands, fnObj));
	  FileName fnFacilitiesType (fnGetSourceFile(pmFacilities, fnObj));
	  PointMap pmDemandsPreference (fnDemandsPreference);
	  PointMap pmFacilitiesType (fnFacilitiesType);

	  vector<String> sDemandsPreference;
	  sDemandsPreference.resize(iNrDemandPoints);
	  if (pmDemandsPreference.fValid() && pmDemandsPreference->fTblAtt()) {
		  Table tbl = pmDemandsPreference->tblAtt();
		  Column col = tbl->col(sColDemandsPreference);
		  for (int i = 0; i < iNrDemandPoints; ++i)
			  sDemandsPreference[i] = col->sValue(pmDemandsPreference->iRaw(i), 0);
	  }
	  vector <String> sFacilitiesType;
	  sFacilitiesType.resize(iNrFacilities);
	  if (pmFacilitiesType.fValid() && pmFacilitiesType->fTblAtt()) {
		  Table tbl = pmFacilitiesType->tblAtt();
		  Column col = tbl->col(sColFacilitiesType);
		  for (int i = 0; i < iNrFacilities; ++i)
			  sFacilitiesType[i] = col->sValue(pmFacilitiesType->iRaw(i), 0);
	  }

	  rPreferenceMatrix.resize(iNrDemandPoints);
	  for (int demandIndex = 0; demandIndex < iNrDemandPoints; ++demandIndex) {
		  rPreferenceMatrix[demandIndex].resize(iNrFacilities);
		  for (int facilityIndex = 0; facilityIndex < iNrFacilities; ++facilityIndex)
			  rPreferenceMatrix[demandIndex][facilityIndex] = (sDemandsPreference[demandIndex] == sFacilitiesType[facilityIndex]) ? 1.0 : 0.0;
	  }
  }

  if (pmDemands->dvrs().fValues())
  {
	  rDemand = new double [iNrDemandPoints];
	  for (int i = 0; i < iNrDemandPoints; ++i)
		  rDemand[i] = pmDemands->rValue(i);
  }
  if (pmFacilities->dvrs().fValues())
  {
	  rCapacity = new double [iNrFacilities];
	  for (int i = 0; i < iNrFacilities; ++i)
		  rCapacity[i] = pmFacilities->rValue(i);
  }
}

bool LandAllocation::fFreezing()
{
	seedrand(clock());
	trq.SetText(String(TR("Finding optimal locations in '%S'").c_str(), pmFacilities->sName()));
	long iPoints = pmFacilities->iFeatures();
	if (iPoints < 2)
		throw ErrorObject(TR("Not enough potential locations"));
	if (iOptimalFacilities > iPoints)
		throw ErrorObject(TR("Nr requested facilities is larger than the nr of available facilities"));
	if (iPopulationSize < 1)
		throw ErrorObject(TR("Bad population size"));

	ScoreFunc scoreFunc1 = (ScoreFunc)&LandAllocation::rStdDistanceFunc;
	ScoreFunc scoreFunc2 = (ScoreFunc)&LandAllocation::rStdPreferenceFunc;

	GA GAAlgorithm(this, fMultiObjective ? (FitnessFunc)&LandAllocation::FitnessMO : (FitnessFunc)&LandAllocation::FitnessSO, scoreFunc1, scoreFunc2);
	GAAlgorithm.SetSelectionType(fMultiObjective ? GA::Probability : GA::Tournament);
	GAAlgorithm.SetStoppingCriteria(iStoppingCriteria);
	GAAlgorithm.SetGenerations(iGenerations);
	GAAlgorithm.SetPopulationSize(iPopulationSize);
	GAAlgorithm.SetMutation(rMutationPercent);
	GAAlgorithm.SetCrossover(rCrossoverPercent);
	GAAlgorithm.Initialize();
	RealBuf vrFitnessList (iGenerations);
	RealBuf vrPopulationAvgList (iGenerations);
	int iGenerationNr = 0;
	while (!GAAlgorithm.IsDone())
	{
		vrFitnessList[iGenerationNr] = GAAlgorithm.rGetBestValue(iGenerationNr);
		vrPopulationAvgList[iGenerationNr] = (GAAlgorithm.rGetAverageValue(iGenerationNr));
        GAAlgorithm.CreateNextGeneration();
	    if (trq.fUpdate(iGenerationNr, GAAlgorithm.GetGenerations()))
			return false;
        ++iGenerationNr;
	}
	trq.fUpdate(GAAlgorithm.GetGenerations(), GAAlgorithm.GetGenerations());
	double rBestFitness = vrFitnessList[GAAlgorithm.GetGenerations() - 1];
	for (int iGenerationNr = 0; iGenerationNr < GAAlgorithm.GetGenerations(); ++iGenerationNr)
	{
		vrFitnessList[iGenerationNr] /= rBestFitness;
		vrPopulationAvgList[iGenerationNr] /= rBestFitness;
	}
    GAChromosome * chromosome = GAAlgorithm.GetBestChromosome();
	if (chromosome != 0)
	{
		for (int i = 0; i < iOptimalFacilities; i++)
		{
			unsigned int facilityIndex = chromosome->at(i);
			String sValue = pmFacilitiesNoAttribute->sValue(facilityIndex,0);
			pms->iAddVal(pmFacilities->cValue(facilityIndex), sValue);
		}
	}

	// Create Fitness Table

	FileName fnFitness (fnObj, ".tbt", true);
	fnFitness.sFile += "_fitness";
	Domain fitnessDom (FileName(fnFitness, ".dom", true), iGenerations, dmtUNIQUEID);
	Table fitnessTbl (fnFitness, fitnessDom);
	Column colBestFitness (fitnessTbl, "BestFitness", DomainValueRangeStruct(0, 1, 0));
	Column colAvgFitness (fitnessTbl, "AvgFitness", DomainValueRangeStruct(0, 1, 0));
	colBestFitness->PutBufVal(vrFitnessList, 1);
	colAvgFitness->PutBufVal(vrPopulationAvgList, 1);

	// Create Fitness Graph

	CartesianGraphDoc cgd;
	cgd.CreateNewGraph(fitnessTbl, Column(), colBestFitness, "Contineous", Color(0, 255, 0));
	cgd.AddColumnGraph(colAvgFitness, "Contineous", Color(255, 0, 255));
	// Save the Fitness Graph
	FileName fnFitnessGraph(fnFitness, ".grh", true);
	String sFile = fnFitnessGraph.sFullPath();
	const char * lpszPathName = sFile.c_str();
	CFileException fe;
	CFile* pFile = cgd.GetFile(lpszPathName, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive, &fe);
	if (pFile == NULL)
	{
		cgd.ReportSaveLoadException(lpszPathName, &fe, TRUE, AFX_IDP_INVALID_FILENAME);
	}
	else
	{
		CArchive saveArchive(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
		saveArchive.m_pDocument = &cgd;
		saveArchive.m_bForceFlat = FALSE;
		TRY
		{
			CWaitCursor wait;
			cgd.Serialize(saveArchive);
			saveArchive.Close();
			cgd.ReleaseFile(pFile, FALSE);
		}
		CATCH_ALL(e)
		{
			cgd.ReleaseFile(pFile, TRUE);

			TRY
			{
				cgd.ReportSaveLoadException(lpszPathName, e, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);
			}
			END_TRY
			e->Delete(); //DELETE_EXCEPTION(e);
			return FALSE;
		}
		END_CATCH_ALL
	}

	// Create Connections Segment Map
	if (chromosome != 0)
	{
		FileName fnConnections (fnObj, ".mps", true);
		fnConnections.sFile += "_connections";
		Domain dmConnections (FileName(fnConnections, ".dom", true), 0, dmtUNIQUEID);
		DomainIdentifier* dmIdentifierPtr = dmConnections->pdid();

		CoordSystem csyDest (pmFacilities->cs());
		CoordBounds cbMap (pmDemands->cb());
		cbMap += pmFacilities->cb();
		SegmentMap segMap(fnConnections, csyDest, cbMap, dmConnections);
		vector<int> source;
		vector<int> destination;
		vector<double> allocations;
		unsigned long iNrSegments = AddConnections(segMap, dmConnections, source, destination, allocations, *chromosome, this, scoreFunc1, scoreFunc2);
		LongBuf lbSource (iNrSegments);
		LongBuf lbDestination (iNrSegments);
		RealBuf rbAllocations (iNrSegments);
		for (unsigned long i = 0; i < iNrSegments; ++i)
		{
			lbSource[i] = source[i];
			lbDestination[i] = destination[i];
			rbAllocations[i] = allocations[i];
		}

		// Create the Attribute Table
		Table connectionTbl = Table(FileName(fnConnections, ".tbt", true), dmConnections);

		PointMap pmDemandsNoAttribute (fnGetSourceFile(pmDemands, fnObj));
		Column colSource (connectionTbl, "DemandID", pmDemandsNoAttribute->dm());
		colSource->PutBufRaw(lbSource, 1);
		Column colDestination (connectionTbl, "FacilityID", pmFacilitiesNoAttribute->dm());
		colDestination->PutBufRaw(lbDestination, 1);
		Column colAllocations (connectionTbl, "Allocated", DomainValueRangeStruct(0, 10000, 0));
		colAllocations->PutBufVal(rbAllocations, 1);

		segMap->SetAttributeTable(connectionTbl);
	}

  _iPoints = pms->iPnt();
  return true;
}

double LandAllocation::rStdDistanceFunc(int demandIndex, int facilityIndex)
{
	double distanceFacilityDemand = rDistanceOD[demandIndex][facilityIndex];
	//double rScore = 1 - distanceFacilityDemand / rMaxDistance + rMinDistance / rMaxDistance; // MAXIMUM
	//double rScore = 1 / distanceFacilityDemand; // ORIGINAL FORMULA (not standardized to [0..1])
	double rScore = 1 - (distanceFacilityDemand - rMinDistance) / (rMaxDistance - rMinDistance); // INTERVAL
	return rScore;
}

double LandAllocation::rStdPreferenceFunc(int demandIndex, int facilityIndex)
{
	return rPreferenceMatrix[demandIndex][facilityIndex];
}

void LandAllocation::FitnessSO(GAChromosome & chromosome, LandAllocation * context, ScoreFunc scoreFunc1, ScoreFunc scoreFunc2)
{
    if (chromosome.size() == 0)
        return;
	int iNrFacilities = pmFacilities->iFeatures();
    double * Allocation = new double [iNrFacilities]; // keep track of the allocation while we're in the loop
	for (int i = 0; i < iNrFacilities; ++i)
		Allocation[i] = 0;
    double totalScore = 0;
	double totalAllocation = 0;
	int iNrDemandPoints = pmDemands->iFeatures();
    for (int demandIndex = 0; demandIndex < iNrDemandPoints; demandIndex++)
    {
		double rDemandCount = (rDemand != 0) ? rDemand[demandIndex] : 1.0; //when an attribute is used to denote how many demands are at that location
		while (rDemandCount > 0)
		{
			double rBestScore = -1;
			int selectedFacilityIndex = -1;
			for (int chromosomeIndex = 0; chromosomeIndex < iOptimalFacilities; chromosomeIndex++)
			{
				int facilityIndex = chromosome[chromosomeIndex];

				double rScore = (context->*scoreFunc1)(demandIndex, facilityIndex);

				if ((!fCapacitated) || (Allocation[facilityIndex] < ((rCapacity != 0) ? rCapacity[facilityIndex] : 1.0))) // If a capacity attribute is indicated, respect the maximum capacity of the facility
				{
					if ((selectedFacilityIndex == -1) || (rScore > rBestScore))
					{
						rBestScore = rScore;
						selectedFacilityIndex = facilityIndex;
					}
				}
			}
			if (selectedFacilityIndex != -1)
			{
				double allocated = fCapacitated ? min(((rCapacity != 0) ? rCapacity[selectedFacilityIndex] : 1.0) - Allocation[selectedFacilityIndex], rDemandCount) : rDemandCount;
				Allocation[selectedFacilityIndex] += allocated;
				totalScore += rBestScore * allocated;
				totalAllocation += allocated;
				rDemandCount -= allocated; // The leftover demands will have to be served by another facility
			}
			else
			{
				totalScore = 0; // abandon this chromosome, as it did not reach to a solution
				demandIndex = iNrDemandPoints;
				break;
			}
		}
    }

	chromosome.SetFitness(totalScore / totalAllocation);

	delete [] Allocation;
}

void LandAllocation::FitnessMO(GAChromosome & chromosome, LandAllocation * context, ScoreFunc scoreFunc1, ScoreFunc scoreFunc2)
{
    if (chromosome.size() == 0)
        return;
	int iNrFacilities = pmFacilities->iFeatures();
    double * Allocation = new double [iNrFacilities]; // keep track of the allocation while we're in the loop
	for (int i = 0; i < iNrFacilities; ++i)
		Allocation[i] = 0;
    double totalScore = 0;
	double totalPartialScore1 = 0;
	double totalPartialScore2 = 0;
	double totalAllocation = 0;
	int iNrDemandPoints = pmDemands->iFeatures();
    for (int demandIndex = 0; demandIndex < iNrDemandPoints; demandIndex++)
    {
		double rDemandCount = (rDemand != 0) ? rDemand[demandIndex] : 1.0; //when an attribute is used to denote how many demands are at that location
		while (rDemandCount > 0)
		{
			double rBestScore = -1;
			double rBestPartialScore1 = 0;
			double rBestPartialScore2 = 0;
			int selectedFacilityIndex = -1;
			for (int chromosomeIndex = 0; chromosomeIndex < iOptimalFacilities; chromosomeIndex++)
			{
				int facilityIndex = chromosome[chromosomeIndex];

				double rPartialScore1 = (context->*scoreFunc1)(demandIndex, facilityIndex);
				double rPartialScore2 = (context->*scoreFunc2)(demandIndex, facilityIndex);
				double rScore = chromosome.w1() * rPartialScore1 + chromosome.w2() * rPartialScore2;

				if ((!fCapacitated) || (Allocation[facilityIndex] < ((rCapacity != 0) ? rCapacity[facilityIndex] : 1.0))) // If a capacity attribute is indicated, respect the maximum capacity of the facility
				{
					if ((selectedFacilityIndex == -1) || (rScore > rBestScore))
					{
						rBestScore = rScore;
						rBestPartialScore1 = rPartialScore1;
						rBestPartialScore2 = rPartialScore2;
						selectedFacilityIndex = facilityIndex;
					}
				}
			}
			if (selectedFacilityIndex != -1)
			{
				double allocated = fCapacitated ? min(((rCapacity != 0) ? rCapacity[selectedFacilityIndex] : 1.0) - Allocation[selectedFacilityIndex], rDemandCount) : rDemandCount;
				Allocation[selectedFacilityIndex] += allocated;
				totalScore += rBestScore * allocated;
				totalPartialScore1 += rBestPartialScore1 * allocated;
				totalPartialScore2 += rBestPartialScore2 * allocated;
				totalAllocation += allocated;
				rDemandCount -= allocated; // The leftover demands will have to be served by another facility
			}
			else
			{
				totalScore = 0; // abandon this chromosome, as it did not reach to a solution
				totalPartialScore1 = 0;
				totalPartialScore2 = 0;
				demandIndex = iNrDemandPoints;
				break;
			}
		}
    }

	chromosome.SetFitness(totalScore / totalAllocation);
	chromosome.SetPartialFitness(totalPartialScore1 / totalAllocation, totalPartialScore2 / totalAllocation);

	delete [] Allocation;
}

long LandAllocation::AddConnections(SegmentMap & segMap, Domain & dm, vector<int> & source, vector<int> & destination, vector<double> & allocations, GAChromosome & chromosome, LandAllocation * context, ScoreFunc scoreFunc1, ScoreFunc scoreFunc2)
{
    if (chromosome.size() == 0)
        return 0;
	DomainUniqueID * pdUid = dm->pdUniqueID();
	long iSegRecord = 0; // increase before adding first segment; segment count starts at 1
	int iNrFacilities = pmFacilities->iFeatures();
    double * Allocation = new double [iNrFacilities]; // keep track of the allocation while we're in the loop
	for (int i = 0; i < iNrFacilities; ++i)
		Allocation[i] = 0;
	int iNrDemandPoints = pmDemands->iFeatures();
    for (int demandIndex = 0; demandIndex < iNrDemandPoints; demandIndex++)
    {
		double rDemandCount = (rDemand != 0) ? rDemand[demandIndex] : 1.0; //when an attribute is used to denote how many demands are at that location
		while (rDemandCount > 0)
		{
			double rBestScore = -1;
			int selectedFacilityIndex = -1;
			for (int chromosomeIndex = 0; chromosomeIndex < iOptimalFacilities; chromosomeIndex++)
			{
				int facilityIndex = chromosome[chromosomeIndex];

				double rScore = (context->*scoreFunc1)(demandIndex, facilityIndex);
				if (fMultiObjective)
					rScore = chromosome.w1() * rScore + chromosome.w2() * (context->*scoreFunc2)(demandIndex, facilityIndex);

				if ((!fCapacitated) || (Allocation[facilityIndex] < ((rCapacity != 0) ? rCapacity[facilityIndex] : 1.0))) // If a capacity attribute is indicated, respect the maximum capacity of the facility
				{
					if ((selectedFacilityIndex == -1) || (rScore > rBestScore))
					{
						rBestScore = rScore;
						selectedFacilityIndex = facilityIndex;
					}
				}
			}
			if (selectedFacilityIndex != -1)
			{
				double allocated = fCapacitated ? min(((rCapacity != 0) ? rCapacity[selectedFacilityIndex] : 1.0) - Allocation[selectedFacilityIndex], rDemandCount) : rDemandCount;
				Allocation[selectedFacilityIndex] += allocated;
				rDemandCount -= allocated; // The leftover demands will have to be served by another facility
				pdUid->iAdd();
				source.push_back(demandIndex + 1);
				destination.push_back(selectedFacilityIndex + 1);
				allocations.push_back(allocated);
				CoordBuf cBuf(2);
				ILWIS::Segment *segCur = CSEGMENT(segMap->newFeature());
				cBuf[0] = cDemands[demandIndex];
				cBuf[1] = cFacilities[selectedFacilityIndex];
				segCur->PutCoords(2, cBuf);
				segCur->PutVal(++iSegRecord);
			}
			else
			{
				demandIndex = iNrDemandPoints;
				break;
			}
		}
    }

	delete [] Allocation;
	return iSegRecord;
}

void LandAllocation::ChromosomeMutator(GAChromosome & chromosome)
{
    GAChromosome newChromosome;
    // copy chromosome into newChromosome
	long Count = pmFacilities->iFeatures();
    for (int i = 0; i < Count; ++i)
        newChromosome.push_back(chromosome[i]);

    int firstRandomIndex = random(iOptimalFacilities) - 1;
    int secondRandomIndex = random(Count) - 1;
    
    // swap the genes at the two indexes
    unsigned int tempGene = newChromosome[firstRandomIndex];
    newChromosome[firstRandomIndex] = newChromosome[secondRandomIndex];
    newChromosome[secondRandomIndex] = tempGene;

    // copy the result back into chromosome
    chromosome.CopyChromosome(newChromosome);
}

void LandAllocation::Initializer(GAChromosome & chromosome)
{
	long Count = pmFacilities->iFeatures();
    bool * PointSel = new bool [Count];
	for (int i = 0; i < Count; ++i)
		PointSel[i] = false;
    bool bStop = false;
    do
    {
        //THIS  algorithm is taking all genes in the chromosome wheareas in your case p number gene out of all genes will create chromosome
        unsigned int iSel = random(Count) - 1;
        if (!PointSel[iSel])
        {
            PointSel[iSel] = true;
            chromosome.push_back(iSel);
        }
		for (int i = 0; i < Count; i++)
        {
            if (!PointSel[i])
                break;
			if (i == Count - 1)
                bStop = true;
        }
    } while (!bStop);
	delete [] PointSel;
}

void LandAllocation::CrossOver(GAChromosome & Dad, GAChromosome & Mum, GAChromosome & child1, GAChromosome & child2)
{
    GreedyCrossOver(Dad, Mum, child1);
    GreedyCrossOver(Mum, Dad, child2);
}

void LandAllocation::GreedyCrossOver(GAChromosome & Dad, GAChromosome & Mum, GAChromosome & child)
{
    int length = Dad.size();
    int MumIndex = -1;
    int DadIndex = -1;
    unsigned int DadGene;
    bool fFound = false;
    //Take random value from dadindex until it also found in mum
    do
    {
        DadIndex = random(length) - 1;
        DadGene = Dad[DadIndex];
        MumIndex = Mum.HasThisGene(DadGene);
        if (MumIndex >= 0)
            fFound = true;
    } while (!fFound);
    if (MumIndex < 0)
        throw new ErrorObject("Gene not found in mum");
	child.push_back(DadGene);
    bool bDadAdded = true;
    bool bMumAdded = true;
    do
    {
        //As long as I can add from dad
        unsigned int obMumGene;
        unsigned int obDadGene;
        if (bDadAdded)
        {
            if (DadIndex > 0)
                DadIndex = DadIndex - 1;
            else
                DadIndex = length - 1;
            obDadGene = Dad[DadIndex];
        }
        //As long as I can add from mum
        if (bMumAdded)
        {
            if (MumIndex < length - 1)
                MumIndex = MumIndex + 1;
            else
                MumIndex = 0;
            obMumGene = Mum[MumIndex];
        }
        if (bDadAdded && child.HasThisGene(obDadGene) < 0)
        {
            //Add to head Dad gene
            child.insert(child.begin(), obDadGene);
        }
        else
            bDadAdded = false;
        if (bMumAdded && child.HasThisGene(obMumGene) < 0)
        {
            //Add to Tail Mum gene
            child.push_back(obMumGene);
        }
        else
            bMumAdded = false;
    } while (bDadAdded || bMumAdded);
    // Add rest of genes by Random Selection
    while (child.size() < length)
    {
        bool bDone = false;
        do
        {
            unsigned int candidateGene = random(length) - 1;
            if (child.HasThisGene(candidateGene) < 0)
            {
				child.push_back(candidateGene);
                bDone = true;
            }
        } while (!bDone);
    }
}
