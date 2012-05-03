#ifndef LANDALLOCATION_H
#define LANDALLOCATION_H

#include "GAChromosome.h"
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Applications\SEGVIRT.H"

IlwisObjectPtr * createLandAllocation(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT LandAllocation: public PointMapVirtual
{
  friend class PointMapVirtual;
public:
  LandAllocation(const FileName&, PointMapPtr&);
  LandAllocation(const FileName& fn, PointMapPtr&, const PointMap& _pmFacilities, const PointMap& _pmFacilitiesNoAttribute, const String& _sColFacilitiesType, const PointMap& _pmDemands, const String& _sColDemandsPreference, int _iOptimalFacilities, bool _fCapacitated, int _iStoppingCriteria, long _iGenerations, int _iPopulationSize, double _rMutationPercent, double _rCrossoverPercent);
  ~LandAllocation();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  static LandAllocation* create(const FileName&, PointMapPtr&, const String& sExpression);
  void Init();
  void Fitness(GAChromosome & chromosome, LandAllocation * context, ScoreFunc scoreFunc);
  double rStdDistanceFunc(int demandIndex, int facilityIndex);
  double rStdDistancePreferenceFunc(int demandIndex, int facilityIndex);
  void ChromosomeMutator(GAChromosome & chromosome);
  void Initializer(GAChromosome & chromosome);
  void CrossOver(GAChromosome & Dad, GAChromosome & Mum, GAChromosome & child1, GAChromosome & child2);
  void GreedyCrossOver(GAChromosome & Dad, GAChromosome & Mum, GAChromosome & child);

private:
  long AddConnections(SegmentMap & segMap, Domain & dm, vector<int> & source, vector<int> & destination, vector<double> & allocations, GAChromosome & chromosome, LandAllocation * context, ScoreFunc scoreFunc);
  static FileName fnGetSourceFile(const PointMap & pm, const FileName & fnObj);
  PointMap pmFacilities;
  PointMap pmFacilitiesNoAttribute; // The original pointmap, before applying MapAttribute
  PointMap pmDemands;
  String sColFacilitiesType;
  String sColDemandsPreference;
  // Buffers
  Coord * cFacilities;
  Coord * cDemands;
  double * rDemand;
  double * rCapacity;
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
  double rMutationPercent;
  double rCrossoverPercent;
  bool fMultiObjective;
};


#endif // LANDALLOCATION_H