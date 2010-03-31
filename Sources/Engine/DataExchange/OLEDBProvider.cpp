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

#include "Engine\DataExchange\OLEDBProvider.h"
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\Color.h"
#include "Engine\Base\System\RegistrySettings.h"
#include <set>

map<String, ProviderCreate > mpDummy;
map<String, ProviderCreate > OLEDBProvider::mpCreateFuncs = mpDummy;

OLEDBProvider::OLEDBProvider( const String& sProvID) :
	sProviderID(sProvID)
{
}

OLEDBProvider *OLEDBProvider::create(const String& sProvID)
{
	// dummy should be at a generalized place
	OLEDBProvider::mpCreateFuncs["ODBC"] = ODBCProvider::create;

	CreateMap::iterator where = mpCreateFuncs.find(sProvID);
	if ( where != mpCreateFuncs.end() )
	{
		ProviderCreate func = (*where).second;
		return (func)(sProvID);
	}
	return NULL;
}

String OLEDBProvider::sSupportedFormatExtensions() const 
{
	return "";
}

String OLEDBProvider::sBuildConnectionString(const String& sFormat, const String& sInitialDir, const String& sUserID, const String& sPassword)  
{
	return "";
}

bool OLEDBProvider::fIsDataBaseFormat(const String& sFormat)
{
	return false;
}

//----------------------------------------------------

ODBCProvider::ODBCProvider( const String& sProvID) :
	OLEDBProvider(sProvID)
{
}

OLEDBProvider *ODBCProvider::create(const String& sProvID)
{

	if ( sProvID == "ODBC" )
		return new ODBCProvider(sProvID);

	return NULL;
}

String ODBCProvider::sSupportedFormatExtensions() const 
{

	/*HKEY hKey;
	char sValueName[100];
	unsigned char sData[100];
	DWORD iSize1=100, iSize2=100, iType;
	int iIndex = 0;
	String sExt;
	// enumrate trhough the keys in the registry to fund the extensions
	LONG iRet = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\ODBC\\ODBC.INI\\ODBC Data Sources", 0, KEY_READ, &hKey);
	if ( iRet == ERROR_SUCCESS )
	{
		iRet = RegEnumValue(hKey, iIndex, sValueName, &iSize1, NULL, &iType, sData, &iSize2);
		while(iRet == ERROR_SUCCESS || iRet == ERROR_MORE_DATA)
		{
			// the formats seem to have the format "name(*.ext)". The ext part is optional
			String sFormat(sData);
			int iWhere = sFormat.find("*.");
			if ( iWhere != string::npos )
			{
				int iEnd = sFormat.find(")", iWhere);
				sExt += sFormat.substr(iWhere + 1, iEnd - iWhere - 1 );
			}
			++iIndex;
			iRet = RegEnumValue(hKey, iIndex, sValueName, &iSize1, NULL, &iType, sData, &iSize2);
		}	
		RegCloseKey(hKey);		
		return sExt;
	}
	return "";	*/
	return ".mdb,.dbf,.xls"		;
		
}

bool ODBCProvider::fIsDataBaseFormat(const String& sFormat)
{
	String sFormatName = sRetrieveFormatName(sFormat);	
	if ( sFormatName == "Excel Files")
		return true;
	if ( FileName(sFormat).sExt == ".mdb")
		return true;

	return false;
}

String ODBCProvider::sBuildConnectionString(const String& sFormat, const String& sInitialDir, const String& sUserID, 
																						const String& sPassword)  
{
	String sFormatName = sRetrieveFormatName(sFormat);
	FileName fn(sFormat);
	String sKey("Software\\ODBC\\ODBC.INI\\%S", sFormatName);
	RegistrySettings reg(HKEY_CURRENT_USER, sKey);
	long iDriverID = reg.iValue("DriverId");
	FileName fnPath(fn.sPath().sLeft(fn.sPath().size()-1));
	String sDBQ =  fIsDataBaseFormat(sFormat) ? fn.sRelative() : fnPath.sRelative();

	if ( iDriverID != iUNDEF )
		return String("DSN=%S;DBQ=%S;DriverId=%d;UID=%S;PWD=%S", sFormatName, sDBQ, 
		                                                                    iDriverID, 
																																				sUserID, sPassword);
	else
		return String("Provider=MSDASQL;Data Source=%S;Initial Catalog=%S; UID=%S;PWD=%S", sFormatName, sInitialDir, 
		                                                                                   sUserID, sPassword);
}

String ODBCProvider::sRetrieveFormatName(const String& sFormat)
{
	FileName fnTable(sFormat);
	if ( fnTable.fExist()) //  for file based tables
	{
		HKEY hKey;
		char sValueName[100];
		unsigned char sData[100];
		DWORD iSize1 = 100, iSize2 = 100, iType;
		int iIndex = 0;
		String sExt;
		// enumrate trhough the keys in the registry to fund the extensions
		LONG iRet = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\ODBC\\ODBC.INI\\ODBC Data Sources", 0, KEY_READ, &hKey);
		if ( iRet == ERROR_SUCCESS )
		{
			iRet = RegEnumValue(hKey, iIndex, sValueName, &iSize1, NULL, &iType, sData, &iSize2);
			while(iRet == ERROR_SUCCESS || iRet == ERROR_MORE_DATA)
			{
				// the formats seem to have the format "name(*.ext)". The ext part is optional
				String sFormat(sData);
				size_t iWhere = sFormat.find("*.");
				if ( iWhere != string::npos )
				{
					size_t iEnd = sFormat.find(")", iWhere);
					sExt = sFormat.substr(iWhere + 1, iEnd - iWhere - 1 );
					if ( sExt == fnTable.sExt)
					{
						String sKey("Software\\ODBC\\ODBC.INI\\%s", sValueName);
						RegistrySettings reg(HKEY_CURRENT_USER, sKey);
						long iDriverID = reg.iValue("DriverId");	
						if ( iDriverID != iUNDEF )
						{
							RegCloseKey(hKey);
							return sValueName;	
						}							
					}						
				}
				iSize1 = iSize2 = 100;
				iRet = RegEnumValue(hKey, ++iIndex, sValueName, &iSize1, NULL, &iType, sData, &iSize2);
			}	
			RegCloseKey(hKey);
		}
	}		
	return "";
}
