#pragma once

#include "Engine\Applications\COLVIRT.H"

class CALCEXPORT Instructions;

IlwisObjectPtr * createColumnToTime(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class DATEXPORT ColumnToTime: public ColumnVirtual
{
  friend class ColumnVirtual;
public:
  ~ColumnToTime();
  bool fFreezing();
  static bool fValidFunc(const String& sAggFunc);
  static const char* sSyntax();
  virtual void Store();
  virtual String sExpression() const;
  ColumnToTime(const Table& tbl, const String& sColName, ColumnPtr& p);
  static ColumnVirtual* create(const Table& tbl, const String& sColName, ColumnPtr& p,
                                 const String& sExpression, const DomainValueRangeStruct& dvs);
private:
  ColumnToTime(const Table& tbl, const String& sColName, ColumnPtr& p, const DomainValueRangeStruct& dvs,
                 const Column& colSource, const Table& _tblOther);
  Column colSource;
  Table tblOther; // table may not leave scope of the column else the tableref in the column gets invalid (delayed load)

  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
};



