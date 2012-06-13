#include "LandAllocation.h"
#include "GA.h"
#include "Engine\Base\Algorithm\Random.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Client\TableWindow\CartesianGraphDoc.h"


LandAllocation::LandAllocation(const PointMap& _pmFacilities, const PointMap& _pmFacilitiesNoAttribute, const String& _sColFacilitiesType, const PointMap& _pmDemands, const PointMap& _pmDemandsNoAttribute, const String& _sColDemandsPreference,
							   int _iOptimalFacilities, bool _fCapacitated, int _iStoppingCriteria, long _iGenerations, int _iPopulationSize, int _iNelite, int _iNpareto, double _rMutationPercent, double _rCrossoverPercent)
: pmFacilities(_pmFacilities)
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

void LandAllocation::KeepElite(std::vector<GAChromosome> & solutions, std::vector<GAChromosome> pareto)
{
	for (int i = 0; i < iNelite; ++i)
		solutions.erase(solutions.begin() + random(solutions.size()) - 1); // remove Nelite elements randomly
	for (int i = 0; i < iNelite; ++i) { // add Nelite distinct elements from pareto to solutions
		int index = random(pareto.size()) - 1; // select a random index into the pareto array
		GAChromosome chromosome = pareto[index]; // get the corresponding chromosome
		pareto.erase(pareto.begin() + index); // remove it from the (local!) pareto array
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
	GAAlgorithm->SetCrossover(rCrossoverPercent);
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
