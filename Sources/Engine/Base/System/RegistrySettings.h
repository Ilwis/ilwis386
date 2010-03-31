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
#ifndef REGISTRYSETTINGS_H
#define REGISTRYSETTINGS_H

class RegistrySettings
{
	friend class RegistrySettings;

public:
  RegistrySettings(); 
	_export RegistrySettings(const HKEY primKey, const String& sKey);
	_export RegistrySettings(const RegistrySettings& reg);
	_export ~RegistrySettings();

	String  _export sCurrentKey();
	String  _export sSoftwareSubKey();
	String  _export sIlwisSubKey();

	long    _export SetCurrentKey(const HKEY primKey, const String& sKey, bool fCreate = true, bool fWritingRequired = true, bool fCreateReadWrite = false);
	void    _export DeleteKey(bool fRecursive=true);
	void    _export DeleteValue(const String& sValueName);

	long    _export iValue(const String& sSubKey, const long iDefault = iUNDEF);
	String  _export sValue(const String& sSubkey, const String& sDefault = sUNDEF);
	double  _export rValue(const String& sSubKey, const double rDefault = rUNDEF);
	bool    _export fValue(const String& sSubKey, const bool fDefault = true);

	void    _export SetValue(const String& sSubKey, const long iVal);
	//void       _export SetValue(const String& sSubKey, const int iVal);
	void    _export SetValue(const String& sSubKey, const String& sVal);
	void    _export SetValue(const String& sSubKey, const double rVal);
	void    _export SetValue(const String& sSubKey, const bool fVal);

	static bool fWritingAllowed(const HKEY primKey, const String& sKey);

protected:
	bool       fKeyExists(const HKEY hPrimKey, const String& sSubKey) const;

	String     sPath;
	HKEY       hPrimaryKey;

private:
	bool       fValid() const;
	HKEY       hCurrentKey;
};

class _export IlwisSettings : public RegistrySettings
{
public:
	enum PrimaryKeyUsed{pkuUSER, pkuMACHINE};
	enum OpenMode{omREADONLY, omREADWRITE};
	enum AccessRequired{arCURRENTUSER, arALLUSERS};
	IlwisSettings(const String& sSubKey, PrimaryKeyUsed keyUsed = pkuUSER, bool fCreate = false, OpenMode om = omREADWRITE, AccessRequired = arCURRENTUSER);
	bool fKeyExists(const String& sSubKey = "") const;

	void SetValue(const String& sSubKey, const long iVal);
	void SetValue(const String& sSubKey, const int iVal);
	void SetValue(const String& sSubKey, const String& sVal);
	void SetValue(const String& sSubKey, const double rVal);
	void SetValue(const String& sSubKey, const bool fVal);

	MinMax mmValue(const String& sValueName, const MinMax& mmDefault = MinMax());
	void SetValue(const String& sValueName, const MinMax& mmValue);
	Coord crdValue(const String& sValueName, const Coord& crdDefault = Coord());
	void SetValue(const String& sValueName, const Coord& crdValue);
	RowCol rcValue(const String& sValueName, const RowCol& rcDefault = RowCol());
	void SetValue(const String& sValueName, const RowCol& rcValue);
	Color clrGetRGBColor(const String& sValueName, const Color& clrDefault = Color());
	Color clrValue(const String& sValueName, const Color& clrDefault = Color()) 
		{ return clrGetRGBColor(sValueName, clrDefault); }
	void SetValue(const String& sValueName, const Color& clrValue);
	CPoint pntGetPoint(const String& sValueName, const CPoint& pntDefault = CPoint());
	CPoint pntValue(const String& sValueName, const CPoint& pntDefault = CPoint())
		{ return pntGetPoint(sValueName, pntDefault); }
	void SetValue(const String& sValueName, const CPoint& pntValue);
	CRect rctValue(const String& sValueName, const CRect& rctDefault = CRect());
	void SetValue(const String& sValueName, const CRect& rctV);
	WINDOWPLACEMENT GetWindowPlacement(const String& sValueName, WINDOWPLACEMENT& wplDefault);
	void SetWindowPlacement(const String& sValueName, const WINDOWPLACEMENT wplValue);
	void GetFont(const String& sValueName, CFont* fnt) const;
	void SetFont(const String& sValueName, CFont* fnt);

	static bool fDeletePossible();
	static bool fWritingAllowed(const String& sSubKey, PrimaryKeyUsed keyUsed = pkuUSER);

private:
	IlwisSettings();
};

#endif
