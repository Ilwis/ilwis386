#ifndef GACHROMOSOME_H
#define GACHROMOSOME_H

#include "Engine\Applications\objvirt.h"
#include <vector>
#include "FunctionProvider.h"

class GAChromosome;
class LandAllocation;

typedef double (FunctionProvider::*ScoreFunc)(int demandIndex, int facilityIndex);
typedef void (FunctionProvider::*FitnessFunc)(GAChromosome & chromosome, LandAllocation * context, ScoreFunc scoreFunc1, ScoreFunc scoreFunc2);

class GAChromosome : public std::vector<unsigned int>
{
public:
	GAChromosome();
	GAChromosome(LandAllocation * context, FitnessFunc fitnessFunc, ScoreFunc scoreFunc1, ScoreFunc scoreFunc2, bool fInitialize);
	virtual ~GAChromosome();
	const double rGetFitness() const;
	const double rGetPartialScore1() const;
	const double rGetPartialScore2() const;
	const double w1() const {return m_w1;};
	const double w2() const {return m_w2;};
	void SetFitness(double value);
	void SetPartialFitness(double value1, double value2);
	bool operator < (const GAChromosome& chromosome) const;
	void operator = (const GAChromosome& chromosome);

private:
	double m_w1;
	double m_w2;
	double m_dFitness;
	double m_score1;
	double m_score2;
};

#endif // GACHROMOSOME_H