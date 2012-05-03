#ifndef GACHROMOSOME_H
#define GACHROMOSOME_H

#include "Engine\Applications\objvirt.h"
#include <vector>

class GAChromosome;
class LandAllocation;

typedef void (IlwisObjectVirtual::*FitnessFunc)(GAChromosome & chromosome);

class GAChromosome : public std::vector<unsigned int>
{
public:
	GAChromosome();
	GAChromosome(LandAllocation * context, FitnessFunc fitnessFunc, bool fInitialize);
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