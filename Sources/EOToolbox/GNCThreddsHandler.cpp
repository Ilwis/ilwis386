#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "GNCThreddsHandler.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\Downloader.h"
#include "Engine\Base\DataObjects\RemoteXMLObject.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "EOToolbox\EOtoolbox.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\GeoStatSat.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Scripting\Script.h"
#include "netcdf.h"
#include "IlwisNetCdf.h"
#include <regex> 

long ILWIS::GNCThreddsHandler::threddsFolderCount = 0;

using namespace ILWIS;
using namespace std::tr1;

RequestHandler *GNCThreddsHandler::createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv) {
	return new GNCThreddsHandler(c, request_info,kvps, serv);
}

GNCThreddsHandler::GNCThreddsHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& _kvps, IlwisServer *serv)
: RequestHandler("GNCThreddsHandler", c,ri,_kvps, serv)
{
	config.add("Extensions\\EO-toolbox", "EORequestHandler");
}

bool GNCThreddsHandler::doCommand() {
	createExecutionEnviroment();
	String product = kvps["product"];
	String time = kvps["time"];
	executeGNCCommand(product + " " + time);
	FilenameIter itr("*.mpr");
	while(itr.fValid()) {
		FileName fn((*itr));
		Map mp(fn);
		if ( mp->cs()->pcsProjection() ){
			fn = resampleToLatLonWGS84(mp);
		}
		FileName fnOut(fn, ".nc");
		IlwisNetCdf ncd;
		ncd.addMap(Map(fn));
		ncd.saveToNetCdf(fnOut);
		++itr;
	}
	return true;

}

FileName GNCThreddsHandler::resampleToLatLonWGS84(const Map& mp) {
	CoordBounds cb = mp->cb();
	CoordSystem cs("LatlonWGS84");
	FileName fnGrf(mp->fnObj, ".grf");
	LatLon llMiddle = mp->cs()->llConv(cb.middle());
	LatLon llMax(55.0,min(180.0, llMiddle.Lon + 90.0)) ;
	LatLon llMin(-55.0,max(-180, llMiddle.Lon - 90.0));
	GeoRefCorners *grc = new GeoRefCorners(fnGrf, cs, mp->rcSize(), false, Coord(llMin.Lon, llMin.Lat), Coord(llMax.Lon, llMin.Lat));
	GeoRef grf;
	grf.SetPointer(grc);
	grf->Store();
	FileName fnMap(FileName::fnUnique(mp->fnObj));
	String expr("%S := MapResample(%S,%S,bicubic);",fnMap.sRelative(), mp->fnObj.sRelative(), fnGrf.sRelative());
	getEngine()->Execute(expr);

	return fnMap;

}
void GNCThreddsHandler::writeResponse() const{
	ILWIS::XMLDocument doc;
	pugi::xml_node first = doc.addNodeTo(doc,"GNC_Catalog");


	String txt = doc.toString();
	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.c_str(), txt.size());
	mg_write(getConnection(), buf, txt.size()+1);
	delete [] buf;
}

bool GNCThreddsHandler::needsResponse() const {
	return true;
}

void GNCThreddsHandler::createExecutionEnviroment() {
	String rootDir = getConfigValue("GNC:ServiceContext:LocalRoot");
	if ( rootDir == sUNDEF)
		throw ErrorObject(TR("Missing entry LocalRoot in server configuration"));

	executionDir = String("%S\\process_%d", rootDir, threddsFolderCount++);
	_mkdir(executionDir.c_str());
	//setConfigValue("GNC:Service:Context",String("%d",threddsFolderCount);
	getEngine()->SetCurDir(executionDir);
}