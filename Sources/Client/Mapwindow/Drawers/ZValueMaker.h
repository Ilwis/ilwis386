#pragma once

namespace ILWIS {
class _export ZValueMaker {
public:
	enum ZValueTypeScaling{zvsNONE, zvsLINEAR, zvsLOGARITHMIC};
	ZValueMaker();
	void setSpatialSourceMap(const BaseMap& mp);
	void setDataSourceMap(const BaseMap& mp);
	void setTable(const Table& tbl, const String& colName);
	void setScaleType(ZValueTypeScaling t);
	double getValue(const Coord& crd, Feature *f = 0);
	void setOffset(double u=rUNDEF);
	double getOffset() const;
	RangeReal getRange() const;
	void setThreeDPossible(bool v);
	bool getThreeDPossible() const;
	double getZScale() const;
	void setZScale(double v);

private:
	BaseMap spatialsourcemap;
	BaseMap datasourcemap;
	Table table;
	Column column;
	ZValueTypeScaling scalingType;
	double offset;
	RangeReal range;
	IlwisObject::iotIlwisObjectType type;
	bool self;
	bool threeDPossible;
	double zscale;

};
}