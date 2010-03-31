#include "Engine\Applications\PNTVIRT.H"

typedef OGRErr (__stdcall *OSRImportFromEPSGFunc)( OGRSpatialReferenceH, int );
typedef void (*CPLPushFinderLocationFunc)( const char * );
typedef OGRSpatialReferenceH (__stdcall *OSRNewSpatialReferenceFunc)( const char *);
typedef const char* (__stdcall  *OSRGetAttrValueFunc)( OGRSpatialReferenceH hSRS, const char * pszName, int iChild /* = 0 */ );
typedef int (*OSRIsProjectedFunc)( OGRSpatialReferenceH );

struct BaseMapInfo {
	CoordSystem csy;
	CoordBounds cb;
	DomainValueRangeStruct dvrs;
};

class PostGisMap : public PointMapVirtual {
public:
	PostGisMap(const FileName&, PointMapPtr& p);
	PostGisMap(const FileName& fn, PointMapPtr& p,BaseMapInfo& info);
	
	void Store();
};

class PostGisMap : public IlwisObjectVirtual{
public:
  PointMapForeignPostgres(const FileName&, PointMapPtr& p);
  PointMapForeignPostgres(const FileName& fn, PointMapPtr&, BaseMapInfo& info, URL host, const String& database,const String& table, const String& column, const String& user, 
							const String& pass, const String& schema, const String& query); 
  static PointMapVirtual* create(const FileName&, PointMapPtr& p, const String& sExpression);
  static const char* sSyntax();
  ~PointMapForeignPostgres();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
  void CreatePointMapStore();
  virtual long iRaw(long iRec) const;
  virtual long iValue(long iRec) const;
  virtual double rValue(long iRec) const;
  virtual String sValue(long iRec, short iWidth=-1, short iDec=-1) const;
  virtual Coord cValue(long iRec) const;

private:
	static String makeConnectionString(URL host, const String& database, const String& user, const String& pass);
	static BaseMapInfo makeBaseMapInfo(const FileName& fn,const String& connectionString, const String& table, const String& column, const String& query);
	static CoordSystem getCoordSystem(const FileName& fnBase, const String& srsName);

	URL host;
	String database;
	String table;
	String column;
	String user;
	String pass;
	String schema;
	String query;
  // inline functions taken over from PointMapPtr for convenience:
};