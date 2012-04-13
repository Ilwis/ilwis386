#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\frmtblcr.h"
#include "client\formelements\fldlist.h"
#include "Client\Forms\frmmapcr.h"
#include "TableAttributeFromMapListForm.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

LRESULT createTableFromMapList(CWnd *wnd, const String& s)
{
	new FormTableFromMapList(wnd, s.c_str());
	return -1;
}

FormTableFromMapList::FormTableFromMapList(CWnd* mw, const char* sPar)
:FormTableCreate(mw, TR("Aggregate Maplist"))
{
	methods.push_back(TR("Average") + ".average");
	methods.push_back(TR("Minimum") + ".Min");
	methods.push_back(TR("Maximum") + ".Max");
	methods.push_back(TR("Median") + ".median");
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
	new FieldMapList(root, TR("Input"), &mplName);
	FieldOneSelectString *fs = new FieldOneSelectString(root,TR("Method"),&methodIndex, methods);
	FieldGroup *fg = new FieldGroup(root);


	initTableOut(false);
	//SetHelpItem("ilwisapp\\fill_sinks_dialogbox.htm");
	create();
}


int FormTableFromMapList::exec() 
{
	FormTableCreate::exec();
	FileName fn(sOutTable);
	FileName fnMap(mplName); 
	mplName = fnMap.sRelativeQuoted(false,fn.sPath());
	String m ;
	
	if ( methodIndex == 5)
		m = "stdev";
	if ( methodIndex == 4)
		m = "sum";
	if ( methodIndex == 1)
		m = "minimum";
	if ( methodIndex == 2)
		m = "maximum";
	if ( methodIndex == 3)
		m = "median";
	if ( methodIndex == 6)
		m = "predominant";
	if ( methodIndex == 0)
		m = "average";

	String sExpr = String("TableMapListAttributeFrom(%S,histogram,%S",mplName, m);


	execTableOut(sExpr);  
	return 0;
}