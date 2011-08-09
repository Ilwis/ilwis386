#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\frmmapcr.h"
#include "client\formelements\fldlist.h"
#include "ApplicationsUI\FormMapListChangeDetection.h"
#include "Headers\messages.h"
#include "Engine\Base\System\Engine.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

LRESULT Cmdchangedetection(CWnd *wnd, const String& s)
{
	new FormMapListChangeDetection(wnd, s.c_str());
	return -1;
}

FormMapListChangeDetection::FormMapListChangeDetection(CWnd* mw, const char* sPar)
:FormGeneralApplication(mw, TR("Aggregate Maplist")), choice(0),threshold(0), undefHandling(false)
{
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
	new FieldMapList(root, TR("First Input"), &mplName);
	RadioGroup *rg = new RadioGroup(root,TR("Comparison source"),&choice);
	RadioButton *rb = new RadioButton(rg,TR("Other MapList"));
	FieldMapList *fmpl = new FieldMapList(rb,"",&name);
	fmpl->Align(rb, AL_AFTER);
	rb->SetIndependentPos();

	rb = new RadioButton(rg,TR("Baseline map"));
	FieldMap *fmp = new FieldMap(rb,"",&name);
	fmp->Align(rb, AL_AFTER);
	rb->SetIndependentPos();

	rb = new RadioButton(rg,TR("Previous Index"));

	new FieldReal(root,TR("Threshold"),&threshold);
	CheckBox *cb = new CheckBox(root,TR("Treat Undefined deltas as zero deltas"), &undefHandling);
	cb->SetIndependentPos();
	new FieldMapCreate(root, TR("&Output MapList"), &mplOut);
	//SetHelpItem("ilwisapp\\fill_sinks_dialogbox.htm");
	create();
}


int FormMapListChangeDetection::exec() 
{
	FormGeneralApplication::exec();
	FileName fnOut(mplOut,".mpl");
	FileName fnIn1(mplName);
	FileName fnIn2(name);

	String expr;
	if ( choice == 0 ) {
		expr = String("%S=MapListChangeDetection(%S, %S, %f, %s)", fnOut.sRelative(), fnIn1.sRelative(),fnIn2.sRelative(),threshold, undefHandling ? "true": "false");
	}
	if ( choice == 1) {
		expr = String("%S=MapListChangeDetection(%S, %S, %f, %s)", fnOut.sRelative(), fnIn1.sRelative(),fnIn2.sRelative(),threshold, undefHandling ? "true": "false");
	} 
	if ( choice == 2) {
		expr = String("%S=MapListChangeDetection(%S, %f, %s)", fnOut.sRelative(), fnIn1.sRelative(),threshold, undefHandling ? "true": "false");
	}
	getEngine()->Execute(expr);
	getEngine()->Execute(String("open %S", fnOut.sRelative()));
	
	return 0;
}