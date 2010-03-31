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
// Date Time Interface for ILWIS 2.0
// 28 may 1993, by Wim Koolhoven
// (c) Computer Department ITC

// never rely on private parts, they will change in near future

#ifndef ILWDT_H
#define ILWDT_H
//#include "Engine\Base\File\FILENAME.H"

class Date;
class Time;

class Time
{
public:
  Time() : iTime(0) {}
  Time(long t) : iTime(t) {}
  __export Time(const FileName&); // use stat()
  String _export sDateTime() const;  
  String _export sDate() const;
  String _export sTime() const;
  operator long() const { return iTime; }
  static Time timCurr() {
    time_t t = time(0);
    // adjust for comparison with DOS file time which has resolution of 2 seconds
    if (t & 1) 
      --t;
    return Time((long)t);

  } 
private:
  long iTime;	// seconds since 1 january 1970
};

#endif







