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
#ifndef TABLEFOREIGN_H
#define TABLEFOREIGN_H

#include "Engine\Table\tblstore.h"
#include "Engine\DataExchange\ForeignStore.h"
#include "Engine\DataExchange\ForeignFormat.h"

class ForeignFormat;

class TableForeign : public TableStore
{
	public:
		TableForeign(const FileName& fnObj, TablePtr& p, ParmList& pm);
		~TableForeign();
		void Store()	;
    virtual void _export  LoadBinary();
		void SetLoading(bool fYesNo = true);

		virtual void _export  PutStringInForeign(const String& sCol, long iRec, const String& str);
		virtual void _export  PutByteInForeign(const String& sCol, long iRec, char str);
		virtual void _export  PutUByteInForeign(const String& sCol, long iRec, unsigned char bV);
		virtual void _export  PutULongInForeign(const String& sCol, long iRec, unsigned long iV);
		virtual void _export  PutLongInForeign(const String& sCol, long iRec, long iV);	
		virtual void _export  PutRealInForeign(const String& sCol, long iRec, double rV);		
		virtual void _export  PutBoolInForeign(const String& sCol, long iRec, bool fV);
		virtual void _export  PutCoordInForeign(const String& sCol, long iRec, Coord cV);		

		_export static Table CreateDataBaseTable(const FileName& fnObject, ParmList& pm);		

	private:
		bool fLoading; // will be set during loading of the table to prevent any writebacks;
		String sTableName; // true name of a table used in a database;
		String sDataBase;
		String sMethod;

};

#endif
