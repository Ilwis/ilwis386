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
// Implementation of Date Time Interface for ILWIS 2.0
// 28 may 1993, by Wim Koolhoven
// (c) Computer Department ITC

#include "Headers\toolspch.h"

Time::Time(const FileName& fn)
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

String Time::sDateTime() const
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

  
String Time::sDate() const
{
  char sTmp[50];
  tm* ptm;
  time_t t = (long)iTime;
  ptm = localtime(&t);
  strftime(sTmp, 49, "%#x", ptm);
  return String(sTmp);
}

String Time::sTime() const
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








