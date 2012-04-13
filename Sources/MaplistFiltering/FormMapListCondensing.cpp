#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\frmmapcr.h"
#include "client\formelements\fldlist.h"
#include "FormMapListCondensing.h"
#include "Headers\messages.h"
#include "Engine\Base\System\Engine.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

LRESULT CmdMapListCondensing(CWnd *wnd, const String& s)
{
	new FormMapListCondensing(wnd, s.c_str());
	return -1;
}

FormMapListCondensing::FormMapListCondensing(CWnd* mw, const char* sPar)
:FormGeneralApplication(mw, TR("Condense Maplist")), startIndex(0), methodIndex(0), step(2)
{
	methods.push_back(TR("Average") + ".average");
	methods.push_back(TR("Minimum") + ".Min");
	methods.push_back(TR("Maximum") + ".Max");
	methods.push_back(TR("Median") + ".median");

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
					mplOut = fn.sFullName(false);
		}
	}
	new FieldMapList(root, TR("MapList"), &mplName);
	new FieldInt(root,TR("Starting map index"),&startIndex,Domain(), true);
	new FieldInt(root,TR("Step size"),&step,ValueRange(2,100),true);


	new FieldOneSelectString(root,TR("Method"),&methodIndex, methods);

	new FieldMapCreate(root, TR("&Output MapList"), &mplOut);
	//SetHelpItem("ilwisapp\\fill_sinks_dialogbox.htm");
	create();
}


int FormMapListCondensing::exec() 
{
	FormGeneralApplication::exec();
	FileName fnOut(mplOut,".mpl");
	FileName fnIn(mplName, ".mpl");
	String method = "mean";
	if ( methodIndex == 0)
		method = "mean";
	if ( methodIndex == 1)
		method = "min";
	if ( methodIndex == 2)
		method = "max";
	if ( methodIndex == 3)
		method = "median";

	String expr;
	expr = String("%S=MapListCondensing(%S, %d, %d, %S)", fnOut.sRelative(), fnIn.sRelative(),startIndex, step, method);
	getEngine()->Execute(expr);
	
	return 0;
}