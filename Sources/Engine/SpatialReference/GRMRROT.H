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
/* GeoRefMirrorRotate
   Copyright Ilwis System Development ITC
   november 1995, by Wim Koolhoven
	Last change:  WK   15 Jul 97    1:50 pm
*/


#ifndef ILWGRMIRRROT_H
#define ILWGRMIRRROT_H
#include "Engine\SpatialReference\Gr.h"

enum MirrorRotateType { mrNORM=0, mrHOR, mrVERT, mrTRANS, mrDIAG, mrR90, mrR180, mrR270 };

class DATEXPORT GeoRefMirrorRotate: public GeoRefPtr
{
	friend class GeoRef;
	friend class GeoRefPtr;
	friend class GeoRefMirrorRotateForm;
	friend class DATEXPORT MapMirrorRotate;
	friend class DATEXPORT MapIterator;
protected:
	GeoRefMirrorRotate(const FileName& fn);
	_export GeoRefMirrorRotate(const FileName& fn, const GeoRef& grf, MirrorRotateType mr);
public:
	MirrorRotateType _export mrtMethod() const;
	void _export SetMirrorRotateMethod(MirrorRotateType mrt);

	virtual String sType() const;
	virtual void Store();
	virtual void Coord2RowCol(const Coord& c, double& rRow, double& rCol) const;
	virtual void RowCol2Coord(double rRow, double rCol, Coord& c) const;
	virtual double rPixSize() const;
	virtual bool fEqual(const IlwisObjectPtr& ptr) const;
	virtual bool fLinear() const;
	virtual bool fDependent() const; 
	static _export String sMirrorRotateType(MirrorRotateType mr);
	static _export MirrorRotateType mrType(const String& sMirrRot);
	void _export GetObjectStructure(ObjectStructure& os);
	virtual bool fGeoRefNone() const;
private:
	GeoRef gr;
	MirrorRotateType mr;
	bool fSwapRows, fSwapCols, fRotate;
};

#endif




