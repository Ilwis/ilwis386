#pragma once

IlwisObjectPtr * createTableMapListAttributeFrom(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );
ApplicationMetadata metadataTableMapListAttributeFrom(ApplicationQueryData *query);

class TableMapListAttributeFrom : public TableVirtual 
{
public:
	TableMapListAttributeFrom(const FileName& fn, TablePtr& ptr);
	TableMapListAttributeFrom(const FileName& fn, TablePtr& _ptr, const MapList& mpl, const String& source, const String& method);
	static TableMapListAttributeFrom *create(const FileName& fn, TablePtr& ptr, const String& sExpr);
	virtual ~TableMapListAttributeFrom();
	virtual void Store();
	//virtual bool fDomainChangeable() const;
 //   virtual bool fValueRangeChangeable() const;
	bool fFreezing();
	virtual String sExpression() const;
private:
	void Init();
	MapList mpl1;
	String method;
	String sourceDefinition;
	int extra;
};
