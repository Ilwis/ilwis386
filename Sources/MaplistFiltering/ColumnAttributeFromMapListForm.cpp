#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\frmtblcr.h"
#include "client\formelements\fldlist.h"
#include "Client\Forms\frmmapcr.h"
#include "ColumnAttributeFromMapListForm.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\Editors\Utils\GeneralBar.h"
#include "engine\base\system\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Engine\Base\System\commandhandler.h"
#include "Client\TableWindow\TableCommandHandler.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Client\TableWindow\TableSummaryPaneView.h"
#include "Engine\Table\tblview.h"
#include "Client\Base\datawind.h"
#include "Client\TableWindow\TableWindow.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

LRESULT createColumnAttributeFromMapList(CWnd *wnd, const String& s)
{
	TableWindow *tbw = dynamic_cast<TableWindow *>(wnd);
	String tableName;
	if ( tbw) {
		tableName = tbw->tvw()->fnTbl().sPhysicalPath();
		String sexpr("%S %S", s, tableName);
		new FormColumnFromMapList(wnd, sexpr.c_str());
	}else
		new FormColumnFromMapList(wnd, s.c_str());
	return -1;
}

FormColumnFromMapList::FormColumnFromMapList(CWnd* mw, const char* sPar)
:FormTableCreate(mw, TR("Aggregate Maplist"))
{
	methods.push_back(TR("Average.average"));
	methods.push_back(TR("Minimum.Min"));
	methods.push_back(TR("Maximum.Max"));
	methods.push_back(TR("Median.median"));
	methods.push_back(TR("Sum"));
	methods.push_back(TR("Standard deviation"));
	methods.push_back(TR("Predominant"));

	if (sPar) {
		TextInput inp(sPar);
		TokenizerBase tokenizer(&inp);
		String sVal;
		for (;;) {
			Token tok = tokenizer.tokGet();
			sVal = tok.sVal();
			if (sVal == "")
				break;
			FileName fn(sVal);
			if (fn.sExt == ".mpl" || fn.sExt == "") 
				if (mplName == "")
					mplName = fn.sFullNameQuoted(true);
				else
					sOutTable = fn.sFullName(false);
		}
	}
	fldmpl = new FieldMapList(root, TR("Input"), &mplName);
	fldmpl->SetCallBack((NotifyProc)&FormColumnFromMapList::setDefaultTable);

	fldTab = new FieldString(root, TR("Table"), &sOutTable);
	new FieldString(root, TR("Column name"), &sCol);

	FieldOneSelectString *fs = new FieldOneSelectString(root,TR("Method"),&methodIndex, methods);
	FieldGroup *fg = new FieldGroup(root);


	//initTableOut(false);
	//SetHelpItem("ilwisapp\\fill_sinks_dialogbox.htm");
	create();
}

int FormColumnFromMapList::setDefaultTable(Event *ev) {
	fldmpl->StoreData();
	if ( mplName == "")
		return 1;

	MapList mpl(mplName);
	if ( mpl.fValid() && mpl->fTblAtt()) {
		sOutTable = mpl->tblAtt()->sName();
		fldTab->SetVal(sOutTable);
	} else
		sOutTable = mpl->fnObj.sFile + ".tbt";

	return 1;
}

int FormColumnFromMapList::exec() 
{
	FormTableCreate::exec();
	FileName fn(sOutTable);
	FileName fnMap(mplName); 
	mplName = fnMap.sRelativeQuoted(false,fn.sPath());
	String m = String(methods[methodIndex]).sHead(".");
	m = m.toLower();
	if ( m == "standard deviation")
		m = "stdev";

	String sExpr = String("ColumnAttributeFromMapList(%S,%S,%S,histogram,%S)",sOutTable,sCol,  mplName, m);


	execTableOut(sExpr);  
	return 0;
}