class  _export FeatureMap : public BaseMap {
public:
         FeatureMap();
         FeatureMap(const FileName& filename);
         FeatureMap(const FeatureMap& mp);

        void                         operator=(const FeatureMap& mp) 
                                       { SetPointer(mp.pointer()); }
        FeatureMapPtr*                  ptr() const 
                                       { return static_cast<FeatureMapPtr*>(pointer()); }
        FeatureMapPtr*                  operator->() const 
                                       { return ptr(); }
        static FeatureMapPtr*           pGet(const FileName& fn);
        static SegmentMapPtr  *pGetSegMap(const FileName& fn);
        static PolygonMapPtr  *pGetPolMap(const FileName& fn);
        static PointMapPtr    *pGetPntMap(const FileName& fn);
        static MapPtr         *pGetRasMap(const FileName& fn);
		static FeatureMapPtr 		 *pGetFeatureMap(const FileName& fn) {

    protected:
         FeatureMap(IlwisObjectPtr* ptr);

    private:
        static IlwisObjectPtrList    listMap;

};

class  _export FeatureMapPtr : public BaseMapPtr, public GeometryCollection {
public:
	virtual long iFeatures() const { return iUNDEF; } ;
	virtual Geometry *getFeature(long i) const { return NULL;}
	Geometry *getTransformedFeature(long iRec, const CoordSystem& csy) const	{ return NULL;}
	virtual Feature *newFeature(geos::geom::Geometry *pnt=NULL) { return NULL; }
protected:
	FeatureMapStore* pms;
	FeatureMapVirtual* pmv;	
	
};