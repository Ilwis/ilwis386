#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\frmmapcr.h"
//#include "client\formelements\fldlist.h"
#include "ApplicationsUI\FormMapColorFromRpr.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

LRESULT Cmdmapcolorfromrpr(CWnd *wnd, const String& s)
{
	new FormMapColorFromRpr(wnd, s.c_str());
	return -1;
}

FormMapColorFromRpr::FormMapColorFromRpr(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Create Color Map From Map and Representation"))
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
			if (fn.sExt == ".mpr" || fn.sExt == "") 
				if (mpName == "")
					mpName = fn.sFullNameQuoted(true);
				else
					sOutMap = fn.sFullName(false);
		}
	}
	new FieldMap(root, TR("Input"), &mpName);
	new FieldRepresentation(root,TR("Representation"), &rprName);
	initMapOut(false, false);
	create();
}


int FormMapColorFromRpr::exec() 
{
	FormMapCreate::exec();
	FileName fn(sOutMap);
	FileName fnMap(mpName); 
	mpName = fnMap.sRelativeQuoted(false,fn.sPath());
	String sExpr = String("MapColorFromRpr(%S,%S)", mpName, rprName);

	execMapOut(sExpr);  
	return 0;
}