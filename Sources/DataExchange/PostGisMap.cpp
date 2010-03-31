#include "DataExchange\gdalincludes\gdal.h"
#include "DataExchange\gdalincludes\gdal_frmts.h"
#include "DataExchange\gdalincludes\cpl_vsi.h"
#include "DataExchange\gdalincludes\ogr_srs_api.h"

#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\URL.h"
#include "DataExchange\PostGisMap.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmsort.h"
#include "DataExchange\PostgreSQL.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\Cslatlon.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\point.hs"


PostGisMap::PostGisMap(const FileName& fn, PointMapPtr& p) : PointMapVirtual(fn, p)
{
}

PostGisMap::PostGisMap(const FileName& fn, PointMapPtr& p,BaseMapInfo& info) :
PointMapVirtual(fn,p, info.csy, info.cb, info.dvrs)
{
}

void PostGisMap::Store() {
	PointMapVirtual::Store();
	WriteElement("IlwisObjectVirtual", "Type", "PostGisMap");
}

//--------------------------------
PointMapVirtual* PostGisMap::create(const FileName& fn, PointMapPtr& p, const String& sExpr) {
  Array<String> as(8);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  URL host(as[0]);
  String database = as[1];
  String table = as[2];
  String column = as[3];
  String user = as[4];
  String pass = as[5];
  String schema = as[6] != "" ? as[6] : "public";
  String query = as[7] != "" ? as[7] : String("Select * from %S", table);
  String connstr = makeConnectionString(host,database,user,pass);
  BaseMapInfo inf = makeBaseMapInfo(fn,connstr,table,column,query);

  return new PostGisMap(fn,p,inf,host,database,table,column,user,pass,schema,query);
}

const char* PostGisMap::sSyntax() {
	return "PostGisMap(host, database, table, column, username, passwordd, schema, query)";
}

PostGisMap::PostGisMap(const FileName& fn, PointMapPtr& p) :PostGisMap(fn, p)
{
	ReadElement("PostGisMap","Database",database);
	ReadElement("PostGisMap","URL",host.sVal());
	ReadElement("PostGisMap","Table",table);
	ReadElement("PostGisMap","Column",column);
	ReadElement("PostGisMap","Username",user);
	ReadElement("PostGisMap","Password",pass);
	ReadElement("PostGisMap","Schema",schema);
	ReadElement("PostGisMap","Query",query);


}

PostGisMap::PostGisMap(const FileName& fn, PointMapPtr& p,BaseMapInfo& info, URL _host, const String& _database, 
												 const String& _table, const String& _column,const String& _user, const String& _pass, 
												 const String& _schema, const String& _query) :
PostGisMap(fn,p,info),
host(_host),
database(_database),
table(_table),
column(_column),
user(_user),
pass(_pass),
schema(_schema),
query(_query)
{
}

PostGisMap::~PostGisMap() 
{
}
String PostGisMap::sExpression() const{
	return "";
}
void PostGisMap::Store(){
	PostGisMap::Store();
	WriteElement("PostGisMap", "Type", "PostGisMap");
	WriteElement("PostGisMap","Database",database);
	WriteElement("PostGisMap","URL",host.sVal());
	WriteElement("PostGisMap","Table",table);
	WriteElement("PostGisMap","Column",column);
	WriteElement("PostGisMap","Username",user);
	WriteElement("PostGisMap","Password",pass);
	WriteElement("PostGisMap","Schema",schema);
	WriteElement("PostGisMap","Query",query);
}

bool PostGisMap::fDomainChangeable() const{
	return false;
}
bool PostGisMap::fValueRangeChangeable() const{
	return false;
}
void PostGisMap::CreatePointMapStore(){
}
long PostGisMap::iRaw(long iRec) const{
	return iUNDEF;
}
long PostGisMap::iValue(long iRec) const{
	return iUNDEF;
}
double PostGisMap::rValue(long iRec) const{
	return rUNDEF;
}
String PostGisMap::sValue(long iRec, short iWidth, short iDec) const {
	return "";
}
Coord PostGisMap::cValue(long iRec) const{
	return Coord();
}

String PostGisMap::makeConnectionString(URL host, const String& database, const String& user, const String& pass) {
	return String("host='%S' port='%d' dbname='%S' user='%S' password='%S' connect_timeout='10'", host.getHost(), host.getPort(), database,user, pass);
}

BaseMapInfo PostGisMap::makeBaseMapInfo(const FileName& fn, const String& connectionString, const String& table, const String& column, const String& query) {
	BaseMapInfo info;
	Domain dmKey(fn,0,dmtUNIQUEID);
	info.dvrs = DomainValueRangeStruct(dmKey);

	PostGreSQL db(connectionString.scVal());
	String envquery = String("select st_astext(Envelope(st_union(%S.%S))) as singleton from %S where ageom in (%S)",table, column, table,query);
	db.getNTResult(envquery.scVal());
	if ( db.getNumberOf(PostGreSQL::ROW) > 0) {
		String envelope(db.getValue(0,0));
		if ( envelope.sHead("(") == "POINT") {
			double x = envelope.sTail("(").sHead(" ").rVal();
			double y = envelope.sTail("(").sTail(" ").sHead(")").rVal();
			info.cb += Coord(x,y);
		} else if ( envelope.sHead("(") == "POLYGON") {
			String coordList = envelope.sTail("(").sHead(")");
			Array<String> parts;
			Split(coordList,parts,",");
			for(int i=0 ; i< 4;++i) {
				Coord c(parts[i].sHead(" ").rVal(),parts[i].sTail(" ").rVal());
				info.cb += c;
			}
		}
		String query("Select srid from geometry_columns where f_geometry_column='%S'", column);
		db.getNTResult(query.scVal());
		if(db.getNumberOf(PostGreSQL::ROW) > 0) {
			String srid(db.getValue(0,"srid"));
			info.csy = getCoordSystem( FileName(column), String("EPSG:%S",srid));
		}

	}
	return info;
}

CoordSystem PostGisMap::getCoordSystem(const FileName& fnBase, const String& srsName) {
	CFileFind finder;
	String path = getEngine()->getContext()->sIlwDir() + "\\gdal*.dll";
	BOOL found = finder.FindFile(path.scVal());
	if ( !found) 
		return CoordSystem();
	finder.FindNextFile();
	FileName fnModule (finder.GetFilePath());
	HMODULE hm = LoadLibrary(fnModule.sFullPath().scVal());
	CPLPushFinderLocationFunc findGdal = (CPLPushFinderLocationFunc)GetProcAddress(hm, "CPLPushFinderLocation");
	OSRNewSpatialReferenceFunc newsrs = (OSRNewSpatialReferenceFunc)GetProcAddress(hm, "_OSRNewSpatialReference@4");
	OSRImportFromEPSGFunc importepsg = (OSRImportFromEPSGFunc)GetProcAddress(hm,"_OSRImportFromEPSG@8");
	OSRGetAttrValueFunc getAttr = (OSRGetAttrValueFunc)GetProcAddress(hm,"_OSRGetAttrValue@12");
	OSRIsProjectedFunc isProjected = (OSRIsProjectedFunc)GetProcAddress(hm,"OSRIsProjected");

	path = getEngine()->getContext()->sIlwDir();
	path += "Resources\\gdal_data";
	findGdal(path.scVal());
	OGRSpatialReferenceH handle = newsrs(NULL);
	int epsg = srsName.sTail(":").iVal();
	OGRErr err = importepsg( handle, epsg);
	if ( err != OGRERR_NONE )
		throw ErrorObject(String("The SRS %S could not be used", srsName));

	String datumName(getAttr(handle, "Datum",0));
	//map<String, ProjectionConversionFunctions>::iterator where = mpCsyConvers.find(projectionName);


	FileName fnCsy(fnBase, ".csy");
	if ( _access(fnCsy.sRelative().scVal(),0) == 0)
		return CoordSystem(fnCsy);

	CoordSystemViaLatLon *csv=NULL;
	if ( isProjected(handle)) {
		CoordSystemProjection *csp =  new CoordSystemProjection(fnCsy, 1);
		String dn = Datum::WKTToILWISName(datumName);
		if ( dn == "")
			throw ErrorObject("Datum can't be transformed to an ILWIS known datum");
		csp->datum = new MolodenskyDatum(dn,"");
		csv = csp;
	} else {
		csv = new CoordSystemLatLon(fnCsy, 1);
		csv->datum = new MolodenskyDatum("WGS 1984","");
	}

	CoordSystem csy;
	csy.SetPointer(csv);

	return csy;
}