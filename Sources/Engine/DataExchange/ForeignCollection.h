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
#ifndef ILWForeignCollection_H
#define ILWForeignCollection_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"

class ForeignCollection;
class DATEXPORT ForeignCollectionPtr;
class ForeignFormat;
struct LayerInfo;

const String sTypeForeignCollection("Foreign Collection");

class DATEXPORT ForeignCollectionPtr : public ObjectCollectionPtr
{
	friend class ForeignCollection;

public:
	_export virtual ~ForeignCollectionPtr();
	_export virtual void            GetObjectDependencies(Array<FileName>& afnObjDep);
	_export virtual void            GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
	virtual void					Store();
	virtual String					sType() const;
	virtual void _export            Create(ParmList& pm);
	_export static ObjectCollectionPtr *Create(const FileName& fn, ParmList& pm);
	_export String					sGetMethod();
	_export virtual String			sContainer();
	_export virtual String			sGetExpression();
protected:
	_export ForeignCollectionPtr(const FileName&, ParmList& pm);
	String sMethod;
	String sExpression;

private:
	void                    CreateLayer(int iLayer);
	void                    CreateRasterLayer(int iChannel);
	void                    LoadLayers();

	FileName fnForeign;
	ForeignFormat *ff;
};

class ForeignCollection: public ObjectCollection
{
public:
	_export ForeignCollection();
	_export ForeignCollection(const FileName& fn);
	_export ForeignCollection(const FileName& fn, ParmList& pm);
	_export ForeignCollection(const FileName& fn, const String& sType, ParmList& pm);	
	_export ForeignCollection(const ForeignCollection& mv);
	_export ~ForeignCollection();

	_export static void CreateImplicitObject(const FileName& fnObject, ParmList& pm); //maybe virtual later		
	static bool _export     GetTime(const FileName& fnForeign, CTime& LastWrite);
	static bool _export     fForeignFileTimeChanged(const FileName& fnForeign, const FileName& fnObj);
	void operator = (const ForeignCollection& mv) 
		{ SetPointer(mv.pointer()); }
	ForeignCollectionPtr* ptr() const
		{ return static_cast<ForeignCollectionPtr*>(pointer()); }
  ForeignCollectionPtr* operator -> () const 
		{ return ptr(); }
private:
	static void CreateVectorMap(const FileName& fn, LayerInfo li);
	static void CreateTable(const FileName& fn, ParmList& pm);
	static IlwisObjectPtrList listForeignCollection;
};

inline ForeignCollection undef(const ForeignCollection&) 
{ return ForeignCollection(); }

#endif



