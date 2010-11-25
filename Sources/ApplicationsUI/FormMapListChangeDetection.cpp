#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\frmmapcr.h"
#include "client\formelements\fldlist.h"
#include "ApplicationsUI\FormMapListChangeDetection.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

LRESULT Cmdchangedetection(CWnd *wnd, const String& s)
{
	new FormMapListChangeDetection(wnd, s.scVal());
	return -1;
}

FormMapListChangeDetection::FormMapListChangeDetection(CWnd* mw, const char* sPar)
:FormGeneralApplication(mw, TR("Aggregate Maplist")), choice(0)
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

	new FieldMapCreate(root, TR("&Output MapList"), &mplOut);
	//SetAppHelpTopic(htpFillSinks);
	create();
}


int FormMapListChangeDetection::exec() 
{
	FormGeneralApplication::exec();
	
	execMapOut(sExpr);  
	return 0;
}