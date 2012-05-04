#include "GA.h"
#include "LandAllocation.h"
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\Random.h"
#include <algorithm> // std::sort
		
GA::GA(LandAllocation * _context, FitnessFunc _fitnessFunc, ScoreFunc _scoreFunc1, ScoreFunc _scoreFunc2)
: context(_context)
, fitnessFunc(_fitnessFunc)
, scoreFunc1(_scoreFunc1)
, scoreFunc2(_scoreFunc2)
, m_dMutation(0)
, m_lGenerations(0)
, m_dCrossOver(0)
, m_usPopSize(0)
, m_dTotalFitness(0)
, m_bApplyElitism(true)
, m_usGen(0)	 
, m_SelType(Tournament)
, StoppingCriteria(0)
, TopFitness(0)
, temp_best(0)
, m_chro_TopFitness(0)
, tempBest_m_chro(0)
, m_bestValue(0)
, m_totalValue(0)
{
}

GA::~GA()
{
	if (m_bestValue != 0)
		delete [] m_bestValue;
	if (m_totalValue != 0)
		delete [] m_totalValue;
}

void GA::Initialize()
{
	m_bestValue = new double[m_lGenerations];
	m_totalValue = new double[m_lGenerations];

	for (int i = 0; i < m_lGenerations; ++i)
	{
		m_bestValue[i] = 0;
		m_totalValue[i] = 0;
	}

	m_thisGeneration.clear();

	if (m_usPopSize > 0)
	{
		for (int i = 0; i < m_usPopSize; ++i)
		{
			GAChromosome newParent (context, fitnessFunc, scoreFunc1, scoreFunc2, true);
			m_thisGeneration.push_back(newParent);
		}

		m_dTotalFitness = 0;

		for (int i = 0; i < m_usPopSize; i++)
			m_dTotalFitness += m_thisGeneration[i].rGetFitness();
		
		std::sort(m_thisGeneration.begin(), m_thisGeneration.end());

		temp_best = m_thisGeneration[0].rGetFitness();
		tempBest_m_chro = &m_thisGeneration[0];
		if (TopFitness < temp_best)
		{
			TopFitness = temp_best;
			m_chro_TopFitness = &m_thisGeneration[0];
		}
	}
}
		
void GA::push(double value)
{
	if (m_stack.size() != StoppingCriteria)
		m_stack.push(value);
}

void GA::deleteStack()
{
	int size = m_stack.size();
	for (int i = 0; i < size; ++i)
		m_stack.pop();
}

// Sort the Chromosomes according to their fitness
void GA::RankPopulation()
{
	m_dTotalFitness = 0;

	for (int i = 0; i < m_usPopSize; ++i)
		m_dTotalFitness += m_thisGeneration[i].rGetFitness();
	
	std::sort(m_thisGeneration.begin(), m_thisGeneration.end());

	temp_best = m_thisGeneration[0].rGetFitness();
	tempBest_m_chro = &m_thisGeneration[0];
	if (TopFitness < temp_best)
	{
		TopFitness = temp_best;
		m_chro_TopFitness = &m_thisGeneration[0];
	}
}

void GA::CreateNextGeneration()
{
	std::vector<GAChromosome> m_NextGeneration;

	// Best Chromosome always exists in the new generation	
	GAChromosome * bestChromo = 0;

	// local elitism applied
	if (m_bApplyElitism)
		bestChromo = &m_thisGeneration[0];

	if ((0 != bestChromo) && (0 != m_chro_TopFitness) && (bestChromo->rGetFitness() < m_chro_TopFitness->rGetFitness()))
		bestChromo =  m_chro_TopFitness;

	for (int i = 0 ; i < m_usPopSize ; i+=2)
	{
		//Step 1 Selection								
		int iDadParent = 0 ;
		int iMumParent = 0 ;
		switch(m_SelType)
		{
			case Mix:
				iDadParent = TournamentSelection();
				iMumParent = RandomSelection();
				break;
			case Tournament: 
				iDadParent = TournamentSelection();
				iMumParent = TournamentSelection();
				break;
			case Random:
				iDadParent = RandomSelection();
				iMumParent = RandomSelection();
				break;
			case Probability:
				iDadParent = ProbabilitySelection();
				iMumParent = ProbabilitySelection();
		}
							
		GAChromosome Dad = m_thisGeneration[iDadParent];
		GAChromosome Mum = m_thisGeneration[iMumParent];

		GAChromosome child1 (context, fitnessFunc, scoreFunc1, scoreFunc2, false);
		GAChromosome child2 (context, fitnessFunc, scoreFunc1, scoreFunc2, false);
			
		//Step 2  Cross Over		
		if (random() < m_dCrossOver)
			context->CrossOver(Dad, Mum, child1, child2);
		else
		{
			child1 = Dad;
			child2 = Mum; 
		}
		
		//Mutation or not Mutation
		if (random() < m_dMutation)
		{
			context->ChromosomeMutator(child1);
			context->ChromosomeMutator(child2);
		}
		
		//Calculate the new fitness
		(context->*fitnessFunc)(child1, context, scoreFunc1, scoreFunc2);
		(context->*fitnessFunc)(child2, context, scoreFunc1, scoreFunc2);

		m_NextGeneration.push_back(child1);
		m_NextGeneration.push_back(child2);
	}

	//// APPLYING LOCAL ELITISM TAKING ONLY THE BEST OF CURRENT GENERATION
	if (0 != bestChromo)
		m_NextGeneration.insert(m_NextGeneration.begin(), *bestChromo);

	m_thisGeneration.clear();			
	// now this generation is the new generation					
	for(int j = 0 ; j < m_usPopSize; j++) 
		m_thisGeneration.push_back(m_NextGeneration[j]);

	RankPopulation(); // population rank and print POULATION
	m_usGen++;
}

int GA::RandomSelection()
{
	 return random(m_usPopSize) - 1; // RANDOM SELECTION OF index
}

int GA::RouletteSelection()
{
	//dont like the 
	//Rs  Random fitness											
	double randomFitness = random() * TopFitness; // RANDOM FITNESS DOES THE SELECTION OF FITNESS
	int idx = -1;
	int first = 0;
	int last = m_usPopSize - 1;
	int mid = (last - first)/2;

	//  ArrayList's BinarySearch is for exact values only			
	//  so do this by hand.											
	while (idx == -1 && first <= last) // HERE IT TRIES TO HAVE A MATCH WITH RANDOM FITNESS USING BINARY SEARCH
	{
		double fitness = m_thisGeneration[mid].rGetFitness();
		if (randomFitness < fitness )
			last = mid;
		else if (randomFitness > fitness )
			first = mid;
		mid = (first + last)/2;
		//  lies between i and i+1
		if ((last - first) == 1)
			idx = last;
	}
	return idx;
}

int GA::TournamentSelection()
{
	int Count = 1;
	if (m_usPopSize >=100)
		Count = 8 ;
	else if (m_usPopSize >=50)
		Count = 6 ;
	else if (m_usPopSize >=30)
		Count = 4 ;
	else if (m_usPopSize >=10)
		Count = 3 ;
	else if (m_usPopSize >=2)
		Count = 2;

	int finalindex = 0 ;
	double dMaxFit = 0; 
	for(int i = 0; i < Count ; ++i)
	{
		int selIndex = random(m_usPopSize) - 1;
		double fitness = m_thisGeneration[selIndex].rGetFitness();
		if(fitness > dMaxFit)
		{
			finalindex	= i;
			dMaxFit		= fitness;
		}
	}
	return finalindex;	
}

int GA::ProbabilitySelection()
{
	double rFitnessMin = m_thisGeneration[0].rGetFitness();
	for (int i = 1; i < m_usPopSize; ++i)
		rFitnessMin = min(rFitnessMin, m_thisGeneration[i].rGetFitness());
	double rSumDeltaFitness = 0;
	for (int i = 0; i < m_usPopSize; ++i)
		rSumDeltaFitness += m_thisGeneration[i].rGetFitness() - rFitnessMin;
	std::vector<double> cumulativeProbabilities;
	cumulativeProbabilities.push_back((m_thisGeneration[0].rGetFitness() - rFitnessMin) / rSumDeltaFitness);
	for (int i = 1; i < m_usPopSize; ++i) {
		double rProbability = (m_thisGeneration[i].rGetFitness() - rFitnessMin) / rSumDeltaFitness;
		cumulativeProbabilities.push_back(cumulativeProbabilities[i - 1] + rProbability);
	}
	double rndValue = random() * cumulativeProbabilities[m_usPopSize - 1];
	int i = 0;
	while ((cumulativeProbabilities[i] < rndValue) && (i < m_usPopSize - 1))
		++i;
	return i;
}

bool GA::IsDone()
{
	if (m_usGen < m_lGenerations)
	{
		m_bestValue[m_usGen] = TopFitness;
		m_totalValue[m_usGen] = m_dTotalFitness;

		// Checking repeatation of value in n generation through stack

		if (StoppingCriteria > 1)
		{
			if (m_stack.size() == 0) // always pick a value when stack is free
				push(temp_best);

			if (m_stack.size() > 0) // use push pop
			{
				double value = m_stack.top();
				if (value == temp_best)
					push(temp_best);
				else
					deleteStack();
			}

			if (m_stack.size() == StoppingCriteria) // check is there any match to stop
				return true;
		}
		return false;
	}
	return true;
}

GAChromosome * GA::GetBestChromosome()
{
	return m_chro_TopFitness;
}

void GA::SetSelectionType(GA::Selection value)
{
	m_SelType = value;
}

GA::Selection GA::GetSelectionType()
{
	return m_SelType;
}

void GA::SetMutation(double value)
{
	m_dMutation = value;
}

double GA::GetMutation()
{
	return m_dMutation;
}

void GA::SetCrossover(double value)
{
	m_dCrossOver = value;
}

double GA::GetCrossover()
{
	return m_dCrossOver;
}

void GA::SetGenerations(long value)
{
	m_lGenerations = value;
}

long GA::GetGenerations()
{
	return m_lGenerations;
}

void GA::SetPopulationSize(unsigned int value)
{
	m_usPopSize = value;
}

unsigned int GA::GetPopulationSize()
{
	return m_usPopSize;
}

void GA::SetApplyElitism(bool value)
{
	m_bApplyElitism = value;
}

bool GA::GetApplyElitism()
{
	return m_bApplyElitism;
}

void GA::SetGenerationNum(unsigned int value)
{
	m_usGen = value;
}

unsigned int GA::GetGenerationNum()
{
	return m_usGen;
}

void GA::SetStoppingCriteria(int value)
{
	StoppingCriteria = value;
}

int GA::GetStoppingCriteria()
{
	return StoppingCriteria;
}

double GA::rGetBestValue(unsigned int generation)
{
	return m_bestValue[generation];

}

double GA::rGetAverageValue(unsigned int generation)
{
	return m_totalValue[generation] / (double)m_usPopSize;
}