#ifndef LANDALLOCATION_H
#define LANDALLOCATION_H

#include "GAChromosome.h"
#include "FunctionProvider.h"
#include "Engine\Applications\PNTVIRT.H"

class GA;

class LandAllocation : public FunctionProvider
{
public:
  LandAllocation(const PointMap& _pmFacilities, const PointMap& _pmFacilitiesNoAttribute, const String& _sColFacilitiesType, const PointMap& _pmDemands, const PointMap& _pmDemandsNoAttribute, const String& _sColDemandsPreference, int _iOptimalFacilities, bool _fCapacitated, int _iStoppingCriteria, long _iGenerations, int _iPopulationSize, int _iNelite, int _iNpareto, double _rMutationPercent, double _rCrossoverPercent);
  ~LandAllocation();
  GAChromosome * PerformLandAllocation(Tranquilizer & trq);
  std::vector<GAChromosome> GenerateParetoArray(Tranquilizer & trq);
  void ChromosomeMutator(GAChromosome & chromosome);
  void Initializer(GAChromosome & chromosome);
  void CrossOver(GAChromosome & Dad, GAChromosome & Mum, GAChromosome & child1, GAChromosome & child2);
  void StoreChromosome (GAChromosome * chromosome, PointMapPtr * pntMapPtr);
  void StoreParetoInTable(std::vector<GAChromosome> & pareto, FileName & fnParetoTable);

private:
  void Init();
  void FitnessSO(GAChromosome & chromosome, LandAllocation * context, ScoreFunc scoreFunc1, ScoreFunc scoreFunc2);
  void FitnessMO(GAChromosome & chromosome, LandAllocation * context, ScoreFunc scoreFunc1, ScoreFunc scoreFunc2);
  double rStdDistanceFunc(int demandIndex, int facilityIndex);
  double rStdPreferenceFunc(int demandIndex, int facilityIndex);
  void GreedyCrossOver(GAChromosome & Dad, GAChromosome & Mum, GAChromosome & child);
  void UpdatePareto(std::vector<GAChromosome> & solutions, std::vector<GAChromosome> & pareto);
  void KeepElite(std::vector<GAChromosome> & solutions, std::vector<GAChromosome> & pareto);
  long AddConnections(PointMap & pmFacilities, PointMap & pmDemands, int iOptimalFacilities, bool fMultiObjective, bool fCapacitated, SegmentMap & segMap, Domain & dm, vector<int> & source, vector<int> & destination, vector<double> & allocations, GAChromosome & chromosome, ScoreFunc scoreFunc1, ScoreFunc scoreFunc2);
  PointMap pmFacilities;
  PointMap pmFacilitiesNoAttribute; // The original pointmap, before applying MapAttribute
  PointMap pmDemands;
  PointMap pmDemandsNoAttribute; // The original pointmap, before applying MapAttribute
  String sColFacilitiesType;
  String sColDemandsPreference;
  // Buffers
  vector<vector<double>> rDistanceOD;
  vector<vector<double>> rPreferenceMatrix;
  double rMinDistance;
  double rMaxDistance;
  // Algorithm parameters
  int iOptimalFacilities;
  bool fCapacitated;
  // Genetic Algorithm properties
  int iStoppingCriteria;
  long iGenerations;
  int iPopulationSize;
  int iNelite;
  int iNpareto;
  double rMutationPercent;
  double rCrossoverPercent;
  bool fMultiObjective;
  GA * GAAlgorithm;
  RealBuf vrFitnessList;
  RealBuf vrPopulationAvgList;
  double * rDemand;
  double * rCapacity;
  Coord * cFacilities;
  Coord * cDemands;

};


#endif // LANDALLOCATION_H