/***************************************************************
ILWIS integrates image, vector and thematic data in one unique 
and powerful package on the desktop. ILWIS delivers a wide 
range of feautures including import/export, digitizing, editing, 
analysis and display of data as well as production of 
quality mapsinformation about the sensor mounting platform

Exclusive rights of use by 52°North Initiative for Geospatial 
Open Source Software GmbH 2007, Germany

Copyright (C) 2007 by 52°North Initiative for Geospatial
Open Source Software GmbH

Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
tel +31-534874371

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program (see gnu-gpl v2.txt); if not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA or visit the web page of the Free
Software Foundation, http://www.fsf.org.

Created on: 2007-02-8
***************************************************************/
// dt.c
// Implementation of Date ObjectTime Interface for ILWIS 2.0
// 28 may 1993, by Wim Koolhoven
// (c) Computer Department ITC

#include "Headers\toolspch.h"

#define YEARDAYS 365.25
#define BIGTIME 1e150
#define NOTIME 1e15

using namespace ILWIS ;

ObjectTime::ObjectTime(const FileName& fn)
{ 
	iTime = 0;
	try {
		if (File::fExist(fn))
		{
			File fil(fn);
			iTime = fil.filetime();
		}
	}
	catch (const FileErrorObject&) 
	{  }
}

String ObjectTime::sDateTime() const
{
	char sTmp[80];
	tm* ptm;
	time_t t = (long)iTime;
	ptm = localtime(&t);
	if ( ptm == NULL)
		return "";
	strftime(sTmp, 79, "%#c", ptm);
	return String(sTmp);
}


String ObjectTime::sDate() const
{
	char sTmp[50];
	tm* ptm;
	time_t t = (long)iTime;
	ptm = localtime(&t);
	strftime(sTmp, 49, "%#x", ptm);
	return String(sTmp);
}

String ObjectTime::sTime() const
{
	char sTmp[50];
	tm* ptm;
	time_t t = (long)iTime;
	ptm = localtime(&t);
	if ( ptm == NULL)
		return "";
	strftime(sTmp, 49, "%X", ptm);
	return String(sTmp);
}

//----------------------------------

ILWIS::TimeParser::TimeParser()
: valid(false)
{
}

bool ILWIS::TimeParser::fValid() const
{
	return valid;
}

ILWIS::TimeParser::TimeParser(const String & pattern)
{
	int iIndex [6]; // the plain indices
	int iLastIndex [6]; // their end
	iIndex[0] = pattern.find("Y");
	iIndex[1] = pattern.find("M");
	iIndex[2] = pattern.find("D");
	iIndex[3] = pattern.find("h");
	iIndex[4] = pattern.find("m");
	iIndex[5] = pattern.find("s");
	iLastIndex[0] = pattern.rfind("Y");
	iLastIndex[1] = pattern.rfind("M");
	iLastIndex[2] = pattern.rfind("D");
	iLastIndex[3] = pattern.rfind("h");
	iLastIndex[4] = pattern.rfind("m");
	iLastIndex[5] = pattern.rfind("s");

	vector<std::pair<int, int>> sortedPositions;
	iTotal = 0;
	for(int i = 0; i < 6; ++i) {
		if (iIndex[i] >= 0) {
			sortedPositions.push_back(std::pair<int, int>(iIndex[i], i));
			++iTotal;
		}
	}
	
	std::sort(sortedPositions.begin(), sortedPositions.end());

	valid = (iTotal > 0);

	for (int i = 0; i < iTotal; ++i) {
		iPosition[i] = sortedPositions[i].second;
		if (i < iTotal - 1) {
			iOffset[i] = sortedPositions[i + 1].first - sortedPositions[i].first - 1;
			valid = valid && (iOffset[i] > 0); // strict in this version .. in the future we could accomodate for iOffset[i] == 0, e.g. YYMMDDhhmmss
		}
	}
}

void ILWIS::TimeParser::parse(const String& string, int& year, int& month, int& day, int& hours, int& minutes, double& seconds) const
{
	char *begin = const_cast<char*>(string.c_str());// const_cast<char*>(s);
	char *endptr;
	for (int i = 0; i < iTotal; ++i) {
		// skip leading spaces; strtol will return 0 on a string with only spaces which is not what we want
		//while (isspace(*begin)) ++begin;
		//if (strlen(begin) == 0)
		//	break;
		errno = 0;
		long l;
		double r;
		switch(iPosition[i]) {
			case 0:
				l = strtol(begin, &endptr, 10);
				if (errno == 0)
					year = l;
				break;
			case 1:
				l = strtol(begin, &endptr, 10);
				if (errno == 0)
					month = l;
				break;
			case 2:
				l = strtol(begin, &endptr, 10);
				if (errno == 0)
					day = l;
				break;
			case 3:
				l = strtol(begin, &endptr, 10);
				if (errno == 0)
					hours = l;
				break;
			case 4:
				l = strtol(begin, &endptr, 10);
				if (errno == 0)
					minutes = l;
				break;
			case 5:
				// r = strtodILW(begin, &endptr); // checked, does not work well, fails too much
				r = strtod(begin, &endptr);
				if (errno == 0)
					seconds = r;
				break;
		}
		begin = endptr; // first non-digit
		begin += iOffset[i]; // jump over the delimiter
	}
}

//----------------------------------

ILWIS::Time::Time(int yr, int mnth, int dy, int hr, int min, double sec)
{
	valid = true;
	julianday = rUNDEF;

	int year = yr;
	int month;
	if ( mnth >= 1 && mnth <= 12)
		month = mnth;
	else
		valid = false;

	int day;
	if ( dy >= 1 && dy <= 31 && valid) {
		if ( month == 2) {
			if ( dy <= 29 && isLeapYear(yr)) {
				day = dy;
			} else if( dy <= 28 && !isLeapYear(yr))
				day = dy;
			else
				valid = false;
		}else {
			if ( month < 8 && (month + 1) % 2 == 0 && dy <= 31)
				day = dy;
			else if ( month >= 8 && month % 2 == 0 && dy <= 31)
				day = dy;
			else if ( dy <= 30)
				day = dy;
			else
				valid = false;
		}
	} else
		valid = false;

	int hour;
	if ( hr >= 0 && hr <= 23 && valid) 
		hour = hr;
	else
		valid = false;

	int minutes;
	if ( min >= 0 && min <= 59 && valid)
		minutes = min;
	else
		valid = false;

	double seconds;
	if ( sec >= 0.0 && sec < 60.0  && valid) {
		seconds = sec;
	}
	else {
		valid = false;
	}
	if ( valid)
		julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
	else
		julianday = rUNDEF;
}
ILWIS::Time::Time(const time_t tmt){
	valid = true;
	julianday =rUNDEF;
	struct tm time;
	gmtime_s(&time, &tmt);
	int year = time.tm_year + 1900;
	int month = time.tm_mon;
	int day = time.tm_mday;
	int hour = time.tm_hour;
	int minutes = time.tm_min;
	int seconds = time.tm_sec;
	julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
}

ILWIS::Time::Time(const String& isostring, const TimeParser& parser) {
	valid = true;
	julianday = rUNDEF;
	setValue(isostring, parser);
}

ILWIS::Time::Time(const CTime& time){
	valid = true;
	int year = time.GetYear();
	int month = time.GetMonth();
	int day = time.GetDay();
	int hour = time.GetHour();
	int minutes = time.GetMinute();
	double seconds = time.GetSecond();
	julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);

}

ILWIS::Time::Time(double juliand) {
	julianday = juliand;
}

ILWIS::Time::operator CTime() const{
	if ( abs(julianday) > NOTIME)
		return CTime();

	int year, month, day, hour, minutes;
	double seconds;
	julianToGregorian(year,month,day,hour,minutes,seconds);
	return CTime(year,month, day,hour,minutes,seconds);
}
time_t ILWIS::Time::toTime_t() const{
	struct tm time;
	if ( abs(julianday) > NOTIME) {
		return (__int64) iUNDEF;
	}
	int year, month, day, hour, minutes;
	double seconds;
	julianToGregorian(year,month,day,hour,minutes,seconds);
	
	time.tm_year = year - 1900;
	time.tm_mon  = month ;
	time.tm_mday = day;
	time.tm_hour = hour;
	time.tm_min = minutes;
	time.tm_sec = round(seconds);
	return mktime(&time);
}

ILWIS::Time::operator double() const{
	return julianday;
}

Time& ILWIS::Time::operator=(double t) {
	julianday = t;
	return *this;
}

bool ILWIS::Time::isLeapYear(int year)  const{
	if ( year == iUNDEF && abs(julianday) > NOTIME)
		return false;

	int month, day, hour, minutes;
	double seconds;

	if ( year == iUNDEF) {
		julianToGregorian(year,month,day,hour,minutes,seconds);
	}
	if((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
		return true; /* leap */
	else
		return false; /* no leap */
}

double ILWIS::Time::get(TimePart part) const{
	if ( julianday == rUNDEF)
		return rUNDEF;

	if ( abs(julianday) > NOTIME)
		return rUNDEF;

	int year, month, day, hour, minutes;
	double seconds;
	julianToGregorian(year,month,day,hour,minutes,seconds);
	if ( part == tpDATE) {
		String date("%4d%02d%02d",year,month,day);
		return date.iVal();
	}
	if ( part == tpDAYTIME) {
		String date("%02d%02d%02d",hour,minutes,(int)seconds);
		return date.iVal();
	}
	if ( part == tpYEAR)
		return year;
	if ( part == tpMONTH)
		return month;
	if ( part == tpDAYOFMONTH)
		return day;
	if ( part == tpJULIANDAY) {
		int tempYear = year;
		year = isLeapYear() ? 1980 : 1981;
		time_t p = toTime_t();
		struct tm time;
		gmtime_s(&time, &p);
		year = tempYear;
		return time.tm_wday;
	}
	if ( part == tpDAYOFTHEWEEK) {
		int tempYear = year;
		year = isLeapYear() ? 1980 : 1981;
		time_t p = toTime_t();
		struct tm time;
		gmtime_s(&time, &p);
		year = tempYear;
		return time.tm_yday;
	}
	if ( part == tpHOUR)
		return hour;
	if ( part == tpMINUTE)
		return minutes;
	if ( part == tpSECOND)
		return seconds;
	return tUNDEF;
}

ILWIS::Time ILWIS::Time::operator-(const Time& time) const {
	if ( julianday == rUNDEF)
		return tUNDEF;
	if ( abs(julianday) > NOTIME)
		return julianday;
	double t1 = time;
	double t2 = *this;
	return Time(t2 - t1);
}

ILWIS::Time ILWIS::Time::operator+(const Time& time) const {
	if ( julianday == rUNDEF)
		return tUNDEF;

	if ( abs(julianday) > NOTIME)
		return julianday;
	double t1 = time;
	double t2 = *this;
	return Time(t2 + t1);
}

#define IGREG (14+31*(10+12*1582))
#define JGREG 2299160

int ILWIS::Time::tolong(double val) const{
	if ( abs(val) > NOTIME || val == rUNDEF || val == iUNDEF)
		return iUNDEF;

	if (val >= 0) {return floor(val);}
	else {return ceil(val);}
}
double ILWIS::Time::gregorianToJulian(int year, int month, int day, int hour, int minutes, double seconds) const {
	if ( year == iUNDEF || month == iUNDEF || day == iUNDEF || minutes == iUNDEF || seconds == rUNDEF)
		return rUNDEF;

	int orgyear = year;
	int orgmonth = month ; 
	if (year<0) ++year;
	if (month > 2) ++month;
	else { --year; month+=13;}
	double jul = tolong(floor(365.25*year)+ floor(30.6001*month)+day+1720995);

	if (day+31*(orgmonth+12*orgyear) >= IGREG) {
		int ja = tolong(0.01*year);
		jul += 2-ja+tolong(0.25*ja);
	}
	double fract = (double)hour/24.0 + (double)minutes/(24.0*60.0) + seconds / (24.0*60.0*60);
	return jul + fract;
}

void ILWIS::Time::julianToGregorian(int& year, int& month, int& day, int& hour, int& minutes, double& seconds) const{

	if ( julianday == rUNDEF) {
		year = month = day = minutes = iUNDEF;
		seconds = rUNDEF;
	}

	if ( abs(julianday) > NOTIME) {
		year = LONG_MAX; // doesnt realy matter, just an absurd large number
		month = day = 1;
		hour = minutes = seconds = 0;
		return;
	}
	year = -4713;
	month = day = 1;
	hour = minutes = seconds = 0;
	if ( julianday == rUNDEF)
		return;

	double ja;
	double julian = tolong(julianday);
	double fract = julianday - julian;
	if (julian > JGREG) {
		int jalpha = tolong(((julian-1867216)-0.25)/36524.25);
		ja=julian+1+jalpha-tolong(0.25*jalpha);
	} else { ja = julian; }
	int jb=ja+1524;
	int jc=tolong(6680+((jb-2439870)-122.1)/365.25);
	int jd=tolong(365*jc+0.25*jc);
	int je=tolong((jb-jd)/30.6001);
	day = jb-jd-tolong(30.6001*je);
	month = je-1;
	if (month >12) month -= 12;
	year = jc-4715;
	if (month > 2) --year;
	double hr = fract * 24.0;
	double delta =  hr - int(hr + 0.5);
	hour = abs(delta) < 1e-6 ? int(hr + 0.5) : hr;
	double min = (hr - hour) * 60.0;
	delta = min - int(min + 0.5);
	minutes = abs(delta)< 1e-6 ? int(min + 0.5) : min;
	seconds = (min - minutes) * 60.0;

}


void ILWIS::Time::setValue(const String& isostring, const TimeParser& parser) {
	if ( isostring == "?") {
		julianday = rUNDEF;
		return;
	}
	if ( isostring == "*" || isostring == "+*") {
		julianday = BIGTIME;
		return;
	}
	if ( isostring == "-*") {
		julianday = -BIGTIME;
		return;
	}
	/*accepted iso 8601 strings
	YYYY[-MM[-DD[Thh[:mm[:s[Z]]]]]] ,h,m,s may be fractions. internally they are formatted to a correct int representation
	YYYY[MM[DD[Thh[:mm[:s[Z]]]]]*/
	int year=-4713, month=1, day=1, hour=0, minutes=0;
	double seconds = 0;
	parseIsoString(isostring, parser, year, month, day, hour, minutes,seconds);

	if ( !valid )
		return;
	Time t(year,month,day,hour,minutes,seconds);
	julianday = t;
}

void ILWIS::Time::parseOrdinalDate(int ordinal, int year, int& month, int& day) {
	int count = 0;
	int m=1;
	int prevCount;
	for(; m <= 12 && count < ordinal; ++m) {
		if ( m % 2) {
			count += 31;	
		} else {
			count += m==2 && !isLeapYear(year) ? 28 : 29;
		}
		prevCount = count;
	}
	month = --m;
	day = ordinal - prevCount;
}

void ILWIS::Time::parseYearPart(const String& yearpart, int& year, int& month, int& day) {
	Array<String> parts;
	Split(yearpart,parts,"-");
	if ( parts.size() < 2) {
		year = yearpart.sSub(0,4).iVal();
		if ( yearpart.size() >4)
			month = yearpart.sSub(4,2).iVal();
		if ( yearpart.size() > 6)
			day = yearpart.sSub(6,2).iVal();
	} else {
		year = parts[0].iVal();
		if ( parts.size() > 1) {
			if (parts[1].size() == 3)
				parseOrdinalDate(parts[1].iVal(), year, month, day);
			month = parts[1].iVal();
		}
		if ( parts.size() > 2)
			day = parts[2].iVal();
	}
}

void ILWIS::Time::parseDayPart(const String& daypart, int& hours, int& minutes, double& seconds) {
	Array<String> parts;
	Split(daypart,parts,":");
	double hr=0, min=0, sec=0;
	if ( parts.size() < 2) {
		if ( daypart[2] == '.')
			hr = daypart.rVal();
		else{
			hr = daypart.sSub(0,2).rVal();
			if ( daypart.size() >= 3) {
				if ( daypart[2] == '.') {
					min = daypart.sSub(2, daypart.size() - 2).rVal();
				} else {
					min = daypart.sSub(2,2).rVal();
					if ( daypart.size() >= 5) {
						sec = daypart.sSub(4, daypart.size() - 3).rVal();
					}
				}
			}
		}
	
	} else {
		hr = parts[0].rVal();
		if ( parts.size() > 1)
			min = parts[1].rVal();
		if ( parts.size() > 2)
			sec = parts[2].rVal();
	}
	if ( hr == rUNDEF || min == rUNDEF || sec == rUNDEF) {
		valid = false;
		return;
	}
	if (( hr - (int)hr) > 0)
		min += (hr - (int)hr) * 60;
	else if (( min - (int)min) > 0) {
		sec += (min - (int)min) * 60;
	}
	hours = hr;
	minutes = min;
	seconds = sec;
}

void ILWIS::Time::parseIsoString(const String& isostring, const TimeParser& parser, int& year, int& month, int& day, int& hours, int& minutes, double& seconds) {
	year=-4713, month=1, day=1, hours=0, minutes=0;
	seconds = 0;
	if ( isostring == "?") {
		julianday = rUNDEF;
		return;
	}
	if ( isostring == "*" || isostring == "+*")
		julianday = BIGTIME;
	if ( isostring == "-*")
		julianday = -BIGTIME;
	// special form YYYYMMDDHHMM without the T marker for time, often used in sattelite orbit times
	if ( parser.fValid()) {
		parser.parse(isostring, year, month, day, hours, minutes, seconds);
	} else {
		if ( isostring.size() == 12 && isostring.find("T") == string::npos && 
			isostring.find("-") == string::npos && 
			isostring.find(":") == string::npos && 
			isostring.find(".") == string::npos) 
		{
			parseYearPart(isostring.sSub(0,8), year, month, day);
			parseDayPart(isostring.sSub(8,6),hours, minutes, seconds);
		} else{
			String yearpart = isostring.sHead("T");
			String daypart = isostring.sTail("T").sHead("Z");
			if ( yearpart != "")
				parseYearPart(yearpart,year, month,day);
			if ( daypart != "")
				parseDayPart(daypart, hours, minutes, seconds);
		}
	}
}

bool ILWIS::Time::operator <(const Time& time) const{
	if ( julianday == rUNDEF)
		return false;

	return julianday < (double)time;
}

bool ILWIS::Time::operator >(const Time& time) const{
	if ( julianday == rUNDEF)
		return false;

	return julianday > (double)time;
}

bool ILWIS::Time::operator ==(const Time& time) const{
	if ( julianday == rUNDEF)
		return false;

	return julianday == (double)time;
}

bool ILWIS::Time::operator !=(const Time& time) const{
	if ( julianday == rUNDEF)
		return false;

	return julianday != (double)time;
}

bool ILWIS::Time::isValid() const {
	if ( julianday == rUNDEF)
		return false;

	return valid; 
}

void ILWIS::Time::setYear(int yr) {
	if ( yr == iUNDEF) {
		julianday = rUNDEF;
		return;
	}
	int year, month, day, hour, minutes;
	double seconds;
	julianToGregorian(year,month,day,hour,minutes,seconds);
	year = yr;
	julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
}
void ILWIS::Time::setMonth(int mnth){
	if ( mnth == iUNDEF) {
		julianday = rUNDEF;
		return;
	}
	int year, month, day, hour, minutes;
	double seconds;
	julianToGregorian(year,month,day,hour,minutes,seconds);
	if ( mnth >= 1 && mnth <= 12)
		month = mnth;
	else
		valid = false;
	if ( valid)
		julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
}

void ILWIS::Time::setDay(int dy) {
	if ( dy == iUNDEF) {
		julianday = rUNDEF;
		return;
	}
	int year, month, day, hour, minutes;
	double seconds;
	julianToGregorian(year,month,day,hour,minutes,seconds);
	if ( dy >= 1 && dy <= 31) {
		if ( month == 2) {
			if ( dy <= 28 && isLeapYear()) {
				day = dy;
			} else if( dy <= 27 && !isLeapYear())
				day = dy;
			else
				valid = false;
		}else {
			if ( (month - 1) % 2 == 0 && dy <= 30)
				day = dy;
			else if ( dy <= 29)
				day = dy;
			else
				valid = false;
		}
	}
	if ( valid)
		julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
}

void ILWIS::Time::setHour(int hr) {
	if ( hr == iUNDEF) {
		julianday = rUNDEF;
		return;
	}
	int year, month, day, hour, minutes;
	double seconds;
	julianToGregorian(year,month,day,hour,minutes,seconds);
	if ( hr >= 0 && hr <= 23) 
		hour = hr;
	else
		valid = false;
	if ( valid)
		julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);

}

void ILWIS::Time::setMinute(int min) {
	if ( min == iUNDEF) {
		julianday = rUNDEF;
		return;
	}
	int year, month, day, hour, minutes;
	double seconds;
	julianToGregorian(year,month,day,hour,minutes,seconds);
	if ( min >= 0 && min <= 59)
		minutes = min;
	else
		valid = false;
	if ( valid)
		julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);

}

void ILWIS::Time::setSecond(double sec) {
	if ( sec == rUNDEF || sec == iUNDEF) {
		julianday = rUNDEF;
		return;
	}
	int year, month, day, hour, minutes;
	double seconds;
	julianToGregorian(year,month,day,hour,minutes,seconds);
	if ( sec >= 0.0 && sec < 60.0 ) {
		seconds = sec;
	}
	else {
		valid = false;
	}
	if ( valid)
		julianday = gregorianToJulian(year, month, day,hour,minutes,seconds);
}



String ILWIS::Time::toString(bool local, ILWIS::Time::Mode mode) const{
	if ( *this == tUNDEF || julianday == rUNDEF)
		return "?";
	if ( abs(julianday) > NOTIME && julianday > 0)
		return "+*";
	if ( abs(julianday) > NOTIME && julianday < 0)
		return "-*";

	int year, month, day, hour, minutes;
	double seconds;
	julianToGregorian(year,month,day,hour,minutes,seconds);
	if ( year == -4172 && mode != mTIME )
		return "?";
	struct tm time;
	time.tm_year = isLeapYear() ? 96 : 97;
	time.tm_mon  = month - 1 ; // 0 based
	time.tm_mday = day;
	time.tm_hour = hour;
	time.tm_min = minutes;
	time.tm_sec = round(seconds);
	time_t t = mktime(&time);
	if ( local) {
		struct tm timeinfo;
		localtime_s(&timeinfo, &t);
		if ( mode == mDATE)
				return String("%04d-%02d-%02d", year, timeinfo.tm_mon + 1, timeinfo.tm_mday);
		else if ( mode == mDATETIME)
			return String("%04d-%02d-%02dT%02d:%02d:%02d", year, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
		else if ( mode == mTIME)
			return String("T%02d:%02d:%02.02f", timeinfo.tm_hour, timeinfo.tm_min, seconds);
	} else {
		struct tm timeinfo;
		gmtime_s(&timeinfo, & t);
		if ( mode == mDATE)
			return String("%d-%d-%d", year, month, day);
		else if ( mode == mDATETIME)
			return String("%04d-%02d-%02dT%02d:%02d:%02d", year, month, day, hour, minutes, seconds);
		else if ( mode == mTIME)
			return String("T%02d:%02d:%02d", hour, minutes, seconds);
	}
	return "?";
}

Time ILWIS::Time::now() {
	CTime t = CTime::GetCurrentTime();
	return Time(t);
}
//-------------------------------------------
Duration::Duration(const String& duration) {
	String definition = duration;
	definition.toLower();
	map<String, double> values;
	values["y"] = -4713;
	values["m"] = 1.0;
	values["d"] = 1.0;
	values["h"] = 0.0;
	values["min"] = 0.0;
	values["s"] = 0.0;

	String v;
	double rest = 0;
	bool timePart = false;
	for(int i=0; i < definition.size(); ++i) {
		char c = definition[i];
		if ( (c >= '0' && c <= '9') || c == '.')
			v+=c;
		else if ( c == 'y' || c == 'd' || c == 'h' || c == 's') {
			double value = v.rVal();
			values[String(c)] += value;
			v = "";
		}
		else if ( c == 't') {
			timePart = true;
			v = "";
		} else if ( c == 'm' && timePart ) {
			values["min"] += v.rVal();
			v = "";
		}
		else if ( c == 'm') {
			values["m"] += v.rVal();
			v = "";
		}
	}
	int year, month, day, hour, minute;
	double seconds;
	year = values["y"];
	seconds = values["s"];
	minute = values["min"];
	hour = values["h"];
	day = values["d"]; 
	month = values["m"];

	if ( seconds >= 60) { 
		minute += ((int)seconds) % 60;
		seconds = seconds - minute * 60.0;
	}
	if ( minute >= 60) {
		hour += minute % 60;
		minute = minute - hour * 60;
	}
	if ( hour > 24) {
		day += hour % 24;
		hour = hour - day * 24;
	}
	if (  day > 365) {
		year += day / 365;
		day = day % 365;
	}
	if ( month > 12) {
		year += month % 12;
		month = month - year * 12;
	}
	julianday = gregorianToJulian(year, month, day, hour, minute, seconds);
	julianday += rest;
}

Duration::Duration(double r) : Time(r) {
	if ( r == rUNDEF || r == tUNDEF || r == 0)
		valid = false;
	valid = true;
}

String ILWIS::Duration::toString(bool local, ILWIS::Time::Mode mode) const{
	if ( *this == tUNDEF || julianday == rUNDEF)
		return "";
	int year, month, day, hour, minutes;
	double seconds;
	julianToGregorian(year,month,day,hour,minutes,seconds);
	// time says year, month, day have a minimum of 1 and go up to max but ofc for duration this is not true
	// duration goes from 0-max-1;
	year += 4712;
	month -= 1; 
	day -= 1;

	String dur = "P";
	if ( year != 0)
		dur += String("%dY", year);
	if ( month != 0)
		dur += String("%dM", month);
	if ( day != 0)
		dur+= String("%dD",  day);

	String dur2 = "T";
	if ( hour != 0)
		dur2 += String("%dH", hour);
	if ( minutes != 0)
		dur2 += String("%dM", minutes);
	if ( seconds !=0)
		dur2 += String("%fS", seconds);
	if ( dur2 != "T")
		dur += "T" + dur2;

	
	return dur == "P" ? "" : dur;

}

double ILWIS::Duration::get(TimePart part) const{
	if ( julianday == rUNDEF)
		return rUNDEF;

	if ( abs(julianday) > NOTIME)
		return rUNDEF;

	int year, month, day, hour, minutes;
	double seconds;
	julianToGregorian(year,month,day,hour,minutes,seconds);
	if ( month == 12) { // month 12 is a duration of 1 year, month 12 makes sense in Time's context, but not in duration;
		++year;
		month = 0;
	}
	if ( part == tpYEAR)
		return year + 4712;
	else if (part == tpMONTH)
		return month - 1; // in time months are 1 based, but for duration this doesnt make sense
	else if (part == tpDAYOFMONTH )
		return day - 1; // in time days are 1 based, but for duration this doesnt make sense
	else{
		return ILWIS::Time::get(part);
	}
	return tUNDEF;
}


bool ILWIS::Duration::isValid() const{
	return Time::isValid() && julianday != 0;
}


//-------------------------------------------
TimeInterval::TimeInterval() {
	current = tUNDEF;
	step = Duration(tUNDEF);
}

TimeInterval::TimeInterval(const Time& beg, const Time& en, const Duration& stp) :
RangeReal((double)beg, (double)en),
step(stp),
current(beg)
{
	if ( (double)beg == BIGTIME) {
		_rLo = -BIGTIME;
	}
}

TimeInterval TimeInterval::operator+(const TimeInterval& interval){
	if ( step == interval.step ) {
		return TimeInterval(Time(rLo() + interval.rLo()), Time(rHi() + interval.rHi(), step));
	}
	else if (  interval.getStep() == tUNDEF || step == tUNDEF) {
		TimeInterval(Time(rLo() + interval.rLo()), Time(rHi() + interval.rHi(), step == tUNDEF ? interval.getStep() : step));
	}
	return TimeInterval();
}
TimeInterval TimeInterval::operator-(const TimeInterval& interval){
		if ( step == interval.step ) {
		return TimeInterval(Time(rLo() - interval.rLo()), Time(rHi() - interval.rHi(), step));
	}
	else if (  interval.getStep() == tUNDEF || step == tUNDEF) {
		TimeInterval(Time(rLo() - interval.rLo()), Time(rHi() - interval.rHi(), step == tUNDEF ? interval.getStep() : step));
	}
	return TimeInterval();
}

Time TimeInterval::operator++(){

	if ( rLo() == rUNDEF || rHi() == rUNDEF || step == Duration())
		return tUNDEF;
	if ( current == tUNDEF)
		current = Time(rLo());
	current = current + step;
	if ( current > Time(rHi()))
		return tUNDEF;
	return current;

}

Time TimeInterval::operator--() {
	if ( rLo() == rUNDEF || rHi() == rUNDEF || step == Duration())
		return tUNDEF;
	if ( current == tUNDEF)
		current = Time(rHi());
	current = current  - step;
	if ( current < Time(rLo()))
		return tUNDEF;
	return current;
}

void TimeInterval::reset(bool atStart){
	current = Time(atStart ? rLo() : rHi());
}

String TimeInterval::toString(bool local, ILWIS::Time::Mode mode) {
	Time begin(rLo());
	Time end(rHi());
	String sb = begin.toString(local,mode);
	String se = end.toString(local,mode);
	return String("%S/%S",sb ,se );
}


TimeInterval& ILWIS::TimeInterval::operator=(const TimeInterval& tiv){
	this->rLo() = tiv.rLo();
	this->rHi() = tiv.rHi();
	step = tiv.getStep();

	return *this;
}












