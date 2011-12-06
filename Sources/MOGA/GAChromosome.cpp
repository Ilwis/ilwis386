#include "GAChromosome.h"
#include "LandAllocation.h"
//#include "Engine\Base\DataObjects\strng.h"
#include "Headers\toolspch.h"

GAChromosome::GAChromosome()
: m_dFitness(0)
{
}

GAChromosome::GAChromosome(LandAllocation * context, bool fInitialize)
: m_dFitness(0)
{
	if (fInitialize)
		context->Initializer(*this); //will set the genes of each chromosome
	context->Fitness(*this);
}

GAChromosome::~GAChromosome()
{
}

void GAChromosome::CopyChromosome(GAChromosome newChromosome)
{
	clear();
	int iLength = newChromosome.size();
	for(int i = 0; i < iLength; ++i)
		push_back(newChromosome[i]);
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
