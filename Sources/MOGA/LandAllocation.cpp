#include "LandAllocation.h"
#include "GA.h"
#include "Engine\Base\Algorithm\Random.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Client\TableWindow\CartesianGraphDoc.h"
#include "Engine\Applications\SEGVIRT.H"
#include "PointMapLandAllocation.h"
#include "Engine\Table\tbl2dim.h"


LandAllocation::LandAllocation(const PointMap& _pmFacilities, const PointMap& _pmFacilitiesNoAttribute, const String& _sColFacilitiesType, const PointMap& _pmDemands, const PointMap& _pmDemandsNoAttribute, const String& _sColDemandsPreference, const String& _sODMatrix,
							   int _iOptimalFacilities, bool _fCapacitated, int _iStoppingCriteria, long _iGenerations, int _iPopulationSize, int _iNelite, int _iNpareto, double _rMutationPercent, double _rCrossoverPercent)
: pmFacilities(_pmFacilities)
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
, cFacilities(0)
, cDemands(0)
, rDemand(0)
, rCapacity(0)
, rMinDistance(rUNDEF)
, rMaxDistance(rUNDEF)
{
  fMultiObjective = (sColFacilitiesType.length() > 0) && (sColDemandsPreference.length() > 0);
  Init();
  ScoreFunc scoreFunc1 = (ScoreFunc)&LandAllocation::rStdDistanceFunc;
  ScoreFunc scoreFunc2 = (ScoreFunc)&LandAllocation::rStdPreferenceFunc;
  GAAlgorithm = new GA(this, fMultiObjective ? (FitnessFunc)&LandAllocation::FitnessMO : (FitnessFunc)&LandAllocation::FitnessSO, scoreFunc1, scoreFunc2);
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
	if (GAAlgorithm)
		delete GAAlgorithm;
}

void LandAllocation::Init()
{
  int iNrDemandPoints = pmDemands->iFeatures();
  int iNrFacilities = pmFacilities->iFeatures();

  cDemands = new Coord [iNrDemandPoints];
  for (int i = 0; i < iNrDemandPoints; ++i)
	  cDemands[i] = pmDemands->cValue(i);
  cFacilities = new Coord [iNrFacilities];
  for (int i = 0; i < iNrFacilities; ++i)
	  cFacilities[i] = pmFacilities->cValue(i);
  
  rDistanceOD.resize(iNrDemandPoints);
  if (sODMatrix != "") {
	  Table2Dim table (sODMatrix);
	  if (table->dm1() == pmDemandsNoAttribute->dm() && table->dm2() == pmFacilitiesNoAttribute->dm()) {
		  for (int demandIndex = 0; demandIndex < iNrDemandPoints; ++demandIndex) {
			  rDistanceOD[demandIndex].resize(iNrFacilities);
			  for (int facilityIndex = 0; facilityIndex < iNrFacilities; ++facilityIndex) {
				  double rDistance = table->rValue(demandIndex + 1, facilityIndex + 1);
				  rDistanceOD[demandIndex][facilityIndex] = rDistance;
				  if (rDistance != rUNDEF) {
					  if (rMinDistance == rUNDEF) {
						  rMinDistance = rDistance;
						  rMaxDistance = rDistance;
					  } else {
						  rMinDistance = min(rMinDistance, rDistance);
						  rMaxDistance = max(rMaxDistance, rDistance);
					  }
				  }
			  }
		  }
	  } else if (table->dm2() == pmDemandsNoAttribute->dm() && table->dm1() == pmFacilitiesNoAttribute->dm()) {
		  for (int demandIndex = 0; demandIndex < iNrDemandPoints; ++demandIndex) {
			  rDistanceOD[demandIndex].resize(iNrFacilities);
			  for (int facilityIndex = 0; facilityIndex < iNrFacilities; ++facilityIndex) {
				  double rDistance = table->rValue(facilityIndex + 1, demandIndex + 1);
				  rDistanceOD[demandIndex][facilityIndex] = rDistance;
				  if (rDistance != rUNDEF) {
					  if (rMinDistance == rUNDEF) {
						  rMinDistance = rDistance;
						  rMaxDistance = rDistance;
					  } else {
						  rMinDistance = min(rMinDistance, rDistance);
						  rMaxDistance = max(rMaxDistance, rDistance);
					  }
				  }
			  }
		  }
	  } else { // should never happen!! (OD matrix does not "fit" the selected pointmaps)
		  for (int demandIndex = 0; demandIndex < iNrDemandPoints; ++demandIndex) {
			  rDistanceOD[demandIndex].resize(iNrFacilities);
			  for (int facilityIndex = 0; facilityIndex < iNrFacilities; ++facilityIndex) {
				  rDistanceOD[demandIndex][facilityIndex] = rUNDEF;
			  }
		  }
		  rMinDistance = rUNDEF;
		  rMaxDistance = rUNDEF;
	  }
  } else {
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
  }

  if (fMultiObjective) {
	  vector<String> sDemandsPreference;
	  sDemandsPreference.resize(iNrDemandPoints);
	  if (pmDemandsNoAttribute.fValid() && pmDemandsNoAttribute->fTblAtt()) {
		  Table tbl = pmDemandsNoAttribute->tblAtt();
		  Column col = tbl->col(sColDemandsPreference);
		  for (int i = 0; i < iNrDemandPoints; ++i)
			  sDemandsPreference[i] = col->sValue(pmDemandsNoAttribute->iRaw(i), 0);
	  }
	  vector <String> sFacilitiesType;
	  sFacilitiesType.resize(iNrFacilities);
	  if (pmFacilitiesNoAttribute.fValid() && pmFacilitiesNoAttribute->fTblAtt()) {
		  Table tbl = pmFacilitiesNoAttribute->tblAtt();
		  Column col = tbl->col(sColFacilitiesType);
		  for (int i = 0; i < iNrFacilities; ++i)
			  sFacilitiesType[i] = col->sValue(pmFacilitiesNoAttribute->iRaw(i), 0);
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

void LandAllocation::UpdatePareto(std::vector<GAChromosome> & solutions, std::vector<GAChromosome> & pareto)
{
	for (std::vector<GAChromosome>::iterator it = solutions.begin(); it != solutions.end(); ++it)
		pareto.push_back(*it); // append the chromosomes from solutions to the existing pareto
	std::sort(pareto.begin(), pareto.end()); // sort by fitness (largest fitness at the beginning)
	if (pareto.size() > iNpareto)
		pareto.resize(iNpareto); // crop pareto array
}

void LandAllocation::KeepElite(std::vector<GAChromosome> & solutions, std::vector<GAChromosome> & pareto)
{
	for (int i = 0; i < iNelite; ++i)
		solutions.erase(solutions.begin() + random(solutions.size()) - 1); // remove Nelite elements randomly
	for (int i = 0; i < iNelite; ++i) { // add Nelite distinct elements from pareto to solutions
		int index = random(pareto.size()) - 1; // select a random index into the pareto array
		GAChromosome chromosome = pareto[index]; // get the corresponding chromosome
		pareto.erase(pareto.begin() + index); // remove it from the pareto array
		solutions.push_back(chromosome); // add it to the solutions
	}
	std::sort(solutions.begin(), solutions.end()); // sort by fitness (largest fitness at the beginning)
}

std::vector<GAChromosome> LandAllocation::GenerateParetoArray(Tranquilizer & trq)
{
	seedrand(clock());

	trq.SetTitle(String(TR("Finding Pareto Solutions in '%S'").c_str(), pmFacilities->sName()));

	GAAlgorithm->SetSelectionType(fMultiObjective ? GA::Probability : GA::Tournament);
	GAAlgorithm->SetStoppingCriteria(iStoppingCriteria);
	GAAlgorithm->SetGenerations(iGenerations);
	GAAlgorithm->SetPopulationSize(iPopulationSize);
	GAAlgorithm->SetMutation(rMutationPercent);
	GAAlgorithm->SetCrossover(100); // 100% crossover: make every new generation be different than the current one (having this < 100 causes duplicates to appear in the solutions array)
	GAAlgorithm->SetApplyElitism(false); // we have our own elitism here (having this "true" causes duplicates to appear in the solutions array)
	GAAlgorithm->Initialize();
	std::vector<GAChromosome> pareto;
	int iGenerationNr = 0;
	std::vector<GAChromosome> & solutions = GAAlgorithm->GetCurrentGeneration();
	UpdatePareto(solutions, pareto);
	while (!GAAlgorithm->IsDone())
	{
        GAAlgorithm->CreateNextGeneration();
		solutions = GAAlgorithm->GetCurrentGeneration();
		KeepElite(solutions, pareto);
		UpdatePareto(solutions, pareto);
	    if (trq.fUpdate(iGenerationNr, GAAlgorithm->GetGenerations()))
			return pareto;
        ++iGenerationNr;
	}
	trq.fUpdate(GAAlgorithm->GetGenerations(), GAAlgorithm->GetGenerations());
	return pareto;
}

GAChromosome * LandAllocation::PerformLandAllocation(Tranquilizer & trq)
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

	GAAlgorithm->SetSelectionType(fMultiObjective ? GA::Probability : GA::Tournament);
	GAAlgorithm->SetStoppingCriteria(iStoppingCriteria);
	GAAlgorithm->SetGenerations(iGenerations);
	GAAlgorithm->SetPopulationSize(iPopulationSize);
	GAAlgorithm->SetMutation(rMutationPercent);
	GAAlgorithm->SetCrossover(rCrossoverPercent);
	GAAlgorithm->Initialize();
	vrFitnessList.Size(iGenerations);
	vrPopulationAvgList.Size(iGenerations);
	int iGenerationNr = 0;
	while (!GAAlgorithm->IsDone())
	{
		vrFitnessList[iGenerationNr] = GAAlgorithm->rGetBestValue(iGenerationNr);
		vrPopulationAvgList[iGenerationNr] = (GAAlgorithm->rGetAverageValue(iGenerationNr));
        GAAlgorithm->CreateNextGeneration();
	    if (trq.fUpdate(iGenerationNr, GAAlgorithm->GetGenerations()))
			return 0 ;
        ++iGenerationNr;
	}
	trq.fUpdate(GAAlgorithm->GetGenerations(), GAAlgorithm->GetGenerations());
	double rBestFitness = vrFitnessList[GAAlgorithm->GetGenerations() - 1];
	for (int iGenerationNr = 0; iGenerationNr < GAAlgorithm->GetGenerations(); ++iGenerationNr)
	{
		vrFitnessList[iGenerationNr] /= rBestFitness;
		vrPopulationAvgList[iGenerationNr] /= rBestFitness;
	}
    GAChromosome * chromosome = GAAlgorithm->GetBestChromosome();
	return chromosome;
}

double LandAllocation::rStdDistanceFunc(int demandIndex, int facilityIndex)
{
	double distanceFacilityDemand = rDistanceOD[demandIndex][facilityIndex];
	if (distanceFacilityDemand != rUNDEF) {
		//double rScore = 1 - distanceFacilityDemand / rMaxDistance + rMinDistance / rMaxDistance; // MAXIMUM
		//double rScore = 1 / distanceFacilityDemand; // ORIGINAL FORMULA (not standardized to [0..1])
		double rScore = 1 - (distanceFacilityDemand - rMinDistance) / (rMaxDistance - rMinDistance); // INTERVAL
		return rScore;
	} else
		return 0; // rUNDEF distance scores "0" for the std-distance (it should actually be "worse" than rMaxDistance, but this implicates that constraints must be introduced).
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

void LandAllocation::ChromosomeMutator(GAChromosome & chromosome)
{
    int firstRandomIndex = random(iOptimalFacilities) - 1; // random index within the optimal facilities
	int secondRandomIndex = random(chromosome.size()) - 1; // random index within the whole chromosome
    
    // swap the genes at the two indexes
    unsigned int tempGene = chromosome[firstRandomIndex];
    chromosome[firstRandomIndex] = chromosome[secondRandomIndex];
    chromosome[secondRandomIndex] = tempGene;
}

void LandAllocation::Initializer(GAChromosome & chromosome)
{
	long Count = pmFacilities->iFeatures();
	for (int i = 0; i < Count; ++i)
		chromosome.push_back(i);
	std::random_shuffle(chromosome.begin(), chromosome.end());
}

void LandAllocation::CrossOver(GAChromosome & Dad, GAChromosome & Mum, GAChromosome & child1, GAChromosome & child2)
{
    GreedyCrossOver(Dad, Mum, child1);
    GreedyCrossOver(Mum, Dad, child2);
}

void LandAllocation::GreedyCrossOver(GAChromosome & Dad, GAChromosome & Mum, GAChromosome & child)
{
    int length = Dad.size();

	GAChromosome remainingGenes;
	for (int i = 0; i < length; ++i)
		remainingGenes.push_back(i);
    // Take random gene from first iOptimalFacilities from dad
	GAChromosome::iterator dadIterator = Dad.begin() + random(iOptimalFacilities) - 1;
	// Locate the gene in mom .. by definition it should be there
	GAChromosome::iterator mumIterator = find(Mum.begin(), Mum.end(), *dadIterator);
	assert (mumIterator != Mum.end());
	// This gene becomes the first element of child
	child.push_back(*dadIterator);
	remainingGenes.erase(find(remainingGenes.begin(), remainingGenes.end(), *dadIterator));
    bool bDadAdded = true;
    bool bMumAdded = true;
    do
    {
		if (bDadAdded) { // Add from dad as long as possible
			if (dadIterator != Dad.begin())
				--dadIterator;
			else
				dadIterator = Dad.end() - 1;

			GAChromosome::iterator it = find(remainingGenes.begin(), remainingGenes.end(), *dadIterator);
			if (it != remainingGenes.end()) {
				child.insert(child.begin(), *dadIterator); //Add to head Dad gene
				remainingGenes.erase(it);
			} else
				bDadAdded = false;
		}
		if (bMumAdded) { // Add from mum as long as possible
			++mumIterator;
			if (mumIterator == Mum.end())
				mumIterator = Mum.begin();

			GAChromosome::iterator it = find(remainingGenes.begin(), remainingGenes.end(), *mumIterator);
			if (it != remainingGenes.end()) {
				child.push_back(*mumIterator); //Add to Tail Mum gene
				remainingGenes.erase(it);
			} else
				bMumAdded = false;
		}
    } while (bDadAdded || bMumAdded);
    // Add rest of genes by Random Selection
	std::random_shuffle(remainingGenes.begin(), remainingGenes.end());
	for (int i = 0; i < remainingGenes.size(); ++i)
		child.push_back(remainingGenes[i]);
	assert(child.size() == length);
}

void LandAllocation::StoreParetoInTable(std::vector<GAChromosome> & pareto, FileName & fnParetoTable)
{
	Domain paretoDom (FileName(fnParetoTable, ".dom", true), pareto.size(), dmtUNIQUEID);
	Table paretoTbl (fnParetoTable, paretoDom);
	Column colFacilities (paretoTbl, "Facilities", Domain("String"));
	Column colF (paretoTbl, "F", DomainValueRangeStruct(0, 1, 0));
	Column colF1 (paretoTbl, "f1", DomainValueRangeStruct(0, 1, 0));
	Column colF2 (paretoTbl, "f2", DomainValueRangeStruct(0, 1, 0));
	Column colW1 (paretoTbl, "w1", DomainValueRangeStruct(0, 1, 0));
	Column colW2 (paretoTbl, "w2", DomainValueRangeStruct(0, 1, 0));

	int prefixSize = 0;
	if (pmFacilitiesNoAttribute->dm().fValid()) {
		DomainSort * pds = pmFacilitiesNoAttribute->dm()->pdsrt();
		if (pds) {
			String sPrefix = pds->sGetPrefix();
			prefixSize = sPrefix.size() + 1;
		}
	}

	for (int i = 0; i < pareto.size(); ++i) {
		GAChromosome chromosome = pareto[i];
		String sFacilities = pmFacilitiesNoAttribute->sValue(chromosome[0], 0).substr(prefixSize);
		for (int j = 1; j < iOptimalFacilities; ++j)
			sFacilities = sFacilities + "," + pmFacilitiesNoAttribute->sValue(chromosome[j], 0).substr(prefixSize);
		colFacilities->PutVal(i + 1, sFacilities);
		colF->PutVal(i + 1, chromosome.rGetFitness());
		colF1->PutVal(i + 1, chromosome.rGetPartialScore1());
		colF2->PutVal(i + 1, chromosome.rGetPartialScore2());
		colW1->PutVal(i + 1, chromosome.w1());
		colW2->PutVal(i + 1, chromosome.w2());
	}
}

void LandAllocation::StoreChromosome(GAChromosome * chromosome, PointMapPtr * pntMapPtr)
{	
	if (chromosome != 0)
	{
		for (int i = 0; i < iOptimalFacilities; i++)
			pntMapPtr->iAddRaw(pmFacilities->cValue(chromosome->at(i)), i + 1); // chromosome array values are 0-based, pm->cValue() is 0-based
	}

	if (!fMultiObjective) {
		// Create Fitness Table

		FileName fnFitness (pntMapPtr->fnObj, ".tbt", true);
		fnFitness.sFile += "_fitness";
		Domain fitnessDom (FileName(fnFitness, ".dom", true), iGenerations, dmtUNIQUEID);
		Table fitnessTbl (fnFitness, fitnessDom);
		Column colBestFitness (fitnessTbl, "BestFitness", DomainValueRangeStruct(0, 1, 0));
		Column colAvgFitness (fitnessTbl, "AvgFitness", DomainValueRangeStruct(0, 1, 0));
		colBestFitness->PutBufVal(vrFitnessList, 1);
		colAvgFitness->PutBufVal(vrPopulationAvgList, 1);

		// Create Fitness Graph

		CartesianGraphDoc cgd;
		cgd.CreateNewGraph(fitnessTbl, Column(), colBestFitness, "Continuous", Color(0, 255, 0));
		cgd.AddColumnGraph(colAvgFitness, "Continuous", Color(255, 0, 255));
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
				return;
			}
			END_CATCH_ALL
		}
	}

	// Create Connections Segment Map
	if (chromosome != 0)
	{
		FileName fnConnections (pntMapPtr->fnObj, ".mps", true);
		fnConnections.sFile += "_connections";
		Domain dmConnections (FileName(fnConnections, ".dom", true), 0, dmtUNIQUEID);
		DomainIdentifier* dmIdentifierPtr = dmConnections->pdid();

		CoordSystem csyDest (pmFacilities->cs());
		CoordBounds cbMap (pmDemands->cb());
		cbMap += pmFacilities->cb();
		SegmentMap segMap(fnConnections, csyDest, cbMap, dmConnections);
		vector<int> source; // source and destination array values are 0-based
		vector<int> destination;
		vector<double> allocations;
		ScoreFunc scoreFunc1 = (ScoreFunc)&LandAllocation::rStdDistanceFunc;
		ScoreFunc scoreFunc2 = (ScoreFunc)&LandAllocation::rStdPreferenceFunc;
		unsigned long iNrSegments = AddConnections(pmFacilities, pmDemands, iOptimalFacilities, fMultiObjective, fCapacitated, segMap, dmConnections, source, destination, allocations, *chromosome, scoreFunc1, scoreFunc2);

		// Create the Attribute Table for the Segment Map
		LongBuf lbSource (iNrSegments);
		LongBuf lbDestination (iNrSegments);
		RealBuf rbAllocations (iNrSegments);
		for (unsigned long i = 0; i < iNrSegments; ++i)
		{
			lbSource[i] = source[i] + 1; // lbSource and lbDestination are 1-based (containing iRaw's)
			lbDestination[i] = destination[i] + 1;
			rbAllocations[i] = allocations[i];
		}

		Table connectionTbl = Table(FileName(fnConnections, ".tbt", true), dmConnections);

		PointMap pmDemandsNoAttribute (PointMapLandAllocation::fnGetSourceFile(pmDemands, pntMapPtr->fnObj));
		Column colSource (connectionTbl, "DemandID", pmDemandsNoAttribute->dm());
		colSource->PutBufRaw(lbSource, 1);
		Column colDestination (connectionTbl, "FacilityID", pmFacilitiesNoAttribute->dm());
		colDestination->PutBufRaw(lbDestination, 1);
		Column colAllocations (connectionTbl, "Allocated", DomainValueRangeStruct(0, DBL_MAX, 0));
		colAllocations->PutBufVal(rbAllocations, 1);

		segMap->SetAttributeTable(connectionTbl);

		// Create the Attribute Table for the current chromosome's Point Map

		Table pntMapTbl = Table(FileName(pntMapPtr->fnObj, ".tbt", true), pntMapPtr->dm());

		Column colOriginalFeature (pntMapTbl, "FacilityID", pmFacilitiesNoAttribute->dm());
		for (unsigned long i = 0; i < iOptimalFacilities; ++i)
			colOriginalFeature->PutVal(i + 1, pmFacilitiesNoAttribute->sValue(chromosome->at(i), 0)); // pm->sValue() is 0-based

		if (fCapacitated) {
			RealBuf rbCapacity (iOptimalFacilities);
			for (unsigned long i = 0; i < iOptimalFacilities; ++i)
				rbCapacity[i] = pmFacilities->rValue(chromosome->at(i));
			Column colCapacity (pntMapTbl, "Capacity", pmFacilities->dm());
			colCapacity->PutBufVal(rbCapacity, 1);
		}

		int iNrFacilities = pmFacilities->iFeatures();
		vector<double> allocationsGrouped;
		vector<double> distancesGrouped;
		allocationsGrouped.resize(iNrFacilities);
		distancesGrouped.resize(iNrFacilities);
		for (unsigned long i = 0; i < iNrFacilities; ++i) {
			allocationsGrouped[i] = 0;
			distancesGrouped[i] = 0;
		}
		for (unsigned long i = 0; i < iNrSegments; ++i) {
			allocationsGrouped[destination[i]] += allocations[i];
			double rDistance = rDistanceOD[source[i]][destination[i]];
			if (rDistance == rUNDEF)
				rDistance = rMaxDistance; // anyway this is how distance=rUNDEF was "standardized" (see rStdDistanceFunc)
			distancesGrouped[destination[i]] += allocations[i] * rDistance;
		}

		RealBuf rbAllocationsGrouped (iOptimalFacilities);
		RealBuf rbDistancesGrouped (iOptimalFacilities);
		for (unsigned long i = 0; i < iOptimalFacilities; ++i) {
			rbAllocationsGrouped[i] = allocationsGrouped[chromosome->at(i)];
			rbDistancesGrouped[i] = distancesGrouped[chromosome->at(i)];
		}
		Column colAllocationsGrouped (pntMapTbl, "Allocated", DomainValueRangeStruct(0, DBL_MAX, 0));
		colAllocationsGrouped->PutBufVal(rbAllocationsGrouped, 1);
		Column colDistancesGrouped (pntMapTbl, "TotalDistance", DomainValueRangeStruct(0, DBL_MAX, 0));
		colDistancesGrouped->PutBufVal(rbDistancesGrouped, 1);

		if (fMultiObjective && pmFacilitiesNoAttribute.fValid() && pmFacilitiesNoAttribute->fTblAtt()) {
			Table tbl = pmFacilitiesNoAttribute->tblAtt();
			Column col = tbl->col(sColFacilitiesType);
			Column colFacilitiesType (pntMapTbl, col->sName(), col->dvrs());
			for (unsigned long i = 0; i < iOptimalFacilities; ++i)
				colFacilitiesType->PutVal(i + 1, col->sValue(chromosome->at(i) + 1, 0)); // col->sValue() and col->PutVal() are 1-based
		}

		pntMapPtr->SetAttributeTable(pntMapTbl);
	}
}

long LandAllocation::AddConnections(PointMap & pmFacilities, PointMap & pmDemands, int iOptimalFacilities, bool fMultiObjective, bool fCapacitated, SegmentMap & segMap, Domain & dm, vector<int> & source, vector<int> & destination, vector<double> & allocations, GAChromosome & chromosome, ScoreFunc scoreFunc1, ScoreFunc scoreFunc2)
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

				double rScore = (this->*scoreFunc1)(demandIndex, facilityIndex);
				if (fMultiObjective)
					rScore = chromosome.w1() * rScore + chromosome.w2() * (this->*scoreFunc2)(demandIndex, facilityIndex);

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
				source.push_back(demandIndex);
				destination.push_back(selectedFacilityIndex);
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
