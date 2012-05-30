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
// dt.h
// Date ObjectTime Interface for ILWIS 2.0
// 28 may 1993, by Wim Koolhoven
// (c) Computer Department ITC

// never rely on private parts, they will change in near future

#ifndef ILWDT_H
#define ILWDT_H
//#include "Engine\Base\File\FILENAME.H"

class Date;
class ObjectTime;

class ObjectTime
{
public:
  ObjectTime() : iTime(0) {}
  ObjectTime(long t) : iTime(t) {}
  __export ObjectTime(const FileName&); // use stat()
  String _export sDateTime() const;  
  String _export sDate() const;
  String _export sTime() const;
  operator long() const { return iTime; }
  static ObjectTime timCurr() {
    time_t t = time(0);
    // adjust for comparison with DOS file ObjectTime which has resolution of 2 seconds
    if (t & 1) 
      --t;
    return ObjectTime((long)t);

  } 
private:
  long iTime;	// seconds since 1 january 1970
};

namespace ILWIS {
	class _export TimeParser {
	public:
		TimeParser();
		TimeParser(const String & pattern);
		void parse(const String& string, int& year, int& month, int& day, int& hours, int& minutes, double& seconds) const;
		bool fValid() const;
	private:
		int iPosition[6];
		int iOffset[6];
		int iTotal;
		bool valid;
	};

	class _export Time {
	public:
		enum TimePart{tpYEAR, tpMONTH, tpDAYOFMONTH, tpJULIANDAY, tpDAYOFTHEWEEK, tpHOUR, tpMINUTE, tpSECOND, tpDATE, tpDAYTIME};
		enum Mode{mUNKNOWN, mDATE,mDATETIME,mTIME,mDURATION};
		Time(int yr=-4713, int mnth=1, int dy=1, int hr=0, int min=0, double sec=0);
		Time(const time_t time);
		Time(const CTime& time);
		Time(const String& isostring, const TimeParser& timeParser=TimeParser());
		Time(double secondssince0);
		operator CTime() const;
		operator double() const;
		Time& operator=(double t);
		time_t toTime_t() const;
		bool isLeapYear(int year = iUNDEF) const;
		virtual double get(TimePart part) const;
		Time operator-(const Time& time) const;
		Time operator+(const Time& time) const;
		bool operator <(const Time& time) const;
		bool operator >(const Time& time) const;
		bool operator ==(const Time& time) const;
		bool operator !=(const Time& time) const;
		void setValue(const String& isostring, const TimeParser& timeParser=TimeParser());
		void setYear(int year);
		void setMonth(int Mnt);
		void setDay(int dy);
		void setHour(int hr) ;
		void setMinute(int min);
		void setSecond(double sec);
		virtual String toString(bool local= true, ILWIS::Time::Mode mode=ILWIS::Time::mDATETIME) const;
		virtual bool isValid() const;
		static Time now();
		
	protected:
		double gregorianToJulian(int year, int month, int day, int hour, int minutes, double seconds) const;
		void julianToGregorian(int& year, int& month, int& day, int& hour, int& minutes, double& seconds) const;
		int julianyearday(int year, int month, int day) const;
		void monthday(int year, int jday, int& month, int& day) const;
		int tolong(double val) const;
		void parseOrdinalDate(int ordinal, int year, int& month, int& day);
		void parseYearPart(const String& yearpart, int& year, int& month, int& day) ;
		void parseDayPart(const String& dayPart, int& hours, int& minutes, double& seconds);
		void parseIsoString(const String& isostring, const TimeParser& timeParser, int& year, int& month, int& day, int& hours, int& minutes, double& seconds);

		double julianday;
		bool valid;
	};

	class _export Duration : public Time {
	public:
		Duration(const String& step="");
		Duration(double r);
		String toString(bool local, ILWIS::Time::Mode mode) const;
		bool isValid() const;
		double get(TimePart part) const;
	};

	class _export TimeInterval : public RangeReal {
	public:
		TimeInterval();
		TimeInterval(const Time& begin, const Time& end, const Duration& stp=Duration(""));
		TimeInterval operator+(const TimeInterval& interval);
		TimeInterval operator-(const TimeInterval& interval);
		Time operator++();
		Time operator--();
		TimeInterval& operator=(const TimeInterval& tiv);
		void reset(bool atStart=true);
		Time getBegin() const { return Time(rLo());}
		Time getEnd() const { return Time(rHi());}
		Duration getStep() const { return step;}
		String toString(bool local=true, ILWIS::Time::Mode mode=ILWIS::Time::mDATETIME);

	private:
		Time current;
		Duration step;
	};

}

#endif







