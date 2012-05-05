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
  return "PointMapLandAllocation(PotentialLocationPoints[,PotentialLocationTypes],DemandPoints[,DemandPreference],OptimalFacilities,capacitated|plain,StoppingCriteria,Generations,PopulationSize[,Nelite,Npareto],MutationPercent,CrossoverPercent)";
}

PointMapLandAllocation* PointMapLandAllocation::create(const FileName& fn, PointMapPtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 9 && iParms != 13)
	throw ErrorExpression(sExpr, sSyntax());
  bool fMultiObjective = (iParms == 13);
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
	  int iNelite(atoi(as[9].c_str()));
	  int iNpareto(atoi(as[10].c_str()));
	  double rMutationPercent(atof(as[11].c_str()));
	  double rCrossoverPercent(atof(as[12].c_str()));
	  PointMap pmFacilitiesNoAttribute(fnGetSourceFile(pmFacilities, fn));
  	  PointMap pmDemandsNoAttribute(fnGetSourceFile(pmDemands, fn));
	  return new PointMapLandAllocation(fn, p, pmFacilities, pmFacilitiesNoAttribute, sColFacilitiesType, pmDemands, pmDemandsNoAttribute, sColDemandsPreference, iOptimalFacilities, fCapacitated, iStoppingCriteria, iGenerations, iPopulationSize, iNelite, iNpareto, rMutationPercent, rCrossoverPercent);
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
	  PointMap pmDemandsNoAttribute(fnGetSourceFile(pmDemands, fn));
	  return new PointMapLandAllocation(fn, p, pmFacilities, pmFacilitiesNoAttribute, "", pmDemands, pmDemandsNoAttribute, "", iOptimalFacilities, fCapacitated, iStoppingCriteria, iGenerations, iPopulationSize, 0, 0, rMutationPercent, rCrossoverPercent);
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
							   int _iOptimalFacilities, bool _fCapacitated, int _iStoppingCriteria, long _iGenerations, int _iPopulationSize, int _iNelite, int _iNpareto, double _rMutationPercent, double _rCrossoverPercent)
: PointMapVirtual(fn, p, _pmFacilitiesNoAttribute->cs(),_pmFacilitiesNoAttribute->cb(),_pmFacilitiesNoAttribute->dvrs())
, pmFacilities(_pmFacilities)
, pmFacilitiesNoAttribute(_pmFacilitiesNoAttribute)
, sColFacilitiesType(_sColFacilitiesType)
, pmDemands(_pmDemands)
, pmDemandsNoAttribute(_pmDemandsNoAttribute)
, sColDemandsPreference(_sColDemandsPreference)
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
  if (pmFacilitiesNoAttribute->fTblAttSelf())
    SetAttributeTable(pmFacilitiesNoAttribute->tblAtt());
}

void PointMapLandAllocation::Store()
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
		return String("PointMapLandAllocation(%S,%S,%S,%S,%d,%s,%d,%d,%d,%d,%d,%f,%f)", pmFacilities->sNameQuoted(false, fnObj.sPath()), sColFacilitiesType, pmDemands->sNameQuoted(false, fnObj.sPath()), sColDemandsPreference, iOptimalFacilities, fCapacitated ? "capacitated" : "plain", iStoppingCriteria, iGenerations, iPopulationSize, iNelite, iNpareto, rMutationPercent, rCrossoverPercent);
	else
		return String("PointMapLandAllocation(%S,%S,%d,%s,%d,%d,%d,%f,%f)", pmFacilities->sNameQuoted(false, fnObj.sPath()), pmDemands->sNameQuoted(false, fnObj.sPath()), iOptimalFacilities, fCapacitated ? "capacitated" : "plain", iStoppingCriteria, iGenerations, iPopulationSize, rMutationPercent, rCrossoverPercent);
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

	LandAllocation la (pmFacilities, pmFacilitiesNoAttribute, sColFacilitiesType, pmDemands, pmDemandsNoAttribute, sColDemandsPreference,
							   iOptimalFacilities, fCapacitated, iStoppingCriteria, iGenerations, iPopulationSize, iNelite, iNpareto, rMutationPercent, rCrossoverPercent);
	GAChromosome * chromosome = la.PerformLandAllocation(trq);
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
	colBestFitness->PutBufVal(la.vrFitnessList, 1);
	colAvgFitness->PutBufVal(la.vrPopulationAvgList, 1);

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
		ScoreFunc scoreFunc1 = (ScoreFunc)&LandAllocation::rStdDistanceFunc;
		ScoreFunc scoreFunc2 = (ScoreFunc)&LandAllocation::rStdPreferenceFunc;
		unsigned long iNrSegments = AddConnections(segMap, dmConnections, source, destination, allocations, *chromosome, &la, scoreFunc1, scoreFunc2);
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

long PointMapLandAllocation::AddConnections(SegmentMap & segMap, Domain & dm, vector<int> & source, vector<int> & destination, vector<double> & allocations, GAChromosome & chromosome, LandAllocation * context, ScoreFunc scoreFunc1, ScoreFunc scoreFunc2)
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
		double rDemandCount = (context->rDemand != 0) ? context->rDemand[demandIndex] : 1.0; //when an attribute is used to denote how many demands are at that location
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

				if ((!fCapacitated) || (Allocation[facilityIndex] < ((context->rCapacity != 0) ? context->rCapacity[facilityIndex] : 1.0))) // If a capacity attribute is indicated, respect the maximum capacity of the facility
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
				double allocated = fCapacitated ? min(((context->rCapacity != 0) ? context->rCapacity[selectedFacilityIndex] : 1.0) - Allocation[selectedFacilityIndex], rDemandCount) : rDemandCount;
				Allocation[selectedFacilityIndex] += allocated;
				rDemandCount -= allocated; // The leftover demands will have to be served by another facility
				pdUid->iAdd();
				source.push_back(demandIndex + 1);
				destination.push_back(selectedFacilityIndex + 1);
				allocations.push_back(allocated);
				CoordBuf cBuf(2);
				ILWIS::Segment *segCur = CSEGMENT(segMap->newFeature());
				cBuf[0] = context->cDemands[demandIndex];
				cBuf[1] = context->cFacilities[selectedFacilityIndex];
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


