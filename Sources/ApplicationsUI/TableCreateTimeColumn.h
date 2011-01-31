#pragma once

LRESULT Cmdtimecolumn(CWnd *wnd, const String& s);

class TableCreateTimeColumnForm : public FormWithDest {
public:
	TableCreateTimeColumnForm(CWnd* wPar, const String& sTitle);
	int exec();
private:
	FormEntry *CheckData();
	int selectCB(Event *);
	String maplist;
	int year1,year2;
	int baseyear;
	int month1, month2;
	int day1, day2;
	int hour1, hour2;
	int minutes1, minutes2;
	int decade1,decade2;
	bool fyear, fmonth, fday, fminute,fhour,fdecade;
	String tablename;
	CheckBox *cbDecade, *cbHour, *cbMinute, *cbDay, *cbMnth;
	FieldInt *fiDay1, *fiDay2,*fiHour1, *fiHour2, *fiMinute1, *fiMinute2, *fiMonth1, *fiMonth2,*fiDecade1,*fiDecade2;
};