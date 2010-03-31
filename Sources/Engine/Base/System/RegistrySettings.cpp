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
#include "Headers\toolspch.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Base\DataObjects\Color.h"
#include <atlbase.h>
#include "Engine\Base\System\RegistrySettings.h"

const static String sILWIS_BASE_REGISTRY_PATH="Software\\ITC\\ILWIS\\3.1";

RegistrySettings::RegistrySettings() :
 hCurrentKey(0)
{}

RegistrySettings::RegistrySettings(const HKEY hPrimKey, const String& sKey) :
   hCurrentKey(0)
{
	SetCurrentKey(hPrimKey, sKey, false);
}

RegistrySettings::~RegistrySettings()
{
	if ( hCurrentKey )
  {
		RegCloseKey(hCurrentKey);
		hCurrentKey = 0;
	}
}

RegistrySettings::RegistrySettings(const RegistrySettings& reg)
{
	SetCurrentKey(reg.hPrimaryKey, reg.sPath, false);
}

//---[ sIlwisSubKey ]-------------------------------------------------------------------------------------------
// Purpose : Get the leaf subkey of the current key.
//           Example: if the key is "HKCU\Software\ITC\Ilwis\3.0\DomainSortView" the function will
//                    return "DomainSortView". 
// Parameters : <none>
// returns    : the leaf subkey 
//---------------------------------------------------------------------------------------------------------------
String RegistrySettings::sIlwisSubKey()
{
	String sKey;
	int iLast = sPath.find_last_of("\\");
	if (iLast != string::npos)
		sKey = sPath.substr(iLast + 1);

	return sKey;
}

//---[ sSoftwareSubKey ]-------------------------------------------------------------------------------------------
// Purpose : Get the partial name of the current key, relative to the HKCU\Software or HKLM\Software
//           Example: if the key is "HKCU\Software\ITC\Ilwis\3.0\DomainClassEditor" the function will
//                    return "ITC\Ilwis\3.0\DomainClassEditor". If the current key is not a subkey of
//                    HKCU\Software or HKLM\Software an empty string is returned.
// Parameters : <none>
// returns    : the stripped current key 
//---------------------------------------------------------------------------------------------------------------
String RegistrySettings::sSoftwareSubKey()
{
	String sKey;
	if (fCIStrEqual(sPath.sHead("\\"), "Software"))
		sKey = sPath.sTail("\\");

	return sKey;
}

//---[ sCurrentKey ]-------------------------------------------------------------------------------------------
// Purpose : Get the full name of the current key
// Parameters : <none>
// returns    : the current key
//---------------------------------------------------------------------------------------------------------------
String RegistrySettings::sCurrentKey()
{
	String sKey;
	if (hPrimaryKey == HKEY_CURRENT_USER)
		sKey = String("HKEY_CURRENT_USER\\%S", sPath);
	else if (hPrimaryKey == HKEY_LOCAL_MACHINE)
		sKey = String("HKEY_LOCAL_MACHINE\\%S", sPath);

	return sKey;
}

//---[ fKeyExists ]-------------------------------------------------------------------------------------------
// Purpose : checks if a key exists
// Parameters : hPrimKey ; one of the primary keys of the registry
//              sSubKey  ; path to the subkey from the primary key used
// returns    : true if it exists
//---------------------------------------------------------------------------------------------------------------
bool RegistrySettings::fKeyExists(const HKEY hPrimKey, const String& sSubKey) const
{
	HKEY hKey;

	DWORD iRet = RegOpenKeyEx(hPrimKey, sSubKey.c_str(), 0, KEY_READ, &hKey);
	// needs to be more precise but for the moment this works
	if ( iRet != ERROR_SUCCESS)
		return false;
	
	RegCloseKey(hKey);
	return true;
}


//---[ SetCurrentKey ]-------------------------------------------------------------------------------------------
// Purpose : Switches the current open key to a new open key
// Parameters : hPrimKey ; one of the primary keys of the registry
//              sSubKey  ; path to the subkey from the primary key used
//              fCreate  ; create the key if it does not exist
//              fWritingRequired ; true when we want to open the key with read/write access
//              fCreateReadWrite ; true when we intend to create a key that is "public writable"
//																 fCreateReadWrite is not used when fCreate == false
// returns    : Error code of the CreateEx function
//---------------------------------------------------------------------------------------------------------------
long RegistrySettings::SetCurrentKey(const HKEY hPrimKey, const String& sSubKey, bool fCreate, bool fWritingRequired, bool fCreateReadWrite)
{
	if ( hCurrentKey != 0 )
  {
		RegCloseKey(hCurrentKey);
		hCurrentKey = 0;
  }
	DWORD iDisposition;
	long iRet;
	if ( fCreate )
	{
		SECURITY_DESCRIPTOR lSecurityDescriptor;
		SECURITY_ATTRIBUTES lSecurityAttributes;
		if (fCreateReadWrite)
		{
			InitializeSecurityDescriptor(&lSecurityDescriptor,SECURITY_DESCRIPTOR_REVISION);
			SetSecurityDescriptorDacl(&lSecurityDescriptor,TRUE,NULL,FALSE);
			lSecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
			lSecurityAttributes.lpSecurityDescriptor = &lSecurityDescriptor;
			lSecurityAttributes.bInheritHandle = false;
		}
		// lSecurityAttributes is only used if fCreateReadWrite is true, in order to create public
		// writable keys (e.g. by admin in local machine for Digitizer->MapReference)
		iRet = RegCreateKeyEx(hPrimKey, sSubKey.c_str(), NULL, 0, NULL, fWritingRequired?KEY_ALL_ACCESS:KEY_READ, fCreateReadWrite?&lSecurityAttributes:NULL, &hCurrentKey, &iDisposition);
	}
	else
		iRet = RegOpenKeyEx(hPrimKey, sSubKey.c_str(), 0, fWritingRequired?KEY_ALL_ACCESS:KEY_READ, &hCurrentKey);

	if ( iRet != ERROR_SUCCESS )
  {
		hCurrentKey = 0;
		if ( fCreate )
			throw RegistryError(iRet);
	}
	hPrimaryKey = hPrimKey;
	sPath = sSubKey;
	return iRet;
}

//---[ fWritingAllowed ]-------------------------------------------------------------------------------------------
// Purpose : Checks if the Windows NT user has write access to the given key
// returns    : true if writing is allowed, otherwise false
//---------------------------------------------------------------------------------------------------------------
bool RegistrySettings::fWritingAllowed(const HKEY hPrimKey, const String& sSubKey)
{
	HKEY hCurrentKey;
	bool fRet = (ERROR_SUCCESS == RegOpenKeyEx(hPrimKey, sSubKey.c_str(), 0, KEY_ALL_ACCESS, &hCurrentKey));
	if (fRet)
		RegCloseKey(hCurrentKey);
	return fRet;
}

//---[ fValid ]-------------------------------------------------------------------------------------------
// Purpose : Checks is there is a currently open key
// returns    : valid state of the object
//---------------------------------------------------------------------------------------------------------------
bool RegistrySettings::fValid() const
{
	return hCurrentKey != 0;
}

//---[ iValue ]-------------------------------------------------------------------------------------------
// Purpose : returns the long value of a value in the subkey
// Parameters : sField ; Value name of the value
//              sDefault  ; Default value that should be used if RegQueryValueEx fails
// returns    : long value
//---------------------------------------------------------------------------------------------------------------
long RegistrySettings::iValue(const String& sField, const long iDefault) 
{
	if ( !fValid() )  
		if ( SetCurrentKey( hPrimaryKey, sPath, false ) != ERROR_SUCCESS )
			return iDefault;

	long iReturn;

	unsigned long iValueType = 0;
	unsigned long iValueSize = sizeof(long);
	long iRetValue = iDefault;
	iReturn = RegQueryValueEx(hCurrentKey, sField.c_str(), NULL, &iValueType, (PUCHAR) &iRetValue, &iValueSize);

	if ( iReturn != ERROR_SUCCESS )
		iRetValue = iDefault;

	return iRetValue; 	
}

//---[ sValue ]-------------------------------------------------------------------------------------------
// Purpose : returns the String value of a value in the subkey
// Parameters : sField ; Value name of the value
//              sDefault  ; Default value that should be used if RegQueryValueEx fails
// returns    : String value
//---------------------------------------------------------------------------------------------------------------
String RegistrySettings::sValue(const String& sField, const String& sDefault) 
{
	if ( !fValid() )  // if this is true something realy has gone wrong
		if ( SetCurrentKey( hPrimaryKey, sPath, false ) != ERROR_SUCCESS)
			return sDefault;

	long iReturn;

	unsigned long iValueType = 0;
	unsigned long iValueSize = 1000;
	char sRetValue[1000]; // is there a function to retrieve the length of a value ?
	memset(sRetValue, 0, 1000);
	iReturn = RegQueryValueEx(hCurrentKey, sField.c_str(), NULL, &iValueType, (PUCHAR) sRetValue, &iValueSize);
  String sRet(sRetValue); 

	if ( iReturn != ERROR_SUCCESS )
		sRet = sDefault;
	return sRet; 	
}
//---[ fValue ]-------------------------------------------------------------------------------------------
// Purpose : returns the boolean value of a value in the subkey
// Parameters : sField ; Value name of the value
//              sDefault  ; Default value that should be used if RegQueryValueEx fails
// returns    : bool value
//---------------------------------------------------------------------------------------------------------------
bool RegistrySettings::fValue(const String& sField, const bool fDefault) 
{
	if ( !fValid() )  // if this is true something realy has gone wrong
  {
		if ( SetCurrentKey( hPrimaryKey, sPath, false ) != ERROR_SUCCESS)
			return fDefault;
	}
	String sV = sValue(sField);

	return sV == sUNDEF ? fDefault : ( sV == "true" ? true : false );
}

void RegistrySettings::SetValue(const String& sField, bool fValue)
{
	if ( !fValid() )  // if this is true something realy has gone wrong
  {
		if ( SetCurrentKey( hPrimaryKey, sPath, false ) != ERROR_SUCCESS)
		return;
	}

	SetValue(sField, fValue ? String("true") : String("false"));
}


//---[ iValue ]-------------------------------------------------------------------------------------------
// Purpose : returns the double value of a value in the subkey
// Parameters : sField ; Value name of the value
//              sDefault  ; Default value that should be used if RegQueryValueEx fails
// returns    : double value
//---------------------------------------------------------------------------------------------------------------
double RegistrySettings::rValue(const String& sField, const double rDefault) 
{
	if ( !fValid() )  
  {
		if ( SetCurrentKey( hPrimaryKey, sPath, false ) != ERROR_SUCCESS)
			return rDefault;
	}
	String sV = sValue(sField, "!@@$#%Dummy");
	if (sV == "!@@$#%Dummy")
		return rDefault; // Hack to return exactly the same rDefault that came in
	return String(sV).rVal();
}

//---[ SetValue ]-------------------------------------------------------------------------------------------
// Purpose : sets the long value of a value in the subkey
// Parameters : sValueName ; Value name of the value
//              iVal  ; Value that belongs to ValueName
// returns    : -
//---------------------------------------------------------------------------------------------------------------
void RegistrySettings::SetValue(const String& sValueName, const long iVal)
{
	if ( !fValid() )  
  {
		if ( SetCurrentKey(hPrimaryKey, sPath)  != ERROR_SUCCESS )
		 return ;
	}

	DWORD iValType = REG_DWORD;
	DWORD iDataSize = sizeof(long);
	long iRet = RegSetValueEx(hCurrentKey, sValueName.c_str(), 0, iValType, (PUCHAR) &iVal, iDataSize);
}

//void RegistrySettings::SetValue(const String& sValueName, const int iVal)
//{
//	SetValue(sValueName, (const long)iVal);
//}

//---[ SetValue ]-------------------------------------------------------------------------------------------
// Purpose : sets the double value of a value in the subkey, doubles will be stored as strings
// Parameters : sValueName ; Value name of the value
//              rVal  ; Value that belongs to ValueName
// returns    : -
//---------------------------------------------------------------------------------------------------------------
void RegistrySettings::SetValue(const String& sValueName, const double rVal)
{
	if ( !fValid() )  
  {
		if ( SetCurrentKey(hPrimaryKey, sPath)  != ERROR_SUCCESS )
			return;
	}

	String sVal("%f", rVal);
  SetValue(sValueName, sVal);
}

//---[ SetValue ]-------------------------------------------------------------------------------------------
// Purpose : sets the String value of a value in the subkey
// Parameters : sValueName ; Value name of the value
//              sVal  ; Value that belongs to ValueName
// returns    : -
//----------------------------------------------------------------------------------------------------------
void RegistrySettings::SetValue(const String& sValueName, const String& sVal)
{
	if ( !fValid() )  
  {
		if ( SetCurrentKey(hPrimaryKey, sPath)  != ERROR_SUCCESS )
			return;
	}

	DWORD iValType = REG_SZ;
	DWORD iDataSize = sVal.size();
	long iRet = RegSetValueEx(hCurrentKey, sValueName.c_str(), 0, iValType, (PUCHAR) sVal.c_str(), iDataSize);
}

//---[ DeleteKey ]-------------------------------------------------------------------------------------------
// Purpose : Deletes the current open key and all its subkeys
// Parameters : fRecursive ; deletes also all subkeys of the current open key. default is true. If false tha
//              a delete will fail if there are subkeys.
// returns    : -
//---------------------------------------------------------------------------------------------------------------
void RegistrySettings::DeleteKey( bool fRecursive)
{
	if ( !fValid() )  
  {
		ISTRUE(fIEqual, hCurrentKey, (HKEY) 0);
		return ;
	}
	CRegKey regKey;
	regKey.Attach(hCurrentKey);
	if ( fRecursive )
		regKey.RecurseDeleteKey("");
	else
		regKey.DeleteSubKey("");

	regKey.Detach();
}

//---[ DeleteValue ]-------------------------------------------------------------------------------------------
// Purpose : Deletes a value from the current open key.
// Parameters : sValueName ; name of the value to be deleted
// returns    : -
//------------------------------------------------------------------------------------------------------------
void RegistrySettings::DeleteValue(const String& sValueName)
{
	if ( !fValid() ) 
  {
		if ( SetCurrentKey(hPrimaryKey, sPath, false)  != ERROR_SUCCESS )
			return;
	}
	CRegKey regKey;
	regKey.Attach(hCurrentKey);
	regKey.DeleteValue(sValueName.c_str());
	regKey.Detach();
}

//-------------------------------------------------------------------------------------------------------------

IlwisSettings::IlwisSettings(const String& sSubKey, PrimaryKeyUsed keyUsed, bool fCreate, OpenMode om, AccessRequired ar)
{
	hPrimaryKey = keyUsed == IlwisSettings::pkuUSER ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE;
	if ( sSubKey != "" )
		sPath = String("%S\\%S", sILWIS_BASE_REGISTRY_PATH, sSubKey);
	else
		sPath = String("%S", sILWIS_BASE_REGISTRY_PATH);
	SetCurrentKey(hPrimaryKey, sPath, fCreate, om == IlwisSettings::omREADWRITE, ar == IlwisSettings::arALLUSERS);
}

bool IlwisSettings::fWritingAllowed(const String& sSubKey, PrimaryKeyUsed keyUsed)
{
	HKEY hPrimaryKeyLocal = keyUsed == IlwisSettings::pkuUSER ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE;
	String sPathLocal;
	if ( sSubKey != "" )
		sPathLocal = String("%S\\%S", sILWIS_BASE_REGISTRY_PATH, sSubKey);
	else
		sPathLocal = String("%S", sILWIS_BASE_REGISTRY_PATH);
	return RegistrySettings::fWritingAllowed(hPrimaryKeyLocal, sPathLocal);
}

IlwisSettings::IlwisSettings()
{}

bool IlwisSettings::fKeyExists(const String& sSubKey) const
{
	String sSubK;
	sSubK =  sSubKey!="" ? String("%S\\%S", sPath, sSubKey) : sPath;
	return RegistrySettings::fKeyExists(hPrimaryKey, sSubK);
}

bool IlwisSettings::fDeletePossible()
{
	IlwisSettings settings("DefaultSettings\\SecurityTest");
	settings.SetValue("val", true);
	settings.DeleteKey();
	if ( settings.fKeyExists("val"))
		return false;
	return true;
}

MinMax IlwisSettings::mmValue(const String& sValueName, const MinMax& mmDefault) 
{
	MinMax mmRetMinMax;

	String sMinMax = sValue(sValueName);
	if ( sMinMax == sUNDEF)
		mmRetMinMax = mmDefault;
	else
  {
		RowCol rcMin( sMinMax.sTail(",").sHead(","). iVal(), sMinMax.sHead(",").iVal());
		String sRest = sMinMax.sTail(",").sTail(",");
		RowCol rcMax( sRest.sTail(",").sHead(","). iVal(), sRest.sHead(",").iVal());
		mmRetMinMax = MinMax( rcMin, rcMax);
	}

	return mmRetMinMax;

}

void IlwisSettings::SetValue(const String& sValueName, const MinMax& mmValue)
{
	String sMinRow("%d", mmValue.MinRow());
	String sMinCol("%d", mmValue.MinCol());
	String sMaxRow("%d", mmValue.MaxRow());
	String sMaxCol("%d", mmValue.MaxCol());
	String sMinMax("%S,%S,%S,%S", sMinCol, sMinRow, sMaxCol, sMaxRow);

	SetValue(sValueName, sMinMax);
}

Coord IlwisSettings::crdValue(const String& sValueName, const Coord& crdDefault) 
{
	String sCVal = sValue(sValueName);

  return sCVal == sUNDEF ? crdDefault : Coord(sCVal.sHead(",").rVal(), sCVal.sTail(",").rVal());
}

void IlwisSettings::SetValue(const String& sValueName, const Coord& crdValue)
{
	String sV("%f,%f", crdValue.x, crdValue.y);
	SetValue(sValueName, sV);
}

RowCol IlwisSettings::rcValue(const String& sValueName, const RowCol& rcDefault) 
{
	String sRCVal = sValue(sValueName);

  return sRCVal == sUNDEF ? rcDefault : RowCol(sRCVal.sHead(",").iVal(), sRCVal.sTail(",").iVal());
}


void IlwisSettings::SetValue(const String& sValueName, const RowCol& rcValue)
{
	String sV("%d,%d", rcValue.Row, rcValue.Col);
	SetValue(sValueName, sV);
}

Color IlwisSettings::clrGetRGBColor(const String& sValueName, const Color& clrDefault) 
{
	int iVal = iValue(sValueName);
	if (iVal == iUNDEF)
		return clrDefault;
	return (Color) iVal;
}

void IlwisSettings::SetValue(const String& sValueName, const Color& clrValue)
{
	SetValue(sValueName, (long)clrValue);
}

CPoint IlwisSettings::pntGetPoint(const String& sValueName, const CPoint& pntDefault) 
{
	String sVal = sValue(sValueName);

	return sVal == sUNDEF ? pntDefault : CPoint(sVal.sHead(",").iVal(), sVal.sTail(",").iVal());
}

void IlwisSettings::SetValue(const String& sValueName, const CPoint& pntValue)
{
	RegistrySettings::SetValue(sValueName, String("%d,%d", pntValue.x, pntValue.y));
}

CRect IlwisSettings::rctValue(const String& sValueName, const CRect& rctDefault ) 
{
	Array<String> iParts;

	String sV = sValue(sValueName);
	if ( sV == sUNDEF ) return rctDefault;
  
	Split(sV, iParts, ",");


	int iL = iParts[0].iVal();
  int iT = iParts[1].iVal();
	int iR = iParts[2].iVal();
	int iB = iParts[3].iVal();

	return CRect(iParts[0].iVal(), iParts[1].iVal(), iParts[2].iVal(), iParts[3].iVal());
}

void IlwisSettings::SetValue(const String& sValueName, const CRect& rctV)
{
	SetValue(sValueName, String("%d,%d,%d,%d", rctV.left, rctV.top, rctV.right, rctV.bottom));
}

WINDOWPLACEMENT IlwisSettings::GetWindowPlacement(const String& sValueName, WINDOWPLACEMENT& wplDefault) 
{
	String sWpl("%S\\%S", sPath, sValueName);

	IlwisSettings reg;

	if ( reg.SetCurrentKey(hPrimaryKey, sWpl, false) != ERROR_SUCCESS) 
		return wplDefault;

	WINDOWPLACEMENT wpl;

	wpl.flags = reg.iValue("Flags");
	wpl.showCmd = reg.iValue("ShowState");
	wpl.ptMinPosition = reg.pntGetPoint("MinimizedLTCorner");
	wpl.ptMaxPosition= reg.pntGetPoint("MaximizedLTCorner");
	wpl.rcNormalPosition = reg.rctValue("NormalizedPosition");
	wpl.length = sizeof(WINDOWPLACEMENT);

	return wpl;
}

void IlwisSettings::SetWindowPlacement(const String& sValueName, const WINDOWPLACEMENT wplValue)
{
	String sWpl("%S\\%S", sPath, sValueName);
	IlwisSettings reg;

	reg.SetCurrentKey(hPrimaryKey, sWpl);

	reg.SetValue("Flags", (long)wplValue.flags); 
	reg.SetValue("ShowState", (long)wplValue.showCmd);
	reg.SetValue("MinimizedLTCorner", CPoint(wplValue.ptMinPosition));
	reg.SetValue("MaximizedLTCorner", CPoint(wplValue.ptMaxPosition));
	reg.SetValue("NormalizedPosition", CRect(wplValue.rcNormalPosition));
}

void IlwisSettings::GetFont(const String& sValueName, CFont *fnt) const
{
	String sWpl("%S\\%S", sPath, sValueName);
	RegistrySettings reg;
	if ( reg.SetCurrentKey(hPrimaryKey, sWpl, false) != ERROR_SUCCESS )
		return;

	LOGFONT lfFont;

	lfFont.lfHeight = -reg.iValue("Height");
	lfFont.lfWidth = reg.iValue("Width");
	lfFont.lfEscapement = reg.iValue("Escapement");
	lfFont.lfOrientation = reg.iValue("Orientation");
	lfFont.lfWeight = reg.iValue("Weight");
	lfFont.lfItalic = (BYTE)reg.iValue("Italic");
	lfFont.lfUnderline = (BYTE)reg.iValue("Underline");
	lfFont.lfStrikeOut =  (BYTE)reg.iValue("StrikeOut");
	lfFont.lfCharSet = (BYTE)reg.iValue("CharSet");
  lfFont.lfOutPrecision = (BYTE)reg.iValue("OutPrecision");
	lfFont.lfClipPrecision = (BYTE)reg.iValue("ClipPrecision");
	lfFont.lfQuality = (BYTE)reg.iValue("Quality"); 
  lfFont.lfPitchAndFamily = (BYTE)reg.iValue("PitchAndFamily");
	strcpy(lfFont.lfFaceName, reg.sValue("FaceName").scVal());

	fnt->CreateFontIndirect(&lfFont);

}

void IlwisSettings::SetFont(const String& sValueName, CFont *fnt)
{
	String sWpl("%S\\%S", sPath, sValueName);
	RegistrySettings reg;
	reg.SetCurrentKey(hPrimaryKey, sWpl);
	LOGFONT lfFont;
  
	fnt->GetLogFont(&lfFont);

	reg.SetValue("Height", abs(lfFont.lfHeight));
	reg.SetValue("Width", lfFont.lfWidth);
	reg.SetValue("Escapement", lfFont.lfEscapement);
	reg.SetValue("Orientation", lfFont.lfOrientation);
	reg.SetValue("Underline", (long)lfFont.lfUnderline);
	reg.SetValue("Weight", (long)lfFont.lfWeight);
	reg.SetValue("Italic", (long)lfFont.lfItalic );
	reg.SetValue("StrikeOut", (long)lfFont.lfStrikeOut);
	reg.SetValue("CharSet", (long)lfFont.lfCharSet);
	reg.SetValue("OutPrecision", (long)lfFont.lfOutPrecision);
	reg.SetValue("ClipPrecision", (long)lfFont.lfClipPrecision);
	reg.SetValue("Quality", (long)lfFont.lfQuality);
	reg.SetValue("PitchAndFamily", (long)lfFont.lfPitchAndFamily);
	reg.SetValue("FaceName", String(lfFont.lfFaceName));
}

inline void IlwisSettings::SetValue(const String& sSubKey, const long iVal)
{
	RegistrySettings::SetValue(sSubKey, iVal);
}

inline void IlwisSettings::SetValue(const String& sSubKey, const int iVal)
{
	RegistrySettings::SetValue(sSubKey, (long)iVal);
}

inline void IlwisSettings::SetValue(const String& sSubKey, const double rVal)
{
	RegistrySettings::SetValue(sSubKey,rVal);
}

inline void IlwisSettings::SetValue(const String& sSubKey, const String& sVal)
{
	RegistrySettings::SetValue(sSubKey, sVal);
}

inline void IlwisSettings::SetValue(const String& sSubKey, const bool fVal)
{
	RegistrySettings::SetValue(sSubKey, fVal);
}
