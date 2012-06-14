#ifndef GA_H
#define GA_H

#include "GAChromosome.h"

#include <vector>
#include <stack>

class LandAllocation;

class GA
{
public:
	enum Selection { Mix, Random, Tournament, Probability };

	GA(LandAllocation * _context, FitnessFunc _fitnessFunc, ScoreFunc _scoreFunc1, ScoreFunc scoreFunc2);
	virtual ~GA();
	void Initialize();
	bool IsDone();
	void CreateNextGeneration();
	long GetGenerations();
	GAChromosome * GetBestChromosome();
	void SetSelectionType(Selection value);
	void SetGenerations(long value);
	void SetPopulationSize(unsigned int value);
	void SetMutation(double value);
	void SetCrossover(double value);
	void SetStoppingCriteria(int value);
	double rGetBestValue(unsigned int generation);
	double rGetAverageValue(unsigned int generation);
	void SetApplyElitism(bool value);
	std::vector<GAChromosome> & GetCurrentGeneration();

private:
	void push(double value);
	void deleteStack();
	void RankPopulation();
	int RandomSelection();
	int RouletteSelection();
	int TournamentSelection();
	int ProbabilitySelection();
	Selection GetSelectionType();
	unsigned int GetPopulationSize();
	bool GetApplyElitism();
	void SetGenerationNum(unsigned int value);
	unsigned int GetGenerationNum();
	int GetStoppingCriteria();

	double m_dMutation;
	long m_lGenerations;
	double m_dCrossOver;
	unsigned int m_usPopSize;
	double m_dTotalFitness;
	bool m_bApplyElitism;
	unsigned int m_usGen;	 
	Selection m_SelType;
	int StoppingCriteria;
	double TopFitness;
	double temp_best;
	GAChromosome * m_chro_TopFitness;
	GAChromosome * tempBest_m_chro;

	std::vector<GAChromosome> m_thisGeneration;
	std::stack<double> m_stack;
	double * m_bestValue;
	double * m_totalValue;

	LandAllocation * context;
	FitnessFunc fitnessFunc;
	ScoreFunc scoreFunc1;
	ScoreFunc scoreFunc2;
};

#endif // GA_H