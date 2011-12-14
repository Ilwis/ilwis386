#pragma once


namespace ILWIS {
	class NewDrawer;

class _export ZValueMaker {
public:
	enum ZValueTypeScaling{zvsNONE, zvsLINEAR, zvsLOGARITHMIC};
	enum SourceType{styNONE,stySELF,styMAP,styMAPLIST, styTABLE};
	ZValueMaker(NewDrawer *drw);
	void setSpatialSource(const BaseMap& mp, const CoordBounds& cb);
	void setDataSourceMap(const BaseMap& mp);
	void setTable(const Table& tbl, const String& colName);
	void setTable(const Table& tbl, const vector<String>& names);
	void setScaleType(ZValueTypeScaling t);
	double getValue(const Coord& crd, Feature *f = 0);
	double scaleValue(double value);
	void setOffset(double u=rUNDEF, bool useTrueCoords=false);
	double getOffset() const;
	RangeReal getRange() const;
	void setRange(const RangeReal& rr);
	void setThreeDPossible(bool v);
	bool getThreeDPossible() const;
	double getZScale() const;
	void setZScale(double v);
	void store(const FileName& fn, const String& section);
	void load(const FileName& fn, const String& section);
	void setZOrder(int index, double base);
	int getZOrder() const;
	double getZ0(bool is3D) const;
	void setBounds(const CoordBounds& bnd);
	BaseMapPtr * getSourceRasterMap() const;
	BaseMap getSpatialSourceMap() const;
	void addRange(const BaseMap& mp);
	SourceType getSourceType() const;
	void setSourceType(SourceType s);
	String getColumnName(int index);
	CoordBounds getBounds() const;

private:
	BaseMap spatialsourcemap;
	BaseMap datasourcemap;
	bool isSameCsy;
	Table table;
	vector<Column> columns;
	ZValueTypeScaling scalingType;
	double offset;
	RangeReal range;
	IlwisObject::iotIlwisObjectType type;
	bool threeDPossible;
	double zscale;
	CoordBounds cbLimits;
	int zOrder; // because OpenGL limitations, each set drawer must be drawn for height=0 at a different level.
	double fakeZ; // the faze z value for objects at heigh 0 for the user of this z-maker
	NewDrawer *associatedDrawer;
	bool isSetDrawer;
	SourceType sourceType;

};
}