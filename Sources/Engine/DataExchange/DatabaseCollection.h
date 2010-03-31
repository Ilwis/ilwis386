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
#ifndef DATABASECOLLECTION_H
#define DATABASECOLLECTION_H

#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\DataExchange\ForeignCollection.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"

class DataBaseCollectionPtr;

const String sTypeDataBaseCollection("DataBaseCollection");

class DataBaseCollectionPtr : public ForeignCollectionPtr
{
public:	
	_export DataBaseCollectionPtr(const FileName& fn, ParmList& pm);
	~DataBaseCollectionPtr();

	virtual void            Store();
  virtual String          sType() const;	
	virtual void _export    Add(const FileName& fn);
	virtual void            GetObjectDependencies(Array<FileName>& afnObjDep);
	void _export            Fill();		
	virtual bool            fCanAdd(const FileName& fn);
	static ObjectCollectionPtr *DataBaseCollectionPtr::Create(const FileName& fn, ParmList& pm);
	void _export            Create(ParmList& pm);
	void _export			CreateImpliciteObject(const FileName& fn, ParmList& pm);
	String _export          sContainer();


private:

	String sExpr;
	String sDataBaseName;
};

class DataBaseCollection : public ForeignCollection
{
public:
	_export DataBaseCollection();
	_export DataBaseCollection(const FileName& fn, ParmList& pm);
	_export DataBaseCollection(const FileName& fn);
	_export ~DataBaseCollection();
	FileName createCollectionName(const String& name);

	DataBaseCollectionPtr *ptr() const
	{ return static_cast<DataBaseCollectionPtr *>(pointer()); }
	DataBaseCollectionPtr* operator->()
	{ return ptr(); }
	static DataBaseCollectionPtr *pGet(const FileName& fn);
private:

};

#endif;
