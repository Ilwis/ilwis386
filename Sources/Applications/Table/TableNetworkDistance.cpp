#include "TableNetworkDistance.h"
#include "Engine\Base\Algorithm\NetworkDistance.h"
#include "Engine\Table\tbl2dim.h"

IlwisObjectPtr * createTableNetworkDistance(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TableNetworkDistance::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableNetworkDistance(fn, (TablePtr &)ptr);
}

const char* TableNetworkDistance::sSyntax() {
  return "Table2DimNetworkDistance(OriginPoints,DestinationPoints,NetworkMap,project|noproject)";
}

TableNetworkDistance* TableNetworkDistance::create(const FileName& fn, TablePtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 4)
	throw ErrorExpression(sExpr, sSyntax());

  if ((as[3].compare("project") != 0) && (as[3].compare("noproject") != 0))
	throw ErrorExpression(sExpr, sSyntax());
  PointMap pmOrigins(as[0], fn.sPath());
  PointMap pmDestinations(as[1], fn.sPath());
  SegmentMap smNetwork(as[2], fn.sPath());
  bool fProject(as[3].compare("project") == 0);
  return new TableNetworkDistance(fn, p, pmOrigins, pmDestinations, smNetwork, fProject);
}

TableNetworkDistance::TableNetworkDistance(const FileName& fn, TablePtr& p)
: Table2DimVirtual(fn, p)
{
  fNeedFreeze = true;
  try {
    ReadElement("Table2DimNetworkDistance", "PointMapOrigins", pmOrigins);
    ReadElement("Table2DimNetworkDistance", "PointMapDestinations", pmDestinations);
	ReadElement("Table2DimNetworkDistance", "SegmentMapNetwork", smNetwork);
	ReadElement("Table2DimNetworkDistance", "Project", fProject);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(pmOrigins.ptr());
  objdep.Add(pmDestinations.ptr());
  objdep.Add(smNetwork.ptr());
}

TableNetworkDistance::TableNetworkDistance(const FileName& fn, TablePtr& p, const PointMap& _pmOrigins, const PointMap& _pmDestinations, const SegmentMap& _smNetwork, const bool _fProject)
: Table2DimVirtual(fn, p, true)
, pmOrigins(_pmOrigins)
, pmDestinations(_pmDestinations)
, smNetwork(_smNetwork)
, fProject(_fProject)
{
  fNeedFreeze = true;
  Init();
  ptr.SetDomain("none");
  Domain dm1(pmOrigins->dm());
  Domain dm2(pmDestinations->dm());
  ((Table2DimPtr*)(&ptr))->SetAxisDomains(dm1, dm2);
  objdep.Add(pmOrigins.ptr());
  objdep.Add(pmDestinations.ptr());
  objdep.Add(smNetwork.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
}

void TableNetworkDistance::Store()
{
  Table2DimVirtual::Store();
  WriteElement("TableVirtual", "Type", "Table2DimNetworkDistance");
  WriteElement("Table2DimNetworkDistance", "PointMapOrigins", pmOrigins);
  WriteElement("Table2DimNetworkDistance", "PointMapDestinations", pmDestinations);
  WriteElement("Table2DimNetworkDistance", "SegmentMapNetwork", smNetwork);
  WriteElement("Table2DimNetworkDistance", "Project", fProject);
}

TableNetworkDistance::~TableNetworkDistance()
{
}

String TableNetworkDistance::sExpression() const
{
	return String("Table2DimNetworkDistance(%S,%S,%S,%s)", pmOrigins->sNameQuoted(false, fnObj.sPath()), pmDestinations->sNameQuoted(false, fnObj.sPath()), smNetwork->sNameQuoted(false, fnObj.sPath()), fProject?"project":"noproject");
}

bool TableNetworkDistance::fDomainChangeable() const
{
  return false;
}

void TableNetworkDistance::Init()
{
  htpFreeze = "ilwisapp\\mask_points_algorithm.htm";
  sFreezeTitle = "Table2DimNetworkDistance";
}

bool TableNetworkDistance::fFreezing()
{
	trq.SetTitle(sFreezeTitle);
	trq.SetText(TR("Computing network distance"));
	Domain dm1(pmOrigins->dm());
	Domain dm2(pmDestinations->dm());
	((Table2DimPtr*)(&ptr))->SetAxisDomains(dm1, dm2); // re-do, because this was reset in UnFreeze()
	Domain dm("distance.dom");
	((Table2DimPtr*)(&ptr))->SetDataDomain(dm);
	NetworkDistance nd (pmOrigins, pmDestinations, smNetwork, fProject, trq);
	nd.CopyToTable2Dim(*(Table2DimPtr*)(&ptr));
	ptr.sDescription = String("Distance from %S to %S via the segments in %S", pmOrigins->sName(), pmDestinations->sName(), smNetwork->sName());
	ptr.Store();
	return true;
}


