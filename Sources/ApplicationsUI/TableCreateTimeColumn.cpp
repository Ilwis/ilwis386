#include "Client\Headers\formelementspch.h"
#include "ApplicationsUI\TableCreateTimeColumn.h"
#include "Engine\Base\System\engine.h"
#include "Client\FormElements\fldlist.h"

LRESULT Cmdtimecolumn(CWnd *wnd, const String& s)
{
	new TableCreateTimeColumnForm(wnd, s);
	return -1;
}

TableCreateTimeColumnForm::TableCreateTimeColumnForm(CWnd* wPar, const String& parm) : FormWithDest(wPar, TR("Parse maplist names for time information")){
	year1 = year2 = baseyear = month1 = month2 = day1 = day2 = hour1 = hour2 = minutes1 = minutes2 = decade1 = decade2 = -1;
	fyear = fmonth = fday = fminute = fhour = fdecade = false;
	FieldMapList * fmlist = new FieldMapList(root,TR("Maplist"),&maplist);
	fmlist->SetIndependentPos();
	new StaticText(root,TR("Character positions"),true);
	StaticText *st = new StaticText(root,TR("Year"));
	FieldInt *fiYear1 = new FieldInt(root,TR("start"), &year1);
	fiYear1->Align(st, AL_AFTER);
	FieldInt *fiYear2 = new FieldInt(root,TR("end"), &year2);
	FieldInt *fiBaseYear = new FieldInt(root,TR("Base year"), &baseyear);
	fiBaseYear->Align(fiYear2, AL_AFTER);
	fiYear2->Align(fiYear1, AL_AFTER);

	cbMnth = new CheckBox(root, TR("Month"),&fmonth);
	cbMnth->Align(st, AL_UNDER);
	fiMonth1 = new FieldInt(cbMnth, TR("start"), &month1);
	fiMonth1->Align(cbMnth, AL_AFTER);
	fiMonth2 = new FieldInt(cbMnth, TR("end"), &month2);
	fiMonth2->Align(fiMonth1, AL_AFTER);

	cbDay = new CheckBox(root, TR("Day"), &fday);
	cbDay->Align(cbMnth, AL_UNDER);
	fiDay1 = new FieldInt(cbDay, TR("start"), &day1);
	fiDay1->Align(cbDay, AL_AFTER);
	fiDay2 = new FieldInt(cbDay, TR("end"), &day2);
	fiDay2->Align(fiDay1, AL_AFTER);

	cbHour = new CheckBox(root, TR("Hour"), &fhour);
	cbHour->Align(cbDay, AL_UNDER);
	fiHour1 = new FieldInt(cbHour, TR("start"), &hour1);
	fiHour1->Align(cbHour, AL_AFTER);
	fiHour2 = new FieldInt(cbHour, TR("end"), &hour2);
	fiHour2->Align(fiHour1, AL_AFTER);

	cbMinute = new CheckBox(root, TR("Minute"),&fminute);
	cbMinute->Align(cbHour, AL_UNDER);
	fiMinute1 = new FieldInt(cbMinute, TR("start"), &minutes1);
	fiMinute1->Align(cbMinute, AL_AFTER);
	fiMinute2 = new FieldInt(cbMinute, TR("end"), &minutes2);
	fiMinute2->Align(fiMinute1, AL_AFTER);

	cbDecade = new CheckBox(root, TR("Decade"),&fdecade);
	cbDecade->SetCallBack((NotifyProc)&TableCreateTimeColumnForm::selectCB);
	cbDecade->Align(cbMinute, AL_UNDER);
	fiDecade1 = new FieldInt(cbDecade, TR("start"), &decade1);
	fiDecade1->Align(cbDecade, AL_AFTER);
	fiDecade2 = new FieldInt(cbDecade, TR("end"), &decade2);
	fiDecade2->Align(fiDecade1, AL_AFTER);

	FieldBlank *fb = new FieldBlank(root);
	fb->Align(cbDecade, AL_UNDER);
	new FieldString(root,TR("Output table"), &tablename);

	create();
}

int TableCreateTimeColumnForm::selectCB(Event *) {
	cbDecade->StoreData();
	cbDay->StoreData();
	cbMinute->StoreData();
	if ( fdecade) {
		cbDay->SetVal(false);
		cbMinute->SetVal(false);
		fiDay1->Hide();
		fiDay2->Hide();
		fiMinute1->Hide();
		fiMinute2->Hide();
	}
	if ( fday) {
		cbDecade->SetVal(false);
		fiDecade1->Hide();
		fiDecade2->Hide();
	}
	if ( fminute) {
		cbDecade->SetVal(false);
		fiDecade1->Hide();
		fiDecade2->Hide();
	}
	return 1;
}

FormEntry *TableCreateTimeColumnForm::CheckData() {
	return 0;
}

int TableCreateTimeColumnForm::exec() {
	FormWithDest::exec();
	String format;
	format += String("year=%d:%d", year1, year2);
	if (baseyear != -1)
		format+=String(":%d",baseyear);
	if ( fmonth) {
		format += ";";
		format += String("month=%d:%d",month1, month2);
	}
	if ( fday) {
		format += ";";
		format += String("day=%d:%d",day1, day2);
	}
	if ( fhour) {
		format += ";";
		format += String("hour=%d:%d",hour1, hour2);
	}
	if ( fminute) {
		format += ";";
		format += String("minute=%d:%d",minutes1, minutes2);
	}
	if ( fdecade) {
		format += ";";
		format += String("decade=%d:%d",decade1, decade2);
	}
	String expr("%S:=TableCreateTimeColumn(%S,'%S')",tablename,maplist,format);
	getEngine()->Execute(expr);

	return 1;
}