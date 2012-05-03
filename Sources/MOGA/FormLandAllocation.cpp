#include "Client\Headers\AppFormsPCH.h"
#include "FormLandAllocation.h"
#include "Client\FormElements\fldcol.h"

LRESULT CmdLandAllocation(CWnd *wnd, const String& s)
{
	new FormLandAllocation(wnd, s.c_str());
	return -1;
}

FormLandAllocation::FormLandAllocation(CWnd* mw, const char* sPar)
: FormPointMapCreate(mw, TR("Land Allocation of Point Map"))
{
	iMethod = 0;
	fCapacitated = false;
	iStoppingCriteria = 0;
	iGenerations = 60;
	iPopulationSize = 10;
	rMutationPercent = 20;
	rCrossoverPercent = 98;

	RadioGroup * rgMethod = new RadioGroup(root, TR("Method:"), &iMethod);
	RadioButton* rbSingle = new RadioButton(rgMethod, TR("&Single Objective"));
	RadioButton* rbMulti = new RadioButton(rgMethod, TR("&Multi Objective"));

	plpm = new FieldPointMap(root, TR("&Potential Locations Point Map"), &sPointMapFacilities, new MapListerDomainType(".mpp", 0, true));
	plpm->SetCallBack((NotifyProc)&FormLandAllocation::FacilitiesCallBack);
	fsTotalFacilities = new FieldString(root, TR("Total Capacity"), &sTotalFacilities);
	fcFacilitiesType = new FieldColumn(rbMulti, TR("Facility Type"), Table(), &sColFacilitiesType);
	fcFacilitiesType->Align(fsTotalFacilities, AL_UNDER);
	dpm = new FieldPointMap(root, TR("&Demands Point Map"), &sPointMapDemands, new MapListerDomainType(".mpp", 0, true));
	dpm->SetCallBack((NotifyProc)&FormLandAllocation::DemandsCallBack);
	fsTotalDemands = new FieldString(root, TR("Total Demands"), &sTotalDemands);
	fcDemandsPreference = new FieldColumn(rbMulti, TR("Demands Preference"), Table(), &sColDemandsPreference);
	fcDemandsPreference->Align(fsTotalDemands, AL_UNDER);
	fiOptimalFacilities = new FieldInt(root, TR("Nr &Optimal Facilities"), &iOptimalFacilities, ValueRange(1, 32767), true);
	new CheckBox(root, TR("&Capacitated"), &fCapacitated);
	new FieldInt(root, TR("&Stack Threshold"), &iStoppingCriteria, ValueRange(0, 32767), true);
	new FieldInt(root, TR("&Generations"), &iGenerations, ValueRange(0, 2147483647), true);
	new FieldInt(root, TR("P&opulation Size"), &iPopulationSize, ValueRange(0, 32767), true);
	new FieldReal(root, TR("&Mutation Percent"), &rMutationPercent, ValueRange(0, 100, 0.1));
	new FieldReal(root, TR("C&rossover Percent"), &rCrossoverPercent, ValueRange(0, 100, 0.1));

	initPointMapOut(false);
	SetHelpItem("ilwisapp\\mask_points_dialog_box.htm");
	create();

	fsTotalFacilities->SetReadOnly(true);
	fsTotalDemands->SetReadOnly(true);
}

int FormLandAllocation::FacilitiesCallBack(Event*)
{
	plpm->StoreData();
	if (sPointMapFacilities.length() > 0)
	{
		PointMap pmFacilities (sPointMapFacilities);
		int iNrFacilities = pmFacilities->iFeatures();
		if (fiOptimalFacilities->iVal() < 1 || fiOptimalFacilities->iVal() > iNrFacilities)
			fiOptimalFacilities->SetVal(iNrFacilities);
		fiOptimalFacilities->SetvalueRange(ValueRange(1, iNrFacilities));
		double rNrFacilities;
		if (pmFacilities->dvrs().fValues())
		{
			rNrFacilities = 0;
			for (int i = 0; i < iNrFacilities; ++i)
				rNrFacilities += pmFacilities->rValue(i);
		} else
			rNrFacilities = iNrFacilities;
		sTotalFacilities = String("%d", (int)(rNrFacilities));
		fsTotalFacilities->SetVal(sTotalFacilities);
		FileName fnPointMapFacilities(sPointMapFacilities);
		fnPointMapFacilities.sCol = "";
		pmFacilities = PointMap(fnPointMapFacilities);
		if (!pmFacilities->fTblAtt()) {
			fcFacilitiesType->FillWithColumns((TablePtr*)0);
		} else {
			Table tbl = pmFacilities->tblAtt();
			fcFacilitiesType->FillWithColumns(&tbl);
		}
	}
	return 0; 
}

int FormLandAllocation::DemandsCallBack(Event*)
{
	dpm->StoreData();
	if (sPointMapDemands.length() > 0)
	{
		PointMap pmDemands (sPointMapDemands);
		int iNrDemands = pmDemands->iFeatures();
		double rNrDemands;
		if (pmDemands->dvrs().fValues())
		{
			rNrDemands = 0;
			for (int i = 0; i < iNrDemands; ++i)
				rNrDemands += pmDemands->rValue(i);
		} else
			rNrDemands = iNrDemands;
		sTotalDemands = String("%d", (int)(rNrDemands));
		fsTotalDemands->SetVal(sTotalDemands);
		FileName fnPointMapDemands(sPointMapDemands);
		fnPointMapDemands.sCol = "";
		pmDemands = PointMap(fnPointMapDemands);
		if (!pmDemands->fTblAtt()) {
			fcDemandsPreference->FillWithColumns((TablePtr*)0);
		} else {
			Table tbl = pmDemands->tblAtt();
			fcDemandsPreference->FillWithColumns(&tbl);
		}
	}
	return 0; 
}

int FormLandAllocation::exec() 
{
	FormPointMapCreate::exec();
	String sExpr;
	FileName fn(sOutMap);
	FileName fnPointMapFacilities(sPointMapFacilities);
	sPointMapFacilities = fnPointMapFacilities.sRelativeQuoted(false,fn.sPath());
	FileName fnPointMapDemands(sPointMapDemands);
	sPointMapDemands = fnPointMapDemands.sRelativeQuoted(false,fn.sPath());
	if (iMethod == 0)
		sExpr = String("PointMapLandAllocation(%S,%S,%d,%s,%d,%d,%d,%f,%f)", sPointMapFacilities, sPointMapDemands, iOptimalFacilities, fCapacitated ? "capacitated" : "plain", iStoppingCriteria, iGenerations, iPopulationSize, rMutationPercent / 100.0, rCrossoverPercent / 100.0);
	else // iMethod == 1
		sExpr = String("PointMapLandAllocation(%S,%S,%S,%S,%d,%s,%d,%d,%d,%f,%f)", sPointMapFacilities, sColFacilitiesType, sPointMapDemands, sColDemandsPreference, iOptimalFacilities, fCapacitated ? "capacitated" : "plain", iStoppingCriteria, iGenerations, iPopulationSize, rMutationPercent / 100.0, rCrossoverPercent / 100.0);
	execPointMapOut(sExpr);  
	return 0;
}
