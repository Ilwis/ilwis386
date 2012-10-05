#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\frmmapcr.h"
#include "client\formelements\fldlist.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "FormIsoCluster.h"
#include "Engine\Base\Round.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

LRESULT Cmdisocluster(CWnd *wnd, const String& s)
{
	new FormIsoCluster(wnd, s.c_str());
	return -1;
}

FormIsoCluster::FormIsoCluster(CWnd* mw, const char* sPar)
:FormMapCreate(mw, TR("Iso cluster"))
{
	numclasses = 16;
	iter = 25;
	stdev = 7;
	mindist = 1;
	maxlump = 5;
	minsamplesize  = -1;

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
	fmpl = new FieldMapList(root, TR("Input"), &mplName);
	fmpl->SetCallBack((NotifyProc)&FormIsoCluster::changeInput);
	fclass = new FieldInt(root, TR("Number of desired Classes"), &numclasses);
	fsamplesize = new FieldInt(root, TR("Minimum Class sample size(pixels)"), &minsamplesize);
	fiter = new FieldInt(root, TR("Number of iterations"), &iter);
	fstdev = new FieldReal(root, TR("Maximum Standard Deviation within a Class"), &stdev);
	fmindist = new FieldReal(root, TR("Minimum distance between two Class means"), &mindist);
	fmaxlump = new FieldInt(root, TR("maximum number of pairs to lump"), &maxlump);


	initMapOut(false, false);
	create();
}


int FormIsoCluster::changeInput(Event *ev) {
	fmpl->StoreData();
	if ( mplName == "")
		return 0;

	MapList mpl = MapList(FileName(mplName));
	if ( mpl.fValid() && mpl->iSize() > 0) {
		long sz = mpl->rcSize().Row * mpl->rcSize().Col;
		minsamplesize = sz * 0.1;
		minsamplesize = rRound(minsamplesize);
		fsamplesize->SetVal(minsamplesize);
	}
	return 1;
}

FormEntry *FormIsoCluster::CheckData() {
	FormMapCreate::CheckData();

	if ( mplName == "" || FileName(mplName).fExist() == false) {
		return fmpl;
	} else if ( minsamplesize <= 0) {
		return fsamplesize;
	} else if ( numclasses < 2)
		return fclass;
	else if ( iter < 2)
		return fiter;
	else if ( stdev < 0.1)
		return fstdev;
	else if ( mindist < 1) {
		return fmindist;
	} else if ( maxlump < 3) {
		return fmaxlump;
	}
	return 0;

}
int FormIsoCluster::exec() 
{
	FormMapCreate::exec();
	String sExpr = String("MapIsoCluster(%S,%d, %d, %f, %f, %d, %d )",mplName, numclasses, minsamplesize, stdev,mindist, maxlump, iter);

	execMapOut(sExpr);  
	return 0;
}