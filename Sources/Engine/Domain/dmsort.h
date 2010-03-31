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
/* DomainClass
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   24 Sep 97    9:39 pm
*/

#ifndef ILWDOMSORT_H
#define ILWDOMSORT_H
#include "Engine\Table\tbl.h"
#include "Engine\Table\Col.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include <set>


#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif


class StringIndexDmSort {
public:  
  StringIndexDmSort()
  { iInd = iUNDEF; }
  StringIndexDmSort(const String& sStr, long iIndex=iUNDEF)
  { s = sStr; iInd = iIndex; }
  bool operator==(const StringIndexDmSort& si) 
  { return fCIStrEqual(s, si.s); }
  String s;
  long iInd;
};

enum DomainSortExistType { dseNONEEXIST=0, dseVALUEEXIST, dseCODEEXIST, dseBOTHEXIST, dseCODEISNAME };

class WarningPrefixNotRenamed : public ErrorObject
{
public:
	WarningPrefixNotRenamed(String sErr);
};

//! class DomainSort stores sortable items (no mumerics) with a ordered relation to map elements.
class IMPEXP DomainSort: public DomainPtr
{
friend class Domain;
friend class DomainPtr;
friend class TableView;

public:
  DomainSort(const FileName&, long iNr, const String& sPrefix = "");
  virtual ~DomainSort();

  enum                DomainSortingType 
                        { dsMANUAL = 1, dsALPHA, dsALPHANUM, dsAUTO, dsCODEALPHA, dsCODEALNUM } dsType;
  void                Store();
  void                SetOrd(long iRaw, long iOrd);
  void                SetOrdWithoutIndex(long iRaw, long iOrd);
	void                ReSort();  // sort the domain acoording to the selected method
  void                SortAlphabetical();
  void                SortAlphaNumeric();
  void                SortCodeAlphabetical();
  void                SortCodeAlphaNumeric();
  void                SortNumerical();
  void                SortRawValues();
  void                SetPrefix(const String& sNewPrefix, Tranquilizer* trq = 0);
  String              sGetPrefix();

  virtual void        Delete(long iRaw);
  void                SetVal(long iRaw, const String& sValue);
  void                SetCode(long iRaw, const String& sCode); // adds column if not yet there
  void                SetDescription(long iRaw, const String& sDesc);// adds column if not yet there
  void                SetNoAttributeTable();
  void                CalcIndex(bool fUpdateRequired = true);
  void                Resize(long iNr);
	long                iMergeAdd(const String& sName, const String& sCode);
  virtual void        Merge(const DomainSort* pdsrt,  Tranquilizer* trq=NULL);
  void                SetAttributeTable(const Table& tbl);
  //void                AddCol(const Column&);
  virtual void        GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
  long                iKey(long iOrd) const;
  long                iOrd(long iRaw) const;
  long                iOrd(const String& sValue) const;
  long                iSize() const { return iRecs(); }
  long                iNettoSize() const { return iNr; }
  long                iAdd(const String& sValue, bool fAllNew=false);
  virtual long        iRaw(const String& sValue) const;
  virtual long        iMaxStringWidth() const;
  void                AddValues(vector<String>& values);

  virtual StoreType   stNeeded() const;
  virtual String      sValueByRaw(long iRaw, short iWidth=-1, short=-1) const;
  String              sCodeByRaw(long iRaw, short iWidth = -1) const;
  String              sNameByRaw(long iRaw, short iWidth = -1) const;
  String              sDescriptionByRaw(long iRaw) const;
  String              sValue(long iOrd, short iWidth=-1, short iDec=-1) const
                         { return sValueByRaw(iKey(iOrd), iWidth, iDec); }
  bool                fEqualVal(long iRaw, const String& sVal) const; // checks code and name  
	long                iFindCodeRaw(const String& sCode);
	long                iFindNameRaw(const String& sName);
  bool                fContains(const DomainSort* pds) const;
  bool                fTblAtt() 
                         { return _fAttTable; }
  bool                fCodesAvailable() const 
                         { return colCode.fValid() ; }
  bool                fDescriptionsAvailable() const 
                         { return colDesc.fValid(); }
  virtual bool        fValid(const String& sValue) const;
  virtual bool        fEqual(const IlwisObjectPtr& ptr) const;

  Table               tblAtt();
  ValueRange          vrConstruct() const;
  virtual void GetObjectStructure(ObjectStructure& os);
  virtual void DoNotUpdate();	
  void Load();

protected:
  DomainSort(const FileName&);

  void                CreateColumns();

  int                 iCols();
  Column              col(int) const;
  long                iRecs() const 
                        { if (tbl.fValid()) return tbl->iRecs(); else return iNr; }
  long                iNr;
  Table               tbl;
  Column              colOrd;
  Column              colInd;
  Column              colStr;
  Column              colCode;
  Column              colDesc;
  String              sPrefix;
  bool                fLoaded;

private:
  DomainSortExistType dse(const String& sValue, const String& sCode);

  void                CreateCodeColumn();
  void                CreateDescriptionColumn();

  Table               tblAttLoad();
  int                 iHashSize;
  bool                FillHash();
  long                iFindHashName(const String& sVal) const 
                         { return iFindHash(htName, sVal); }
  long                iFindHashCode(const String& sVal) const 
                         { return iFindHash(htCode, sVal); }
  long                iFindHashCodeName(const String& sVal) const 
                         { return iFindHash(htCodeName, sVal); }
  void                AddHashName(const String& s, long iInd) 
                         { AddHash(htName, s, iInd); }
  void                AddHashCode(const String& s, long iInd) 
                         { AddHash(htCode, s, iInd); }
  void                AddHashCodeName(const String& s, long iInd) 
                         { AddHash(htCodeName, s, iInd); }
  void                RemoveHashName(const String& s) 
                         { RemoveHash(htName, s); }
  void                RemoveHashCode(const String& s) 
                         { RemoveHash(htCode, s); }
  void                RemoveHashCodeName(const String& s) 
                         { RemoveHash(htCodeName, s); }
  bool                fHashName() const 
                         { return htName.iTabSize != 0; }
  bool                fHashCode() const 
                         { return htCode.iTabSize != 0; }
  bool                fHashCodeName() const 
                         { return htCodeName.iTabSize != 0; }
  long                iFindHash(const HashTable<StringIndexDmSort>& hts, const String& sVal) const;
  void                AddHash(HashTable<StringIndexDmSort>& hts, const String& s, long iInd);
  void                RemoveHash(HashTable<StringIndexDmSort>& hts, const String& s);
  void                ChangeName(long j, String& s);

//  Table               _tblAtt;
  bool                _fAttTable;
  HashTable<StringIndexDmSort>    htCode;
  HashTable<StringIndexDmSort>    htName;
  HashTable<StringIndexDmSort>    htCodeName;
};

#endif
