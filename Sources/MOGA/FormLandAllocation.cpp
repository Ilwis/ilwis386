#include "Client\Headers\AppFormsPCH.h"
#include "LandAllocation.h"
#include "FormLandAllocation.h"
#include "Client\FormElements\fldcol.h"
#include "ParetoFieldGraph.h"
#include "PointMapLandAllocation.h"
#include "ParetoGraphFunction.h"

LRESULT CmdLandAllocation(CWnd *wnd, const String& s)
{
	new FormLandAllocation(wnd, s.c_str());
	return -1;
}

FormLandAllocation::FormLandAllocation(CWnd* mw, const char* sPar)
: FormPointMapCreate(mw, TR("Land Allocation of Point Map"))
, m_function(0)
, m_la(0)
{
	iMethod = 0;
	fCapacitated = false;
	iStoppingCriteria = 0;
	iGenerations = 60;
	iPopulationSize = 10;
	iNelite = 8;
	iNpareto = 10;
	rMutationPercent = 20;
	rCrossoverPercent = 98;
	
	FieldGroup* fgLeft = new FieldGroup(root);
	rgMethod = new RadioGroup(fgLeft, TR("Objectives:"), &iMethod);
	rgMethod->SetCallBack((NotifyProc)&FormLandAllocation::MethodCallBack);
	RadioButton* rbSingle = new RadioButton(rgMethod, TR("&Distance"));
	RadioButton* rbMulti = new RadioButton(rgMethod, TR("&Distance + Preference"));
	fpmFacilities = new FieldPointMap(fgLeft, TR("&Potential Locations Point Map"), &sPointMapFacilities, new MapListerDomainType(".mpp", 0, true));
	fpmFacilities->SetCallBack((NotifyProc)&FormLandAllocation::FacilitiesCallBack);
	fpmFacilities->Align(rbMulti, AL_UNDER);
	fsTotalFacilities = new FieldString(fgLeft, TR("Total Capacity"), &sTotalFacilities);
	fcFacilitiesType = new FieldColumn(rbMulti, TR("Facility Type"), Table(), &sColFacilitiesType);
	fcFacilitiesType->Align(fsTotalFacilities, AL_UNDER);
	fpmDemands = new FieldPointMap(fgLeft, TR("&Demands Point Map"), &sPointMapDemands, new MapListerDomainType(".mpp", 0, true));
	fpmDemands->SetCallBack((NotifyProc)&FormLandAllocation::DemandsCallBack);
	fsTotalDemands = new FieldString(fgLeft, TR("Total Demands"), &sTotalDemands);
	fcDemandsPreference = new FieldColumn(rbMulti, TR("Demands Preference"), Table(), &sColDemandsPreference);
	fcDemandsPreference->Align(fsTotalDemands, AL_UNDER);
	fiOptimalFacilities = new FieldInt(fgLeft, TR("Nr &Optimal Facilities"), &iOptimalFacilities, ValueRange(1, 32767), true);
	new CheckBox(fgLeft, TR("&Capacitated"), &fCapacitated);
	//new FieldInt(fgLeft, TR("&Stack Threshold"), &iStoppingCriteria, ValueRange(0, 32767), true);
	new FieldInt(fgLeft, TR("&Generations"), &iGenerations, ValueRange(0, 2147483647), true);
	fiPopulation = new FieldInt(fgLeft, TR("P&opulation Size"), &iPopulationSize, ValueRange(0, 32767), true);
	fiElite = new FieldInt(rbMulti, TR("Nelite"), &iNelite, ValueRange(0, 32767), true);
	fiElite->Align(fiPopulation, AL_UNDER);
	fiPareto = new FieldInt(rbMulti, TR("Npareto"), &iNpareto, ValueRange(0, 32767), true);
	fiPareto->Align(fiElite, AL_UNDER);
	fiPopulation->SetCallBack((NotifyProc)&FormLandAllocation::AdjustEliteCallBack);
	fiPareto->SetCallBack((NotifyProc)&FormLandAllocation::AdjustEliteCallBack);
	FieldReal * frMutation = new FieldReal(fgLeft, TR("&Mutation Percent"), &rMutationPercent, ValueRange(0, 100, 0.1));
	FieldReal * frCrossover = new FieldReal(rbSingle, TR("C&rossover Percent"), &rCrossoverPercent, ValueRange(0, 100, 0.1));
	frCrossover->Align(frMutation, AL_UNDER);

	FieldGroup* fgRight = new FieldGroup(rbMulti);
	fgRight->Align(fgLeft, AL_AFTER);

	fgFunctionGraph = new ParetoFieldGraph(fgRight);
	fgFunctionGraph->SetWidth(200);
	fgFunctionGraph->SetHeight(300);
	fgFunctionGraph->SetIndependentPos();
	fgFunctionGraph->SetFunction(0);
	fgFunctionGraph->SetCallBack((NotifyProc)&FormLandAllocation::CallBackAnchorChangedInGraph);
	pbCalculatePareto = new PushButton(fgRight, TR("Calculate Pareto"), (NotifyProc)&FormLandAllocation::GenerateParetoGraph);
	pbCalculatePareto->Align(fgFunctionGraph, AL_UNDER);
	fsSelectedChromosome = new FieldString(fgRight, TR("Selected Chromosome"), &sSelectedChromosome);
	fsSelectedChromosome->SetWidth(142); // should be 200, but part occupied by "Selected Chromosome"
	fsSelectedChromosome->SetIndependentPos();
	fsSelectedChromosome->Align(pbCalculatePareto, AL_UNDER);
	pbStoreSelectedChromosome = new PushButton(fgRight, TR("Store Selected Chromosome"), (NotifyProc)&FormLandAllocation::StoreSelectedChromosome);
	pbStoreSelectedChromosome->Align(fsSelectedChromosome, AL_UNDER);

	initPointMapOut(false);
	fmc->Align(frCrossover, AL_UNDER);
	SetHelpItem("ilwisapp\\mask_points_dialog_box.htm");
	create();

	fgFunctionGraph->Replot();
	m_function = new ParetoGraphFunction();

	fsTotalFacilities->SetReadOnly(true);
	fsTotalDemands->SetReadOnly(true);
	fsSelectedChromosome->SetReadOnly(true);
}

FormLandAllocation::~FormLandAllocation()
{
	fgFunctionGraph->SetFunction(0);
	if (m_function)
		delete m_function;
	if (m_la)
		delete m_la;
}

int FormLandAllocation::MethodCallBack(Event*)
{
	rgMethod->StoreData();
	if (iMethod == 0) {
		butShow.ShowWindow(SW_SHOW);
		butDefine.ShowWindow(SW_SHOW);
	} else {
		butShow.ShowWindow(SW_HIDE);
		butDefine.ShowWindow(SW_HIDE);
	}
	return 0; 
}

int FormLandAllocation::FacilitiesCallBack(Event*)
{
	fpmFacilities->StoreData();
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
	fpmDemands->StoreData();
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

int FormLandAllocation::AdjustEliteCallBack(Event*)
{
	fiPopulation->StoreData();
	fiPareto->StoreData();
	fiElite->StoreData();
	fiElite->SetvalueRange(ValueRange(0, min(iPopulationSize,iNpareto)));
	return 0; 
}

int FormLandAllocation::GenerateParetoGraph(Event*)
{
	FormEntry *pfe = CheckData();
	if (pfe) 
	{
		MessageBeep(MB_ICONEXCLAMATION);
		pfe->SetFocus();
		return 0;
	}

	root->StoreData();
	pbCalculatePareto->Disable();
	pbStoreSelectedChromosome->Disable();
	AfxBeginThread(GenerateParetoGraphInThread, this);
	return 0;
}

UINT FormLandAllocation::GenerateParetoGraphInThread(LPVOID pParam)
{
	FormLandAllocation * pObject = (FormLandAllocation*)pParam;
	if (pObject == NULL)
	{
		return 1;
	}

	FileName fn(pObject->sOutMap);
	PointMap pmFacilities(pObject->sPointMapFacilities, fn.sPath());
	PointMap pmDemands(pObject->sPointMapDemands, fn.sPath());
	PointMap pmFacilitiesNoAttribute(PointMapLandAllocation::fnGetSourceFile(pmFacilities, fn));
	PointMap pmDemandsNoAttribute(PointMapLandAllocation::fnGetSourceFile(pmDemands, fn));
	pObject->m_la = new LandAllocation(pmFacilities, pmFacilitiesNoAttribute, pObject->sColFacilitiesType, pmDemands, pmDemandsNoAttribute, pObject->sColDemandsPreference,
							   pObject->iOptimalFacilities, pObject->fCapacitated, pObject->iStoppingCriteria, pObject->iGenerations, pObject->iPopulationSize, pObject->iNelite, pObject->iNpareto, pObject->rMutationPercent, pObject->rCrossoverPercent);

	Tranquilizer trq;
	std::vector<GAChromosome> pareto = pObject->m_la->GenerateParetoArray(trq);

	struct sort_pred { 
		bool operator()(const GAChromosome & left, const GAChromosome & right) { 
			return (left.rGetPartialScore1() < right.rGetPartialScore1());
		} 
	};
	std::sort(pareto.begin(), pareto.end(), sort_pred());
	std::vector<double> scoresX;
	std::vector<double> scoresY;

	for (int i = 0; i < pareto.size(); ++i) {
		scoresX.push_back(pareto[i].rGetPartialScore1());
		scoresY.push_back(pareto[i].rGetPartialScore2());
	}

	if (pObject->m_function)
	{
		pObject->m_function->SetData(scoresX, scoresY);
		pObject->fgFunctionGraph->SetFunction(pObject->m_function); // arranges that the XY-axes are re-set and plots the graph
	}
	else
		pObject->fgFunctionGraph->Replot();

	pObject->m_pareto = pareto;

	pObject->CallBackAnchorChangedInGraph(0);

	pObject->pbCalculatePareto->Enable();
	pObject->pbStoreSelectedChromosome->Enable();
	return 0;
}

int FormLandAllocation::CallBackAnchorChangedInGraph(Event*)
{
	if (m_function) {
		int index = m_function->iGetAnchorNr();
		if (index >= 0 && index < m_pareto.size()) {
			GAChromosome * chromosome = &m_pareto[index];
			sSelectedChromosome = String("%d: F=%.03f d=%.03f p=%.03f", index+1, chromosome->rGetFitness(), chromosome->rGetPartialScore1(), chromosome->rGetPartialScore2());
			fsSelectedChromosome->SetVal(sSelectedChromosome);
		} else {
			sSelectedChromosome = "No chromosome selected";
			fsSelectedChromosome->SetVal(sSelectedChromosome);
		}
	}
	return 0;
}

int FormLandAllocation::StoreSelectedChromosome(Event*)
{
	int index = m_function->iGetAnchorNr();
	if (index >= 0 && index < m_pareto.size()) {
		GAChromosome * chromosome = &m_pareto[index];
		FileName fn(sOutMap);
		PointMap pmFacilities(sPointMapFacilities, fn.sPath());
		PointMap pmDemands(sPointMapDemands, fn.sPath());
		PointMap pmFacilitiesNoAttribute(PointMapLandAllocation::fnGetSourceFile(pmFacilities, fn));
		PointMap pmDemandsNoAttribute(PointMapLandAllocation::fnGetSourceFile(pmDemands, fn));
		CoordSystem csyDest (pmFacilities->cs());
		CoordBounds cbMap (pmDemands->cb());
		cbMap += pmFacilities->cb();
		FileName fnOut (fn, ".mpp", true);
		fnOut.sFile += "_pareto";
		fnOut = FileName::fnUnique(fnOut);
		PointMap pntMap(fnOut, csyDest, cbMap, pmFacilitiesNoAttribute->dm());
		m_la->StoreChromosome(chromosome, pntMap.ptr());
		pntMap->SetDescription(sSelectedChromosome);
	} else if (m_pareto.size() == 0)
		MessageBox("Please calculate pareto first");
	else
		MessageBox("Please select a chromosome to store");

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
		sExpr = String("PointMapLandAllocation(%S,%S,%S,%S,%d,%s,%d,%d,%d,%d,%d,%f,%f)", sPointMapFacilities, sColFacilitiesType, sPointMapDemands, sColDemandsPreference, iOptimalFacilities, fCapacitated ? "capacitated" : "plain", iStoppingCriteria, iGenerations, iPopulationSize, iNelite, iNpareto, rMutationPercent / 100.0, rCrossoverPercent / 100.0);
	execPointMapOut(sExpr);  
	return 0;
}
