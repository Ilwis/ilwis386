#ifndef FORMLANDALLOCATION_H
#define FORMLANDALLOCATION_H
#include "Client\Forms\frmpntcr.h"

LRESULT CmdLandAllocation(CWnd *wnd, const String& s);

class FieldColumn;

class _export FormLandAllocation: public FormPointMapCreate
{
public:
  FormLandAllocation(CWnd* mw, const char* sPar);
private:
  int FacilitiesCallBack(Event*);
  int DemandsCallBack(Event*);
  FieldPointMap * plpm;
  FieldString * fsTotalFacilities;
  String sTotalFacilities;
  FieldPointMap * dpm;
  FieldString * fsTotalDemands;
  String sTotalDemands;
  FieldInt * fiOptimalFacilities;
  FieldColumn * fcFacilitiesType;
  FieldColumn * fcDemandsPreference;
  int exec();
  String sPointMapFacilities;
  String sPointMapDemands;
  int iOptimalFacilities;
  bool fCapacitated;
  int iStoppingCriteria;
  long iGenerations;
  int iPopulationSize;
  double rMutationPercent;
  double rCrossoverPercent;
  int iMethod;
  String sColFacilitiesType;
  String sColDemandsPreference;
};

#endif // FORMLANDALLOCATION_H