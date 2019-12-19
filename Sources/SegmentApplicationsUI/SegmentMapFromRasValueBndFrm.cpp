#include "Client\Headers\AppFormsPCH.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\Round.h"
#include "SegmentApplicationsUI\SegmentMapFromRasValueBndFrm.h"
#include "Headers\Hs\DOMAIN.hs"

LRESULT Cmdsegmentisoline(CWnd *wnd, const String& s)
{
	new SegmentMapFromRasValueBndFrm(wnd, s.c_str());
	return -1;
}

SegmentMapFromRasValueBndFrm::SegmentMapFromRasValueBndFrm(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, TR("Iso lines")),intervalType(0),vr(ValueRange(0,1000,100)), frags(0)
{
	if (sPar)
	{
		TextInput ip(sPar);
		TokenizerBase tokenizer(&ip);
		String sVal;
		for (;;) {
			Token tok = tokenizer.tokGet();
			sVal = tok.sVal();
			if (sVal == "")
				break;
			FileName fn(sVal);
			if (sInMap1 == "")
				sInMap1 = fn.sFullNameQuoted(true);
			else  
				sOutMap = fn.sFullName(true);
		}
	}
	FieldMap *fmMap = new FieldMap(root, TR("Raster Map"), &sInMap1, new MapListerDomainType(dmVALUE|dmIMAGE, false));
	fmMap->SetCallBack((NotifyProc)&SegmentMapFromRasValueBndFrm::setValueRange);
	FieldGroup *fg = new FieldGroup(root);
	RadioGroup *rg = new RadioGroup(fg,TR("Iso Line Intervals"), &intervalType);
	RadioButton *rb1 = new RadioButton(rg,TR("Linear"));
	fldVr = new FieldValueRange(rb1,"",&vr,0,ValueRange(-1e9,1e9,100000),true);
	if (fldVr->childlist().size() > 1) {
		FormEntry * feFieldReal = fldVr->childlist()[1];
		if (feFieldReal->childlist().size() > 0) {
			StaticTextSimple * st = dynamic_cast<StaticTextSimple*>(feFieldReal->childlist()[0]);
			if (st)
				st->SetVal(TR("Interval"));
		}
	}
	fldVr->SetIndependentPos();
	RadioButton *rb2 = new RadioButton(rg,TR("Comma-separated heights"));
	FieldString * fs = new FieldString(rb2,"", &sequence);

	rb2->Align(rb1, AL_UNDER);
	fg->SetIndependentPos();

	FieldBlank *fb = new FieldBlank(root);
	fb->Align(rb2, AL_UNDER);
	new FieldReal(root,"Minimum Fragment size",&frags);
			
    initSegmentMapOut(false);
	create();
}

int SegmentMapFromRasValueBndFrm::setValueRange(Event *ev) {
	root->StoreData();
	if ( sInMap1 != "") {
		Map rmp = Map(FileName(sInMap1));
		DomainValueRangeStruct dvr = rmp->dvrs();
		ValueRange vr(dvr.rrMinMax().rLo(), dvr.rrMinMax().rHi(), rRound(dvr.rrMinMax().rWidth() / 10));
		fldVr->SetVal(vr);

	}
	return 1;
}

FormEntry *SegmentMapFromRasValueBndFrm::CheckData() {
	return NULL;
}

int SegmentMapFromRasValueBndFrm::exec()
{
	FormSegmentMapCreate::exec();
	FileName fnMap1(sInMap1);
	String sExpr;
	if ( intervalType == 0)
		sExpr = String("SegmentMapFromRasValueBnd(%S,%f,%f,%f,8,smooth,%f)", fnMap1.sRelativeQuoted(),vr->rrMinMax().rLo(), vr->rrMinMax().rHi(), vr->rStep(),frags);
	else
		sExpr = String("SegmentMapFromRasValueBnd(%S,\"%S\",8,smooth,%f)", fnMap1.sRelativeQuoted(),sequence,frags);

	execSegmentMapOut(sExpr);

	return 0;
}
