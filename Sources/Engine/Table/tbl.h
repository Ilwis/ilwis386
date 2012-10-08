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
/* $Log: /ILWIS 3.0/BasicDataStructures/tbl.h $
 * 
 * 25    2/19/04 4:14p Martin
 * added _export
 * 
 * 24    11/15/01 3:48p Martin
 * added setfilename that changes the filename and datafilename of object
 * 
 * 23    8/23/01 17:07 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 22    8/07/01 12:05p Martin
 * added functions to put data in foreign formats
 * 
 * 21    8/01/01 2:26p Martin
 * record is now in the Ilwis namespace
 * 
 * 20    3/20/01 20:33 Retsios
 * iNettoRecs added
 * 
 * 19    21-12-00 10:17a Martin
 * added the pGetVirtualObject() function to access the virtual object
 * embedded in the object. 
 * 
 * 18    21-11-00 10:05a Martin
 * implemented fUsesDependentObjects
 * 
 * 17    9/19/00 9:05a Martin
 * improved DoNoStore function for tables. It will now also protect the
 * columns
 * 
 * 16    9/18/00 9:35a Martin
 * Added interface functions to force a load from the table. Tables are
 * not loaded until the first value is requested
 * 
 * 15    8-09-00 3:21p Martin
 * added function to set the fChanged member of all the 'members of an
 * object.
 * added function to retrieve the 'structure' of an object (filenames)
 * 
 * 14    16-06-00 11:07a Martin
 * added section prefix for constructor of tables for foreign formats
 * 
 * 13    28-02-00 8:45a Martin
 * Construction and call to TableForeign is now added
 * 
 * 12    21-02-00 4:35p Martin
 * Added a function to quickly add a whole set of values (AddValues)
 * 
 * 11    19-01-00 1:57p Martin
 * 
 * 10    17-01-00 8:12a Martin
 * added friend class. 
 * 
 * 9     8-12-99 17:35 Koolhoven
 * Added HistogramWindow
 * which shows a histogram of a raster map
 * as a graph and as table at the same time.
 * 
 * 8     10/07/99 1:19p Wind
 * put table import in it's own thread
 * 
 * 7     6-10-99 12:58p Martin
 * see table.cpp
 * 
 * 6     9-09-99 2:39p Martin
 * ported 2.22 stuff
 * 
 * 5     8/11/99 10:46a Visser
 * changes for import ASCII table (intermediate version)
 * 
 * 4     6/30/99 3:39p Visser
 * adapted for the changes already made in verion 2.2 for import ASCII
 * table
 * 
 * 3     14-06-99 12:55 Koolhoven
 * Header comment
 * 
 * 2     14-06-99 12:47 Koolhoven
 * exports
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/12 16:09:59  Wim
// Added extra sSection(const char*) to prevent problems
// with temporary Strings.
//
/* Table, TablePtr
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   28 Apr 98   12:30 pm
*/

#ifndef ILWTBL_H
#define ILWTBL_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Table\Col.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Base\objdepen.h"


struct LayerInfo ;
enum eDomTypeT{eValRS, eStr};

namespace Ilwis
{
class Record;
}
class Table2DimPtr;
class SegmentMapStore;
class ForeignFormat;
class IlwisObjectVirtual;

class TablePtr: public IlwisObjectPtr
{
	friend void ImportTable(const Array<String>& as);
  friend class Table;
  friend class TableVirtual;
  friend class TableView;
  friend class TableImport;
  friend class TableStore;
  friend class ObjectDependency;
  friend class ColumnPtr;
	friend class SegmentMapStore;
  friend class TableHistogramInfo;
  friend class Table2DimPtr;
  static TablePtr* create(const FileName&, const String&, bool& fLoadColumns);
  static TablePtr* create(const FileName&, const String&);
	static TablePtr* create(const String& sExpression);
/*
  static TablePtr* create(const FileName& fn, const FileName& fnData,
        const String& sFormat, int iSkip, bool fUseAs,
        const String& sExpr);
*/
  public:
  static TablePtr *ImportTable(const FileName& fn, const String& sExpr); // called from conv.cpp
  _export TablePtr(const FileName&, const String& sSecPrf, bool fCreate=false);
  _export TablePtr(const FileName&, const FileName& fnDat, const Domain&, const String& sSecPrf);
  _export TablePtr(const FileName& fn, const FileName& fnDat, long iRecs, const String& sSecPrf); // domain of table is string
	_export TablePtr(const FileName& fn, const Domain& dm,  ParmList& pm, const String& sSecPrf="");
  void _export Load(); // load column data (is not done automatically by constructor)
  virtual _export ~TablePtr();
  virtual void _export Store();
  Table2DimPtr* pt2d() const;
  void _export Export(const FileName& fn) ;
  virtual String _export sType() const;
  int iCols() const { return _iCols; }
  long iRecs() const { return _iRecs; }
	long _export iNettoRecs() const;
  long iOffset() const { return _iOffset; }
  const Domain& dm() const { return _dm; }
  void _export SetDomain(const Domain& dm);
  virtual const Column _export &col(int) const; // virtual for table view
  virtual const Column _export &col(const String&) const;// virtual for table view
  long _export iCol(const String& sName) const;
  void _export DeleteRec(long iStartRec, long iRecs=1);
  long _export iRecNew(long iRecs = 1);
  const Column _export &colNew(const String& sName, const Domain& dm, const ValueRange& vr);
  const Column _export &colNew(const String&, const DomainValueRangeStruct& dvs);
  const Column _export &AddCol(const Column&);
  void _export RemoveCol(Column&);
  inline Ilwis::Record recFirst() const;
  inline Ilwis::Record recLast() const;
  inline Ilwis::Record rec(long iRow) const;
  inline Ilwis::Record recNew();
  void _export CheckNrRecs();
  static void Changed(const FileName& fnTbl, bool fChanged=true);
  virtual DomainInfo _export dminf() const;
  virtual void _export Rename(const FileName& fnNew);
  virtual void _export GetObjectDependencies(Array<FileName>& afnObjDep);
  virtual bool _export fDataReadOnly() ;// default: returns fReadOnly()
  void Replace(const String& sExpression);
//  virtual bool fAdditionalInfo() const;
//  virtual String sAdditionalInfo() const;
// next group are reimplemented (from IlwisObjectPtr):
  virtual void _export Updated();
  virtual void _export GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
  virtual _export String sExpression() const;
  virtual bool _export fDependent() const; // returns true if it's dependent point map
  virtual bool _export fCalculated() const; // returns true if a calculated result exists
//  virtual bool fUpToDate() const; // returns true if an up to dat calculate result exists
  virtual bool _export fDefOnlyPossible() const; // returns true if data can be retrieved without complete calculation (on the fly)
  virtual void _export Calc(bool fMakeUpToDate=false); // calculates the result;
  // fMakeUpToDate is true it first updates the dependencies
  virtual void _export DeleteCalc(); // deletes calculated result     
  void _export BreakDependency(); // only keeps calculated result, if nothing has been calculated it calculates first
  void CreateTableStore();
  String sSectionPrefix;
  void _export OpenTableVirtual(); // load TableVirtual and set member ptv
  String _export sSection(const String& sSect) const;
  String _export sSection(const char* sSect) const;
	TableVirtual* tv() const { return ptv; }
	void _export SetDataFile(const FileName& sFile);
	virtual void _export GetObjectStructure(ObjectStructure& os);
	virtual void _export DoNotUpdate();
	void _export DoNotStore(bool fValue);
    void _export LoadData();
	void _export Loaded(bool fValue);
	bool _export fUsesDependentObjects() const;
	virtual void _export SetFileName(const FileName& fn);	
	_export IlwisObjectVirtual *pGetVirtualObject() const;
	void _export PutStringInForeign(const String& sCol, long iRec, const String& str);
	void _export PutByteInForeign(const String& sCol, long iRec, char str);
	void _export PutUByteInForeign(const String& sCol, long iRec, unsigned char bV);
	void _export PutULongInForeign(const String& sCol, long iRec, unsigned long iV);
	void _export PutLongInForeign(const String& sCol, long iRec, long iV);	
	void _export PutRealInForeign(const String& sCol, long iRec, double rV);		
	void _export PutBoolInForeign(const String& sCol, long iRec, bool fV);			
	void _export PutCoordInForeign(const String& sCol, long iRec, Coord cV);
	void sharedValue(int column, bool yesno);
protected:
  int _iCols;
  long _iOffset;
  long _iRecs;
  Domain _dm;
private:
  static UINT TablePtr::ImportTableInThread(LPVOID p);
  FileName fnDat;
  TableStore* pts;
  TableVirtual* ptv;
  ObjectDependency objdep;
};

class Table: public IlwisObject
{
  static IlwisObjectPtrList listTbl;
public:
  _export Table();
  _export Table(const FileName& fn);
  _export Table(const FileName& fn, const Domain& dm);
  _export Table(const FileName& fn, const String& sExpression);
 /*
  _export Table(const FileName& fn, const FileName& fnData,
                const String& sFormat, bool fUseAs, int iSkip,
                const String& sExpr);
  */
  _export Table(const String& sExpression);
  _export Table(const String& sExpression, const String& sPath);
  _export Table(const Table& tbl);

  void             operator = (const Table& tbl) 
                     { SetPointer(tbl.pointer()); }
  TablePtr*        ptr() const 
                     { return static_cast<TablePtr*>(pointer()); }
  TablePtr*        operator -> () const 
                     { return ptr(); }
  const Column&    operator [] (int i) 
                     { return ptr()->col(i); }
  const Column&    operator [] (const String& s) 
                     { return ptr()->col(s); }
  _export static TablePtr* pGet(const FileName& fn);
  void _export     Export(const FileName& fn) const;
  static void _export  EnlargeTables(const DomainPtr*);
  static String _export sSectionPrefix(const String& sExt);
  static FileName fnFromExpression(const String& sExpr);
};

inline Table undef(const Table&) { return Table(); }

#endif // ILWTBL_H





