#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\frmmapcr.h"
#include "client\formelements\fldlist.h"
#include "ApplicationsUI\FormMapAggregateMapList.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

LRESULT Cmdaggregatemaplist(CWnd *wnd, const String& s)
{
	new FormMapAggregateMapList(wnd, s.c_str());
	return -1;
}

FormMapAggregateMapList::FormMapAggregateMapList(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Aggregate Maplist"))
{
	methods.push_back(TR("Average.average"));
	methods.push_back(TR("Minimum.Min"));
	methods.push_back(TR("Maximum.Max"));
	methods.push_back(TR("Median.median"));
	methods.push_back(TR("Sum.sum"));
	methods.push_back(TR("Count.count"));

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
					sOutMap = fn.sFullName(false);
		}
	}
	new FieldMapList(root, TR("Input"), &mplName);
	new FieldOneSelectString(root,TR("Method"),&methodIndex, methods);
	initMapOut(false, false);
	//SetHelpItem("ilwisapp\\fill_sinks_dialogbox.htm");
	create();
}


int FormMapAggregateMapList::exec() 
{
	FormMapCreate::exec();
	FileName fn(sOutMap);
	FileName fnMap(mplName); 
	mplName = fnMap.sRelativeQuoted(false,fn.sPath());
	String m = String(methods[methodIndex]).sHead(".");
	String sExpr = String("MapAggregateMapList(%S,%S)",mplName, m);

	execMapOut(sExpr);  
	return 0;
}