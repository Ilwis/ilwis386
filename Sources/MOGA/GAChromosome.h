#ifndef GACHROMOSOME_H
#define GACHROMOSOME_H

#include "Engine\Applications\objvirt.h"
#include <vector>

class GAChromosome;
class LandAllocation;

typedef double (IlwisObjectVirtual::*ScoreFunc)(int demandIndex, int facilityIndex);
typedef void (IlwisObjectVirtual::*FitnessFunc)(GAChromosome & chromosome, LandAllocation * context, ScoreFunc scoreFunc);

class GAChromosome : public std::vector<unsigned int>
{
public:
	GAChromosome();
	GAChromosome(LandAllocation * context, FitnessFunc fitnessFunc, ScoreFunc scoreFunc, bool fInitialize);
	virtual ~GAChromosome();
	void CopyChromosome(GAChromosome newChromosome); 
	int HasThisGene(unsigned int Gene);
	double rGetFitness();
	void SetFitness(double value);
	bool operator < (const GAChromosome& chromosome) const;

private:
	double m_dFitness;
};

#endif // GACHROMOSOME_H