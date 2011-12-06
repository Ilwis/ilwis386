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
  LandAllocation(const FileName& fn, PointMapPtr&, const PointMap& _pmFacilities, const PointMap& _pmFacilitiesNoAttribute, const PointMap& _pmDemands, int _iOptimalFacilities, bool _fCapacitated, int _iStoppingCriteria, long _iGenerations, int _iPopulationSize, double _rMutationPercent, double _rCrossoverPercent);
  ~LandAllocation();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  static LandAllocation* create(const FileName&, PointMapPtr&, const String& sExpression);
  void Init();
  void Fitness(GAChromosome & chromosome);
  void ChromosomeMutator(GAChromosome & chromosome);
  void Initializer(GAChromosome & chromosome);
  void CrossOver(GAChromosome & Dad, GAChromosome & Mum, GAChromosome & child1, GAChromosome & child2);
  void GreedyCrossOver(GAChromosome & Dad, GAChromosome & Mum, GAChromosome & child);

private:
  long AddConnections(SegmentMap & segMap, Domain & dm, vector<int> & source, vector<int> & destination, vector<double> & allocations, GAChromosome & chromosome);
  static FileName fnGetSourceFile(const PointMap & pm, const FileName & fnObj);
  PointMap pmFacilities;
  PointMap pmFacilitiesNoAttribute; // The original pointmap, before applying MapAttribute
  PointMap pmDemands;
  // Buffers
  Coord * cFacilities;
  Coord * cDemands;
  double * rDemand;
  double * rCapacity;
  double * rDistanceOD;
  // Algorithm parameters
  int iOptimalFacilities;
  bool fCapacitated;
  // Genetic Algorithm properties
  int iStoppingCriteria;
  long iGenerations;
  int iPopulationSize;
  double rMutationPercent;
  double rCrossoverPercent;
};


#endif // LANDALLOCATION_H