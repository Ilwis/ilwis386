#ifndef FORMLANDALLOCATION_H
#define FORMLANDALLOCATION_H
#include "Client\Forms\frmpntcr.h"
#include "Client\ilwis.h"
#include "Engine\Table\tbl2dim.h"

LRESULT CmdLandAllocation(CWnd *wnd, const String& s);

class FieldColumn;
class ParetoGraphFunction;
class FieldGraph;

class Table2DimLister: public ObjectExtensionLister
{
public:
  Table2DimLister(const Domain& dom1, const Domain& dom2)
  : ObjectExtensionLister(0, ".ta2"),
  dm1(dom1),
  dm2(dom2)
  {}
  virtual bool fOK(const FileName& fn, const String&)
  {
	if (!dm1.fValid() || !dm2.fValid())
		return false;
	String sDom;
	if (!ObjectInfo::ReadElement("Table2Dim", "Domain1", fn, sDom))
		return false;
	if (sDom.length() == 0)  
		return false;

	FileName fnDom1(FileName(sDom, ".dom", false).sShortNameQuoted(true), fn);
	if (!File::fExist(fnDom1))
	{
		fnDom1.Dir(fn.sPath());     // Was the Domain relative to fn?
		if (!File::fExist(fnDom1))  //  - No!, try the system dir
			fnDom1.Dir(IlwWinApp()->Context()->sStdDir());
	}
	if (!ObjectInfo::ReadElement("Table2Dim", "Domain2", fn, sDom))
		return false;
	if (sDom.length() == 0)  
		return false;

	FileName fnDom2(FileName(sDom, ".dom", false).sShortNameQuoted(true), fn);
	if (!File::fExist(fnDom2))
	{
		fnDom2.Dir(fn.sPath());     // Was the Domain relative to fn?
		if (!File::fExist(fnDom2))  //  - No!, try the system dir
			fnDom2.Dir(IlwWinApp()->Context()->sStdDir());
	}
	return ((fnDom1 == dm1->fnObj && fnDom2 == dm2->fnObj) || (fnDom1 == dm2->fnObj && fnDom2 == dm1->fnObj));
  }
private:
  Domain dm1;
  Domain dm2;
};

class Field2DimTable: public FieldDataType
{
public:
  Field2DimTable(FormEntry* parent, const String& sQuestion, String *sName)
    : FieldDataType(parent, sQuestion, sName, ".ta2", true)
  {
	  SetObjLister(new Table2DimLister(Domain("none.dom"), Domain("none.dom")));
	  setHelpItem(htpUiTbl);
  }
  void SetDomain1(const Domain& _dm1)
  {
	  dm1 = _dm1;
	  SetObjLister(new Table2DimLister(dm1, dm2));
  }
  void SetDomain2(const Domain& _dm2)
  {
	  dm2 = _dm2;
	  SetObjLister(new Table2DimLister(dm1, dm2));
  }
private:
  Domain dm1;
  Domain dm2;
};

class _export FormLandAllocation: public FormPointMapCreate
{
public:
  FormLandAllocation(CWnd* mw, const char* sPar);
  virtual ~FormLandAllocation();
private:
  int MethodCallBack(Event*);
  int FacilitiesCallBack(Event*);
  int DemandsCallBack(Event*);
  int AdjustEliteCallBack(Event*);
  int GenerateParetoGraph(Event*);
  int StoreSelectedChromosome(Event*);
  int StoreParetoAsTable(Event*);
  int CallBackAnchorChangedInGraph(Event*);
  static UINT GenerateParetoGraphInThread(LPVOID pParam);
  RadioGroup * rgMethod;
  FieldPointMap * fpmFacilities;
  FieldString * fsTotalFacilities;
  String sTotalFacilities;
  FieldPointMap * fpmDemands;
  Field2DimTable *ftbODMatrix;
  FieldString * fsTotalDemands;
  String sTotalDemands;
  FieldInt * fiOptimalFacilities;
  FieldInt * fiPopulation;
  FieldInt * fiElite;
  FieldInt * fiPareto;
  FieldColumn * fcFacilitiesType;
  FieldColumn * fcDemandsPreference;
  FieldGraph * fgFunctionGraph;
  PushButton* pbCalculatePareto;
  PushButton* pbStoreSelectedChromosome;
  PushButton* pbStoreParetoAsTable;
  String sSelectedChromosome;
  FieldString* fsSelectedChromosome;
  int exec();
  String sPointMapFacilities;
  String sPointMapDemands;
  int iOptimalFacilities;
  bool fCapacitated;
  int iStoppingCriteria;
  long iGenerations;
  int iPopulationSize;
  int iNelite;
  int iNpareto;
  double rMutationPercent;
  double rCrossoverPercent;
  int iMethod;
  int iDistance;
  String sColFacilitiesType;
  String sColDemandsPreference;
  String sODMatrix;
  Table2Dim tableODmatrix;
  ParetoGraphFunction * m_function;

  LandAllocation * m_la;
  std::vector<GAChromosome> m_pareto;
};

#endif // FORMLANDALLOCATION_H