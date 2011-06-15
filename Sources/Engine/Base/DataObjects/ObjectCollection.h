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
#ifndef ILWOBJECTCOLLECTION_H
#define ILWOBJECTCOLLECTION_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\objdepen.h"

class ObjectCollection;
class ObjectCollectionPtr;
class Table;

typedef ObjectCollectionPtr* (*CollectionCreate)(const FileName& fn, ParmList&);
typedef map<String, CollectionCreate> mapCreates;


class ObjectCollectionPtr : public IlwisObjectPtr
{
	friend class ObjectCollection;
	friend class ObjectCollectionVirtual;

public:
	virtual _export         ~ObjectCollectionPtr();
	virtual void            GetObjectDependencies(Array<FileName>& afnObjDep);
	virtual void _export    GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
	virtual void _export    Store();
	virtual String          sType() const;
	virtual _export bool    fCanAdd(const FileName& fn);
	virtual void _export    Add(const IlwisObject& );
	virtual void _export    Add(const FileName& fn);
	void _export            Remove(const IlwisObject& );
	void _export            Remove(const FileName& fnObj); // remove single object from object collection
	void _export            RemoveAllObjects(); // remove all objects from object collection
	IlwisObject  _export    ioObj(int i);
	int _export             iNrObjects();
	FileName _export        fnObject(int i);
	void _export            DeleteEntireCollection();
	void _export			GetObjectStructure(ObjectStructure& os);
	bool _export			fUsesDependentObjects() const;
	bool _export            fObjectAlreadyInCollection(const FileName& fn) const;
	static ObjectCollectionPtr* Create(const FileName& fn, ParmList& ParmList);
	static ObjectCollectionPtr*	create(const FileName& fn, const String& sExpression);
	bool					fDependent() const;
	void					BreakDependency();
	bool					fCalculated() const;
	void				    Calc(bool fMakeUpToDate);
	void _export			SetAttributeTable(const Table& tbl);
    bool _export         		fTblAtt() const;
    Table _export        		tblAtt() const;
	set<IObjectType>					getFilterTypes() const;
	void					addFilterType(IObjectType type);
	void					removeFilterType(IObjectType type);
	bool	_export			getStatusFor(int query) const;
	CoordBounds	_export 	cb() const;
	RangeReal _export		getRange() ;
	

protected:
	_export ObjectCollectionPtr(const FileName&);
	_export ObjectCollectionPtr(const FileName&, bool /* fCreate */);
	void OpenCollectionVirtual();

	vector<FileName>		arObjects;
	ObjectDependency        objdep;
	ObjectCollectionVirtual *ocv;
	Table   				attTable;
	set<IObjectType>		filterTypes;
	int status;
	RangeReal				range;
};

class ObjectCollection: public IlwisObject
{
public:
	enum ContentStatus{csANY=0,csALLBASEMAP=1, csSAMEDOMAIN=2,csSAMECSY=4, csSAMEGEOREF=8,csALLRASTER=16,csALLPOINT=32, csALLSEGMENT=64,csALLPOLYGON=128};
	_export ObjectCollection();
	_export ObjectCollection(const FileName& fn);
	_export ObjectCollection(const FileName& fn,  const String& sType, ParmList& pm);
	_export ObjectCollection(const ObjectCollection& mv);
	_export ObjectCollection(const FileName& fn,  const String& sExpr);
	_export static ObjectCollectionPtr *Create(const FileName& fn, const String& sExpr, ParmList& pm=ParmList());
	_export static ObjectCollectionPtr *pGet(const FileName& fn);	


	void operator = (const ObjectCollection& mv) 
		{ SetPointer(mv.pointer()); }
	ObjectCollectionPtr* ptr() const
		{ return static_cast<ObjectCollectionPtr*>(pointer()); }
  ObjectCollectionPtr* operator -> () const 
		{ return ptr(); }
	_export void Init();
	static mapCreates	mapCreateFuncs;	
	
protected:
	static IlwisObjectPtrList listObjectCollection;
};

inline ObjectCollection undef(const ObjectCollection&) 
{ return ObjectCollection(); }

#endif



