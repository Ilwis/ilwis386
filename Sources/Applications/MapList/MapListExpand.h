#pragma once

IlwisObjectPtr * createMapListExpand(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );
ApplicationMetadata metadataMapListExpand(ApplicationQueryData *query);

class MapListExpand : public MapListVirtual 
{
public:
	enum Method{mMax, mMin, mHalf, mLinear};
	MapListExpand(const FileName& fn, MapListPtr& ptr);
	MapListExpand(const FileName& fn, MapListPtr& _ptr,const MapList& mplIn, int extr, MapListExpand::Method& m);
	static MapListVirtual *create(const FileName& fn, MapListPtr& ptr, const String& sExpr);
	virtual ~MapListExpand();
	virtual void Store();
	virtual bool fFreezing();
	virtual String sExpression() const;
private:
	void Init();
	MapList mpl1;
	MapListExpand::Method method;
	int extra;
};
