#pragma once

IlwisObjectPtr * createMapListChangeDetection(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );
ApplicationMetadata metadataMapListChangeDetection(ApplicationQueryData *query);

class MapListChangeDetection : public MapListVirtual 
{
public:
	MapListChangeDetection(const FileName& fn, MapListPtr& ptr);
	MapListChangeDetection(const FileName& fn, MapListPtr& _ptr,const MapList& mplIn, const Map& mpBase, double threshld, bool undef);
	MapListChangeDetection(const FileName& fn, MapListPtr& _ptr,const MapList& mplIn1, const MapList& mplIn2, double threshld, bool undef);
	static MapListVirtual *create(const FileName& fn, MapListPtr& ptr, const String& sExpr);
	virtual ~MapListChangeDetection();
	virtual void Store();
	virtual bool fFreezing();
	virtual String sExpression() const;
private:
	void Init();
	bool withBaseMap();
	bool twoMapLists();
	bool oneMapList();
	MapList mpl1;
	MapList mpl2;
	Map mapBaseLine;
	double threshold;
	bool undefHandling;
};
