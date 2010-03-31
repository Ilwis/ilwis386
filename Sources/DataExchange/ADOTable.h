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
#ifndef ADOTABLE_H
#define ADOTABLE_H

#import "c:\Program Files\Common Files\System\ADO\msado15.dll" \
    no_namespace rename("EOF", "EndOfFile")

class DataBaseCollection;

ForeignFormat _export *CreateQueryObjectADO() ;//create query object
ForeignFormat _export *CreateImportObjectADO(const FileName& fnObj, ParmList& pm); //create import object

class ADOTable : public ForeignFormat
{
public:
	ADOTable() ;	
	ADOTable(const FileName& fn, const Array<String>& arParts);
	ADOTable(const FileName& fn, ParmList& pm);
	~ADOTable();

	
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
	virtual void _export		 ReadParameters(const FileName& fnObj, ParmList& pm);
	virtual void _export		 Store(IlwisObject ob);
	virtual void _export         getImportFormats(vector<ImportFormat>& formats);
private:

	void                         PrintProviderError(_ConnectionPtr pConnection, ErrorObject& err);
	void                         PrintComError(_com_error &e);
	DomainValueRangeStruct       dmTranslateDataTypeToIlwis(DataTypeEnum dte, bool &fReadOnly, const String& sColName, TablePtr *tbl);
	void                         PutData(Column& col, long iRec, _variant_t& result);
	void                         CreateKeyDomain(const String& sKeyDomain);
	void						 ReadParameter(const FileName& fnObj, ParmList& pm, const String& entry, const String& parmName);
	String                       sQuery;
	String                       sConnectionString;
	vector<Column>               columns;
	set<String>                  sarForeignCols; // original column names, for efficiency reason they are stored.
	                                      // they are used if data are to be stored in ilwis columns that
	                                      // are not part of the original table. prevent unneeded opens
	OLEDBProvider *provider;
	String username;
	String password;
	String initialdir;
	String database;
	String tableName;
};

#endif
