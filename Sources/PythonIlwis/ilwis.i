%module ilwis

%{
#include "pvalue.h"
#include "pboundingbox.h"
#include "pcoordinate.h"
#include "platlon.h"
#include "PCoordinateSystem.h"
#include "pcoverage.h"
#include "pcoveragecollection.h"
#include "pcoveragelist.h"
#include "pdatatype.h"
#include "pellipsoid.h"
#include "pdatum.h"
#include "pfeature.h"
#include "pfeaturecoverage.h"
#include "pgeoreference.h"
#include "pgridcoverage.h"
#include "rootobject.h"
#include "pnumericdata.h"
#include "ppixel.h"
#include "PSRObject.h"
#include "pprojection.h"
#include "PCRSBoundingBox.h"
#include "pstringdata.h"
#include "ptable.h"
#include "pthematicdata.h"
#include "pvaluerange.h"
%}

%include "std_string.i"
%include "std_vector.i"

%template (Blockd) std::vector< std::vector< std::vector<double> > >
%template (Arrayd) std::vector< std::vector<double> >
%template (Lined) std::vector<double>;

class PPixel
{

public:
	PPixel();
	PPixel(long x, long y, long z=iUNDEF);

	long getX() const;
	long getY() const;
	long getZ() const;
	void setX(long v);
	void setXY(long y, long x);
	void setXYZ(long y, long x, long z);
	void setY(long v);
	void setZ(long v);
	bool isValid() const;
	bool isEqual(const PPixel& pix) const;
	bool isNear2D(const PPixel& pix, long tolerance) const;
	std::string toString() const;

};

class PCoordinate
{

public:
	PCoordinate();
	PCoordinate(const PCoordinate& coord);
	PCoordinate(double x, double y, double z=rUNDEF);	
	virtual ~PCoordinate();

	double getX() const;
	double getY() const;
	double getZ() const;
	void setX(double v);
	void setXYZ(double y, double x, double z = 0);
	void setY(double v);
	void setZ(double v);
	bool isValid() const;
	bool isNear(const PCoordinate& crd, double tolerance);
	bool isEqual(const PCoordinate& crd);
	std::string toString(long precission=0) const;
};

class PLatLon : public PCoordinate
{

public:
	PLatLon();
	virtual ~PLatLon();

	std::string getLat(long precision=0) const;
	std::string getLon(long precision=0) const;
	void setLat(const std::string& lat);
	void setLon(const std::string& lon);
	std::string toString(long precision=0) const;
	virtual bool isValid() const;
	double getPhi() const;
	double getLambda() const;
	void setPhi(double phi);
	void setLambda(double lambda);
};

class PBoundingBox
{
public:
	PBoundingBox();
	PBoundingBox(double minX, double minY, double maxX, double maxY);

	virtual void add(const PBoundingBox& box);
	double getMaxX() const;
	double getMaxY() const;
	double getMinX() const;
	double getMinY() const;
	void setMaxX(double v) ;
	void setMaxY(double v) ;
	void setMinX(double v) ;
	void setMinY(double v) ;
	
	virtual bool isValid() const;
	double getWidth() const;
	double getHeight() const;
	bool isInside(double X, double Y) const;	
};

class PGridBoundingBox : public PBoundingBox
{

public:
	PGridBoundingBox();
	virtual ~PGridBoundingBox();
	
	void add(const PPixel& pix);
	bool isInside(const PPixel& pix);
	PGridBoundingBox& operator+(const PGridBoundingBox& box);	

};

class PCRSBoundingBox : public PBoundingBox
{

public:
	PCRSBoundingBox();
	virtual ~PCRSBoundingBox();
	double getMaxZ() const;
	double getMinZ() const;
	void setMaxZ(double v) ;
	void setMinZ(double v) ;
	PCRSBoundingBox& operator+(const PCRSBoundingBox& box);	
	
	virtual bool isValid() const;
	void add(const PCoordinate& coord);
	bool isInside(const PCoordinate& coord);
};

class PSRObject
{

public:
	PSRObject();
	virtual ~PSRObject();

	int getEPSG();
	string getName();

};

class PCoordinateSystem : public PSRObject
{
public:
	PCoordinateSystem();
	PCoordinateSystem(const string& name );
	virtual ~PCoordinateSystem();

	void convertBB(PCRSBoundingBox bb);
	PCoordinate convertCrd(const PCoordinateSystem& source, const PCoordinate& crd) const;
	PCRSBoundingBox convertBB(const PCoordinateSystem& sourceCsy, const PCRSBoundingBox& bb) const;
	bool isProjected() const;
	bool isValid() const;
	PDatum getDatum() const;
	void setDatum(const PDatum& dat);
	PEllipsoid getEllipsoid() const;
	PProjection getProjection() const;
	void setProjection(const PProjection& proj);
	
};

class PProjection : public PSRObject
{

public:
	// python interface
	enum ProjectionParameter{ppUnknown, ppFalseEasting, ppFalsNorthing, ppCentralMeridian, ppLatitudeOfTrueScale,ppCentralParallel,ppStandardParallel_1, ppStandardParallel_2,
		ppNorthernHemisphere,ppHeight,ppTiltedProjPlane,ppTiltOfPlane,ppAzimuthYAxis, ppAzimuthCentralLine, ppPoleObliqueCylinder,ppNorthOriented, ppScale, ppZone};
	PProjection();
	PProjection(const std::string& name) ;
	PProjection(const std::string& name, const std::string ellipsoid);	
	virtual ~PProjection();

	double getProjectionParameter(ProjectionParameter id) const;
	void setProjecttionParameter(ProjectionParameter id, double v);
	PEllipsoid getEllipsoid() const;
	int getEPSG() const;
	string getName() const;
	bool isEqual(const PProjection& dat) const;
	bool isValid() const;

};

class PEllipsoid : public PSRObject {
public:
	//python interface
	PEllipsoid();
	PEllipsoid(const std::string& ell);
	PEllipsoid(double axis, double flattening);
	virtual ~PEllipsoid();

	bool isSpherical() const;
	bool isEqual(const PEllipsoid& ell) const;
	bool isValid() const;
	double getDistance(const PLatLon& begin, const PLatLon& end) const;
	double getAzimuth(const PLatLon& begin, const PLatLon& end) const;
	double getMajorAxis() const;
	double getMinorAxis() const;
	double getFlattening() const;
	double getExcentricity() const;

};

class PDatum : public PSRObject
{

public:
	PDatum();
	PDatum(const std::string& ellipsoid, const std::string& area);
	PDatum(const std::string& ellipsoid, double dx, double dy, double dz);
	PDatum(const std::string& ellipsoid, double dx, double dy, double dz, double rotX, double rotY, double rotZ, double dS);
	PDatum(const std::string& ellipsoid, double dx, double dy, double dz, double rotX, double rotY, double rotZ, double dS,double X0, double Y0, double Z0);
	PLatLon fromWGS84(const PLatLon& ll);
	PLatLon toWGS84(const PLatLon& ll);
	int getEPSG() const;
	std::string getName() const;
	std::string getArea() const;
	PEllipsoid getEllipsoid() const;

	bool isEqual(const PDatum& dat);
	bool isValid() const;
	virtual ~PDatum();


};

class PDataType
{
public:
	enum DType{tNumeric, tThematic, tString, tCoordinate, tTime};
	PDataType();
	PDataType(const std::string& filename);
	PDataType(const std::string& name, DType type);
	virtual ~PDataType();

	std::string getName();
	int getType();
	virtual bool isValid(PValue v);
};

class PStringData : public PDataType
{

public:
	PStringData();
	virtual ~PStringData();

};

class PThematicData : public PDataType
{

public:
	PThematicData();
	virtual ~PThematicData();

};

class PNumericData : public PDataType
{

public:
	PNumericData();
	virtual ~PNumericData();

};

class RootObject
{

public:
	RootObject();
	virtual ~RootObject();

	string getId();
	double getModifiedTime();
	string getName();

};

class PTable : public RootObject
{
public:
	PTable();
	virtual ~PTable();

	int getColumnCount();
	int getColumnIndex(string name);

};

class PCoverage : public RootObject
{
public:
	PCoverage();
	PCoverage(const std::string& name);
	virtual ~PCoverage();

	PCoordinateSystem getCoordinateSystem();
	PDataType getDataType();
	PCRSBoundingBox getSRSBoundingBox();
	int getType();
	PValue getValue(PCoordinate location);
};

class PFeatureCoverage : public PCoverage
{

public:
	PFeatureCoverage();
	virtual ~PFeatureCoverage();

	PFeature getFeatures(PCoordinate location);
	PFeature getFeatures(PCRSBoundingBox box, string query = "");
	void removeFreature(string id);

};

class PGeoReference : public RootObject
{

public:
	PGeoReference();
	PGeoReference(const std::string& name);
	virtual ~PGeoReference();

	PPixel coordToPixel(PCoordinate location);
	PCoordinateSystem getCoordinateSystem();
	PCoordinate pixelToCoordinate(PPixel location);
	void setCoordinateSystem(PCoordinateSystem csy);

};

class PGridCoverage : public PCoverage
{

public:
	PGridCoverage();
	PGridCoverage(const string& name);
	virtual ~PGridCoverage();

	PGeoReference getGeoReference();
	PGridBoundingBox getGridBoundingBox();
	void getLine(std::vector<PValue>, int line);
	PValue getValue(PPixel location);
	void putLine(std::vector<PValue>, int line);
	PGridCoverage operator+(const PGridCoverage& cov);
	PGridCoverage operator-(const PGridCoverage& cov);
	PGridCoverage operator*(const PGridCoverage& cov);
	PGridCoverage operator/(const PGridCoverage& cov);
};

class PCoverageCollection : public RootObject
{

public:
	PCoverageCollection();
	virtual ~PCoverageCollection();
	PCoverage *m_Coverage;

	void addCoverage(PCoverage cov);
	int getCount();
	PCoverage getCoverage(int index);
	void removeCoverage(int index);

};

class PCoverageList : public PCoverageCollection
{

public:
	PCoverageList();
	virtual ~PCoverageList();

	void addCoverage(PCoverage cov, PCoverage index);

};

class PValue
{
public:
	PValue();
	virtual ~PValue();

	int iValue();
	double rValue();
	string sValue();

};

class PValueRange
{

public:
	PValueRange();
	virtual ~PValueRange();

	PValue getMaxValue();
	PValue getMinValue();
	void isValid(PValue v);
	void setMaxValue(PValue v);
	void setMinValue(PValue v);

};

class PFeature
{

public:
	PFeature();
	virtual ~PFeature();

	vector<PCoordinate> getCoordinates();
	string getId();
	int getType();
	PValue getValue();
	void setCoordinates(vector<PCoordinate> crds);
	void setValue(PValue value);

};


