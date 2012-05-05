#ifndef FORMLANDALLOCATION_H
#define FORMLANDALLOCATION_H
#include "Client\Forms\frmpntcr.h"

LRESULT CmdLandAllocation(CWnd *wnd, const String& s);

class FieldColumn;
class ParetoGraphFunction;
class FieldGraph;

class _export FormLandAllocation: public FormPointMapCreate
{
public:
  FormLandAllocation(CWnd* mw, const char* sPar);
  virtual ~FormLandAllocation();
private:
  int FacilitiesCallBack(Event*);
  int DemandsCallBack(Event*);
  int AdjustEliteCallBack(Event*);
  int GenerateParetoGraph(Event*);
  static UINT GenerateParetoGraphInThread(LPVOID pParam);
  FieldPointMap * fpmFacilities;
  FieldString * fsTotalFacilities;
  String sTotalFacilities;
  FieldPointMap * fpmDemands;
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
  String sColFacilitiesType;
  String sColDemandsPreference;
  ParetoGraphFunction * m_function;
};

#endif // FORMLANDALLOCATION_H