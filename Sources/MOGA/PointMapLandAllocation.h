#ifndef POINTMAPLANDALLOCATION_H
#define POINTMAPLANDALLOCATION_H

#include "GAChromosome.h"
#include "Engine\Applications\PNTVIRT.H"

IlwisObjectPtr * createPointMapLandAllocation(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT PointMapLandAllocation: public PointMapVirtual
{
  friend class PointMapVirtual;
public:
  PointMapLandAllocation(const FileName&, PointMapPtr&);
  PointMapLandAllocation(const FileName& fn, PointMapPtr&, const PointMap& _pmFacilities, const PointMap& _pmFacilitiesNoAttribute, const String& _sColFacilitiesType, const PointMap& _pmDemands, const PointMap& _pmDemandsNoAttribute, const String& _sColDemandsPreference, int _iOptimalFacilities, bool _fCapacitated, int _iStoppingCriteria, long _iGenerations, int _iPopulationSize, int _iNelite, int _iNpareto, double _rMutationPercent, double _rCrossoverPercent);
  ~PointMapLandAllocation();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  static PointMapLandAllocation* create(const FileName&, PointMapPtr&, const String& sExpression);
  void Init();
  static FileName fnGetSourceFile(const PointMap & pm, const FileName & fnObj);

private:
  PointMap pmFacilities;
  PointMap pmFacilitiesNoAttribute; // The original pointmap, before applying MapAttribute
  PointMap pmDemands;
  PointMap pmDemandsNoAttribute; // The original pointmap, before applying MapAttribute
  String sColFacilitiesType;
  String sColDemandsPreference;
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
};


#endif // POINTMAPLANDALLOCATION_H