#pragma once

IlwisObjectPtr * createColumnAttributeFromMapList(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms);


ApplicationMetadata metadataColumnAttributeFromMapList(ApplicationQueryData *query);

class ColumnAttributeFromMapList : public ColumnVirtual 
{
public:
	ColumnAttributeFromMapList(const Table& tbl, const String& sColName, ColumnPtr& ptr);
	ColumnAttributeFromMapList(const Table& tbl, const String& sColName, ColumnPtr& ptr, const MapList& mpl, const String& source, const String& method);
	static ColumnAttributeFromMapList *ColumnAttributeFromMapList::create(const Table& tbl, const String& sColName, ColumnPtr& ptr,
									const String& sExpression, const DomainValueRangeStruct& dvs);
	virtual ~ColumnAttributeFromMapList();
	virtual void Store();
	virtual bool fDomainChangeable() const;
    virtual bool fValueRangeChangeable() const;
	double rComputeVal(long iKey) const;
	virtual String sExpression() const;
private:
	void Init();
	MapList mpl1;
	String method;
	String sourceDefinition;
	int extra;
};
