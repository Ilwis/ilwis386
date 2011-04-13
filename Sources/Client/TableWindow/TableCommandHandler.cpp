/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
// TableCommandHandler.cpp: implementation of the TableCommandHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Headers\constant.h"
#include "Client\ilwis.h"
#include "Headers\hourglss.h"
#include "Client\ilwis.h"
#include "engine\base\system\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\commandhandler.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Client\TableWindow\TableCommandHandler.h"
#include "Engine\Scripting\Calc.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Engine\Applications\COLVIRT.H"
//#include "Engine\Table\COLCALC.H"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldaggr.h"
#include "Client\FormElements\fldsmv.h"
#include "Engine\Table\tblview.h"
#include "Engine\Table\tblinfo.h"
#include "Engine\Matrix\Matrxobj.h"
#include "Engine\Matrix\Confmat.h"
#include "ApplicationsUI\Tblforms.h"
#include "Headers\messages.h"
#include "Client\Editors\Utils\GeneralBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Base\datawind.h"
#include "Client\TableWindow\RecordBar.h"
#include "Client\TableWindow\TableWindow.h"
#include "Client\TableWindow\COLMAN.H"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Forms\SimpleCalcResultsForm.h"
#include "Headers\Hs\Table.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TableCommandHandler::TableCommandHandler()
{
	tbldoc = 0;
	//addCommand(CommandHandlerUI::createCommandInfo("statistics","",createView,"","","","",0,""));
	//AddCommand("statistics", (CommandFunction)& TableCommandHandler::CmdTblStats);
	//AddCommand("clmaggregate", (CommandFunction)& TableCommandHandler::CmdAggregate);
	//AddCommand("join", (CommandFunction)& TableCommandHandler::CmdJoin);
	//AddCommand("leastsquares", (CommandFunction)& TableCommandHandler::CmdLeastSquares);
	//AddCommand("cumulative", (CommandFunction)& TableCommandHandler::CmdCumulative);
	//AddCommand("semivariogram", (CommandFunction)& TableCommandHandler::CmdSemivariogram);
	//AddCommand("graph", (CommandFunction)& TableCommandHandler::CmdGraph);
	//AddCommand("rosediagram", (CommandFunction)& TableCommandHandler::CmdRoseDiagram);
	//AddCommand("slicing", (CommandFunction)& TableCommandHandler::CmdSlicing);
	//AddCommand("sorton", (CommandFunction)& TableCommandHandler::CmdSortOn);
	//AddCommand("updateallcolumns", (CommandFunction)& TableCommandHandler::CmdUpdateAllColumns);
	//AddCommand("addinfo", (CommandFunction)& TableCommandHandler::CmdAdditionalInfo);
	//AddCommand("addrecord", (CommandFunction)& TableCommandHandler::CmdAddRecord);
	//AddCommand("prop", (CommandFunction)& TableCommandHandler::CmdProp);
	//AddCommand("colman", (CommandFunction)& TableCommandHandler::CmdColMan);
	//AddCommand("confmat", (CommandFunction)& TableCommandHandler::CmdConfMat);
	//AddCommand("invmol", (CommandFunction)& TableCommandHandler::CmdInvMol);
	//AddCommand("findshifts", (CommandFunction)& TableCommandHandler::CmdFindDatumShifts);
	//AddCommand("find7parms", (CommandFunction)& TableCommandHandler::CmdFind7Parameters);
	//AddCommand("findazimdist", (CommandFunction)& TableCommandHandler::CmdFindAzimuthAndDistance);
}

TableCommandHandler::~TableCommandHandler()
{

}

LRESULT TableCommandHandler::Execute(const String& sCmd, CWnd *parent)
{	
	if (0 == tbldoc) {
		CDocument* doc = wndOwner->GetActiveDocument();
		tbldoc = dynamic_cast<TableDoc*>(doc);
		if (0 == tbldoc) 
			return false;
	}
  if (sCmd == "") 
		return true;
  if ( sCmd[0] == '?' ) // simple calc
    return fCmdSimpleCalc(sCmd.substr(1));
  else { // Jelle: changed order, because commands can have flags with '='
    try {
		 int f = BaseCommandHandlerUI::Execute(sCmd);
     if (f >= 0)
       return true;
    }
    catch (const ErrorObject&) {
    }
  }
  return tbldoc->fCmdCalc(sCmd); 
}

bool TableCommandHandler::fCmdSimpleCalc(const String& sCmd)
{
  String sRes = Calculator::sSimpleCalc(sCmd, tbldoc->table());
	String sMsg("%S = %S ", sRes, sCmd);
	//wndOwner->MessageBox(sMsg.scVal(), STBTitleSimpleCalc.scVal(), MB_OK | MB_ICONINFORMATION);

	TableWindow *tw = dynamic_cast<TableWindow *>( AfxGetMainWnd()) ;
	if ( tw)
	{
		tw->frmSimpleCalcResults()->ShowWindow(SW_SHOW);
		tw->frmSimpleCalcResults()->AddString( sMsg);
	}	
	return true;
}
//
//void CmdTblStats(CWnd *wndOwner, const String& s)
//{
//  
//  TblStatsForm frm(wndOwner, tbldoc->tvw);
//  if (frm.fOkClicked()) {
//		char* str = frm.sExpr.sVal();
//		wndOwner->SendMessage(ILWM_EXECUTE, 1, (LPARAM)str);
//	}
//	ReroutPost("tblstatistics " + s);
//}
//
//
//void TableCommandHandler::CmdRoseDiagram(const String& s)
//{
//  tbldoc->OnNewRoseDiagram();
//}
//
//void TableCommandHandler::CmdGraph(const String& s)
//{
//  tbldoc->OnNewGraph();
//}
//
//
//void TableCommandHandler::CmdUpdateAllColumns(const String& s)
//{
//  for (int i = 0; i < tbldoc->tvw->iCols(); ++i)
//    tbldoc->tvw->cv(i)->MakeUpToDate();
//	tbldoc->UpdateAllViews(0);
//}
//
//void TableCommandHandler::CmdProp(const String& s)
//{
//	String sCmd("prop %S", tbldoc->tvw->sNameQuoted(true));
//	IlwWinApp()->Execute(sCmd);
//}
//
//void TableCommandHandler::CmdAdditionalInfo(const String& s)
//{
//	tbldoc->OnShowAddInfo();
////	String sCmd("addinfo %S", tbldoc->tvw->sName(true));
////	IlwWinApp()->Execute(sCmd);
//}
//
//void TableCommandHandler::CmdAddRecord(const String& s)
//{
//	try {
//		if (tbldoc->tvw->fReadOnly())
//			throw ErrorObject(STBErrTableReadOnly);
//		if (tbldoc->tvw->dm()->pdnone() == 0)
//			throw ErrorObject(STBErrAddRecordNone);
//		// only allow add record for real tables
//		if (".tbt" != tbldoc->tbl->fnObj.sExt)
//			throw ErrorObject(STBErrAddRecordNone);
//		int iNewRecs;
//    if ("" == s)
//      iNewRecs = 1;
//    else
//      iNewRecs = s.iVal();
//		if (iNewRecs <= 0) 
//			throw ErrorObject(String("%S : %S", SDATErrInvalidParameter, s));
//		tbldoc->tvw->iRecNew(iNewRecs);
//		tbldoc->UpdateAllViews(0);
//	}
//	catch (const ErrorObject& err) {
//		err.Show();
//	}
//}
//
//void TableCommandHandler::CmdColMan(const String&)
//{
//	ColumnManageForm frm(tbldoc, tbldoc->tvw);
//	tbldoc->UpdateAllViews(0);
//}
//
//void TableCommandHandler::CmdConfMat(const String&)
//{
//	TableView* tvw = tbldoc->tvw;
//  ConfusionMatrixForm frm(tbldoc->wndGetActiveView(), tvw);
//  if (!frm.fOkClicked())
//    return; 
//	String sCmd("show MatrixConfusion(%S,%S,%S,%S)", tvw->fnObj.sFullNameQuoted(), frm.sCol1.sQuote(), frm.sCol2.sQuote(), frm.sColFreq.sQuote());
//	IlwWinApp()->Execute(sCmd);
//}
//
//void TableCommandHandler::CmdInvMol(const String& s)
//{
//	InverseMolodenskyForm frm(wndOwner, tbldoc->tvw);
//  if (frm.fOkClicked()) {
//		char* str = frm.sExpr.sVal();
//		wndOwner->SendMessage(ILWM_EXECUTE, 1, (LPARAM)str);
//	}
//}
//
//void TableCommandHandler::CmdFindDatumShifts(const String& s)
//{
//	FindMolodenskyShiftsForm frm(wndOwner, tbldoc->tvw);
//  if (frm.fOkClicked()) {
//		char* str = frm.sExpr.sVal();
//		wndOwner->SendMessage(ILWM_EXECUTE, 1, (LPARAM)str);
//	}
//}
//
//void TableCommandHandler::CmdFind7Parameters(const String& s)
//{
//	Find7ParametersForm frm(wndOwner, tbldoc->tvw);
//  if (frm.fOkClicked()) {
//		char* str = frm.sExpr.sVal();
//		wndOwner->SendMessage(ILWM_EXECUTE, 1, (LPARAM)str);
//	}
//}
//
//void TableCommandHandler::CmdFindAzimuthAndDistance(const String& s)
//{
//	FindAzimuthAndDistanceForm frm(wndOwner, tbldoc->tvw);
//  if (frm.fOkClicked()) {
//		char* str = frm.sExpr.sVal();
//		wndOwner->SendMessage(ILWM_EXECUTE, 1, (LPARAM)str);
//	}
//}
