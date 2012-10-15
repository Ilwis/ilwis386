#ifndef TABLENETWORKDISTANCE_H
#define TABLENETWORKDISTANCE_H

#include "Engine\Applications\Tbl2DimVirt.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Segment\Seg.h"

IlwisObjectPtr * createTableNetworkDistance(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT TableNetworkDistance: public Table2DimVirtual
{
  friend class Table2DimVirtual;
public:
  TableNetworkDistance(const FileName&, TablePtr&);
  TableNetworkDistance(const FileName& fn, TablePtr&, const PointMap& _pmOrigins, const PointMap& _pmDestinations, const SegmentMap& _smNetwork, const bool _fProject, const bool _fOutputMap);
  ~TableNetworkDistance();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  static TableNetworkDistance* create(const FileName&, TablePtr&, const String& sExpression);
  void Init();

private:
  PointMap pmOrigins;
  PointMap pmDestinations;
  SegmentMap smNetwork;
  bool fProject;
  bool fOutputMap;
};


#endif // TABLENETWORKDISTANCE_H