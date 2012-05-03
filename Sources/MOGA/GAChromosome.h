#ifndef GACHROMOSOME_H
#define GACHROMOSOME_H

#include "Engine\Applications\objvirt.h"
#include <vector>

class GAChromosome;
class LandAllocation;

typedef double (IlwisObjectVirtual::*ScoreFunc)(int demandIndex, int facilityIndex, double w1, double w2);
typedef void (IlwisObjectVirtual::*FitnessFunc)(GAChromosome & chromosome, LandAllocation * context, ScoreFunc scoreFunc);

class GAChromosome : public std::vector<unsigned int>
{
public:
	GAChromosome();
	GAChromosome(LandAllocation * context, FitnessFunc fitnessFunc, ScoreFunc scoreFunc, bool fInitialize);
	virtual ~GAChromosome();
	void CopyChromosome(GAChromosome & chromosome); 
	int HasThisGene(unsigned int Gene);
	double rGetFitness();
	const double w1() {return m_w1;};
	const double w2() {return m_w2;};
	void SetFitness(double value);
	bool operator < (const GAChromosome& chromosome) const;
	void operator = (const GAChromosome& chromosome);

private:
	const double m_w1;
	const double m_w2;
	double m_dFitness;
};

#endif // GACHROMOSOME_H