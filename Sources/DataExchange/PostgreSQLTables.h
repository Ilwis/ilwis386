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
#ifndef PostgreSQLTables_H
#define PostgreSQLTables_H

class DataBaseCollection;

ForeignFormat *CreateQueryObjectPostGres() ;//create query object
ForeignFormat *CreateImportObjectPostGres(const FileName& fnObj, ParmList& pm); //create import object

class PostgreSQLTables : public ForeignFormat
{
public:
	PostgreSQLTables() ;	
	//PostgreSQLTables(const FileName& fn, ParmList& pm);
	PostgreSQLTables(const FileName& fn, ParmList& pm);
	~PostgreSQLTables();

	static String _export sFormatPostGreSQL(const FileName& fnForeign);
	
	virtual bool _export				 fIsSupported(const FileName& fn, ForeignFormat::Capability dtType = cbANY) const;
	virtual bool _export         fIsCollection(const String& sForeignObject) const;
	virtual bool _export         fMatchType(const String& sFileName, const String& sType) ;	
	void virtual _export         LoadTable(TablePtr* tbl) ;
	virtual void _export         PutDataInCollection(ForeignCollectionPtr* collection, ParmList& pm);	
	virtual void _export         PutStringField(const String& sColumn, long iRecord, const String& sValue) ;
	virtual void _export         PutUByteField(const String& sColumn, long iRecord, unsigned char iValue) {};		
	virtual void _export         PutByteField(const String& sColumn, long iRecord, char iValue) {};				
	virtual void _export         PutLongField(const String& sColumn, long iRecord, long iValue) {};						
	virtual void _export         PutULongField(const String& sColumn, long iRecord, unsigned long iValue) {};								
	virtual void _export         PutRealField(const String& sColumn, long iRecord, double rValue) {};								
	virtual void _export         PutBoolField(const String& sColumn, long iRecord, bool fValue) {};
	virtual void _export         PutCoordField(const String& sColumn, long iRecord, Coord cValue) {} ;										
	virtual void                 Store(IlwisObject obj);
	virtual void                 ReadParameters(const FileName& fn, ParmList& pm);
	_export FileName             createCollectionName(const String& name, ParmList& pm);
	_export void                 getImportFormats(vector<ImportFormat>& formats);
protected:

	DomainValueRangeStruct       dmTranslateDataTypeToIlwis(String type, bool &fReadOnly, const String& sColName, TablePtr *tbl);
	void						 CreateColumns(PostGreSQL& db, TablePtr *tbl, int iNumColumn, vector<String>& vDataTypes);
	void						 FillRecords(PostGreSQL& db, TablePtr* tbl, int iNumRecords, int iNumColumns, const vector<String>& vDataTypes);
	void						 SetTable(LayerInfo& li, ForeignFormat::mtMapType mtType);
	CoordSystem					 getCoordSystem(const FileName& fnBase, const String& srsName);
	void                         PutData(Column& col, long iRec, const String& data);
//	void						 retrieveFromCollection(const FileName& fnObj, ParmList& pm);
	String						 sCreateConnectionString(const ForeignCollection& col, ParmList& pm);
	String						 makeKey(const FileName& fn);
	String						 expandSentence(const String& p);
	String						 encrypt(const FileName& fn, const String& sentence);
	String						 decrypt(const FileName& fn, const String& sentence);
	String                       sQuery;
	String                       sConnectionString;
	set<String>                  sarForeignCols; // original column names, for efficiency reason they are stored.
	                                      // they are used if data are to be stored in ilwis columns that
	                                      // are not part of the original table. prevent unneeded opens
	String						 tableName;
	map<String, Column>			 psgrToIlwMapping;
	FileName					 fnTable;
	String						 host;
	String						 username;
	String						 password;
	String						 port;
	String						 database;
//	String					     table;
};

#endif
