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
// StereoPairEpiPolar.h: interface for the StereoPairEpiPolar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEREOPAIREPIPOLAR_H__8CE7478D_2F7E_47F0_8FC7_14F4D5E591D8__INCLUDED_)
#define AFX_STEREOPAIREPIPOLAR_H__8CE7478D_2F7E_47F0_8FC7_14F4D5E591D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Engine\Stereoscopy\StereoPairVirtual.h"

class StereoPairEpiPolar : public StereoPairVirtual  
{
friend DATEXPORT StereoPairVirtual;
friend DATEXPORT GeoRefEpipolar;

public:
	_export GeoRefEpipolar *pGetEpiLeft();
	_export GeoRefEpipolar *pGetEpiRight();
	_export String sLeftInputMapName() const;
	_export String sRightInputMapName() const;
	_export String sErrGeorefs(); // if s != "", calc/store can't proceed (show s to user)
	virtual String sExpression() const;
	StereoPairEpiPolar(const FileName& fn, StereoPairPtr& p, 
											const Map& mpL, const Map& mpR);									 
protected:
	static const char* sSyntax();
  virtual void Store();
	//StereoPairEpiPolar();
	StereoPairEpiPolar(const FileName& fn, StereoPairPtr& p);
	
	virtual ~StereoPairEpiPolar();
	bool fFreezing();
	String sLeftInputPhoto;
	String sRightInputPhoto;
	GeoRef grNewLeft;
	GeoRef grNewRight;
private:
	
};

#endif // !defined(AFX_STEREOPAIREPIPOLAR_H__8CE7478D_2F7E_47F0_8FC7_14F4D5E591D8__INCLUDED_)
