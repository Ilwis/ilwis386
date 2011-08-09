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
#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"

#include "Engine\Base\File\ElementMap.h"
#include <afx.h>

//#pragma warning( disable : 4786 )


ElementMap::ElementMap()
{
}

ElementMap::~ElementMap()
{
}

String& ElementMap::operator()(const string& s1, const string& s2)
{ 
	return operator[](s1)[s2]; 
}

#pragma warning( disable : 4786 )

typedef map<string, String> msS;
typedef map<string, msS>::const_iterator CI;
typedef msS::const_iterator CImsS;

void ElementMap::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		for (CI it = begin(); it != end(); ++it) 
		{
			String sSection = it->first;
			String s("[%S]\r\n", sSection);
			ar.WriteString(s.c_str());
			const msS& ss = it->second;
			for (CImsS entry = ss.begin(); entry != ss.end(); ++entry) 
			{
				String sEntry = entry->first;
				String sValue = entry->second;
				if (sValue == "")
					continue;
				String s("%S=%S\r\n", sEntry, sValue);
				ar.WriteString(s.c_str());
				ar.Flush();
			}
			ar.WriteString("\r\n");
		}	
	}
	else
	{
		String sSection, sEntry, sValue;
		CString str;
		while (ar.ReadString(str)) 
		{
			if (str == "")
				continue;
			if (str[0] == '[') {
				int i = str.Find(']');
				sSection = str.Mid(1, i-1);
			}
			else {
				int i = str.Find('=');
				sEntry = str.Left(i);
				sValue = str.Mid(i+1);
				operator()(sSection, sEntry) = sValue;
			}
		}
	}
}

void ElementMap::SerializeSection(CArchive& ar, const string& sSect)
{
	string sSection = sSect;
	if (ar.IsStoring())
	{
		String s("[%S]\r\n", sSection);
		ar.WriteString(s.c_str());

		const msS& ss = operator[](sSection);
		for (CImsS entry = ss.begin(); entry != ss.end(); ++entry) 
		{
			String sEntry = entry->first;
			String sValue = entry->second;
			if (sValue == "")
				continue;
			String s("%S=%S\r\n", sEntry, sValue);
			ar.WriteString(s.c_str());
		}
		ar.WriteString("\r\n");
	}
	else
	{
		bool fFound = false;
		String sEntry, sValue;
		CString str;
		while (ar.ReadString(str)) 
		{
			if (str == "")
				if (fFound)
					return;
				else
					continue;
			if (str[0] == '[') {
				int i = str.Find(']');
				sSection = str.Mid(1, i-1);
				if (sSection == sSect)
					fFound = true;
			}
			else {
				int i = str.Find('=');
				sEntry = str.Left(i);
				sValue = str.Mid(i+1);
				operator()(sSection, sEntry) = sValue;
			}
		}
	}
}
