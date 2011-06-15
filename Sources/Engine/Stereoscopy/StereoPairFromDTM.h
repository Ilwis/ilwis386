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
// StereoPairFromDTM.h: interface for the StereoPairFromDTM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEREOPAIRFROMDTM_H__F10DBB79_0FC4_4199_8832_6A20E92D0514__INCLUDED_)
#define AFX_STEREOPAIRFROMDTM_H__F10DBB79_0FC4_4199_8832_6A20E92D0514__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Engine\Stereoscopy\StereoPairVirtual.h"

class StereoPairFromDTM : public StereoPairVirtual  
{
	friend StereoPairVirtual;
	friend GeoRefStereoMate;
	friend GeoRefPtr;
	enum LookModus {lmLEFT, lmBOTH, lmRIGHT};
	enum ResampleModus {rmFAST, rmACCURATE};
	static StereoPairFromDTM* create(const FileName& fn, StereoPairPtr& p, const String& sExpr);
public:
	StereoPairFromDTM(const FileName& fn, StereoPairPtr& p, 
										const Map& mpImage,	const Map& mpDTM,
										double rLookAng, double rRefH,
										LookModus lm, ResampleModus rm);
	String sExpression() const;

protected:
	static const char* sSyntax();
  virtual void Store();
	StereoPairFromDTM(const FileName& fn, StereoPairPtr& p);

	virtual ~StereoPairFromDTM();
	bool fFreezing();
	void Init();
	String help;
private:
	Map mpInputImage;
	Map mpDTM;
	double rLookAngle;
	double rRefHeight;
	LookModus lookModus;
	GeoRef grNewLeft;
	GeoRef grNewRight;
	bool fImage; // if true, resampling is done bilinearly
	long iColOffSet;
	long iColsExtended;
	long iParallProjectToStereoMate(RowCol rcIn, GeoRefStereoMate* pgStMate);
	ResampleModus resampleModus;
	void EraseOutputMap(Map &mp);
	void FindSingleLookAngles(double rLookAng, LookModus lm, double &rLookAngleL, double &rLookAngleR);
	bool fTransformDTMCoords;
	CoordSystem csDTM;
	double rSourceMapPixSize; 
};

#endif // !defined(AFX_STEREOPAIRFROMDTM_H__F10DBB79_0FC4_4199_8832_6A20E92D0514__INCLUDED_)
