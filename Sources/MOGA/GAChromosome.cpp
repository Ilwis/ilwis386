#include "GAChromosome.h"
#include "LandAllocation.h"
//#include "Engine\Base\DataObjects\strng.h"
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\Random.h"

GAChromosome::GAChromosome()
: m_dFitness(0)
, m_w1(random())
, m_w2(1.0 - m_w1)
{
}

GAChromosome::GAChromosome(LandAllocation * context, FitnessFunc fitnessFunc, ScoreFunc scoreFunc, bool fInitialize)
: m_dFitness(0)
, m_w1(random())
, m_w2(1.0 - m_w1)
{
	if (fInitialize)
		context->Initializer(*this); //will set the genes of each chromosome
	(context->*fitnessFunc)(*this, context, scoreFunc);
}

GAChromosome::~GAChromosome()
{
}

void GAChromosome::operator = (const GAChromosome& chromosome)
{
	clear();
	int iLength = chromosome.size();
	for(int i = 0; i < iLength; ++i)
		push_back(chromosome[i]);
	m_dFitness = chromosome.m_dFitness;
}

void GAChromosome::CopyChromosome(GAChromosome & chromosome)
{
	clear();
	int iLength = chromosome.size();
	for(int i = 0; i < iLength; ++i)
		push_back(chromosome[i]);
	m_dFitness = chromosome.m_dFitness;
}
    
int GAChromosome::HasThisGene(unsigned int Gene)
{
	bool fFound = false;
	int length = size();
	int index = 0;
	while(index < length)
	{
		if(Gene == at(index))
		{
			fFound = true;
			break;
		}
		++index;
	}
	if(!fFound)
		return -1;
	return index;
}

double GAChromosome::rGetFitness()
{
	return m_dFitness;
}

void GAChromosome::SetFitness(double value)
{
	m_dFitness = value;
}

bool GAChromosome::operator < (const GAChromosome& chromosome) const
{
	return (m_dFitness > chromosome.m_dFitness); // for sorting in descending order of fitness
}
