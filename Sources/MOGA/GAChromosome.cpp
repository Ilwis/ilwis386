#include "GAChromosome.h"
#include "LandAllocation.h"
//#include "Engine\Base\DataObjects\strng.h"
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\Random.h"

GAChromosome::GAChromosome()
: m_dFitness(0)
, m_score1(0)
, m_score2(0)
, m_w1(random())
, m_w2(1.0 - m_w1)
{
}

GAChromosome::GAChromosome(LandAllocation * context, FitnessFunc fitnessFunc, ScoreFunc scoreFunc1, ScoreFunc scoreFunc2, bool fInitialize)
: m_dFitness(0)
, m_score1(0)
, m_score2(0)
, m_w1(random())
, m_w2(1.0 - m_w1)
{
	if (fInitialize)
		context->Initializer(*this); //will set the genes of each chromosome
	(context->*fitnessFunc)(*this, context, scoreFunc1, scoreFunc2);
}

GAChromosome::~GAChromosome()
{
}

void GAChromosome::operator = (const GAChromosome& chromosome)
{
	* static_cast<std::vector<unsigned int> *>(this) = chromosome;
	m_dFitness = chromosome.m_dFitness;
	m_score1 = chromosome.m_score1;
	m_score2 = chromosome.m_score2;
	m_w1 = chromosome.m_w1;
	m_w2 = chromosome.m_w2;
}

const double GAChromosome::rGetFitness() const
{
	return m_dFitness;
}

void GAChromosome::SetFitness(double value)
{
	m_dFitness = value;
}

void GAChromosome::SetPartialFitness(double value1, double value2)
{
	m_score1 = value1;
	m_score2 = value2;
}

const double GAChromosome::rGetPartialScore1() const
{
	return m_score1;
}

const double GAChromosome::rGetPartialScore2() const
{
	return m_score2;
}


bool GAChromosome::operator < (const GAChromosome& chromosome) const
{
	return (m_dFitness > chromosome.m_dFitness); // for sorting in descending order of fitness
}
