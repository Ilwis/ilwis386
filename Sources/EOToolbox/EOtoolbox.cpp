#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Engine\Base\System\commandhandler.h"
#include "Engine/base/system/engine.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\GeoStatSat.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "HttpServer\IlwisServer.h"
#include "netcdf.h"
#include "IlwisNetCdf.h"
#include "EOToolbox\Services.h"
#include "Engine\Scripting\Script.h"
#include "EOToolbox\EOtoolbox.h"
#include "httpserver\ServiceConfiguration.h"


EOToolbox *GNCObject = new EOToolbox();

String createTimeString(const String& timep, const FormatInfo& info) {
	if ( info.format == "yyyymmddhhmm") {
		ILWIS::Time time(timep);
		String stime("%d%02d%02d",(int)time.get(ILWIS::Time::tpDATE), (int)time.get(ILWIS::Time::tpHOUR), (int)time.get(ILWIS::Time::tpMINUTE));
		return stime;
	}
	if ( info.format == "yyyymmdd") {
		String prt = timep.substr(0,8);
		return prt;
	}
	return "";

}

String removeBeginEndSlash(const String& str) {
	String tmp = str;
	if ( str[0] == '\\')
		tmp = str.substr(1,tmp.size() - 2);
	if ( tmp[tmp.size() - 1] == '\\') {
		tmp = tmp.substr(0,tmp.size() - 1);
	}
	return tmp;
}

FileName resampleToLatLonWGS84(const Map& mp) {
	CoordBounds cb = mp->cb();
	CoordSystem cs("LatlonWGS84");
	FileName fnGrf(mp->fnObj, ".grf");
	LatLon llMiddle = mp->cs()->llConv(cb.middle());
	LatLon llMax(62,min(180.0, llMiddle.Lon + 90.0)) ;
	LatLon llMin(-62, max(-180, llMiddle.Lon - 90.0));
	GeoRefCorners *grc = new GeoRefCorners(fnGrf, cs, mp->rcSize(), false, Coord(llMin.Lon, llMin.Lat), Coord(llMax.Lon, llMax.Lat));
	GeoRef grf;
	grf.SetPointer(grc);
	grf->Store();
	FileName fnMap(FileName::fnUnique(mp->fnObj));
	String expr("%S := MapResample(%S,%S,bicubic);",fnMap.sRelative(), mp->fnObj.sRelative(), fnGrf.sRelative());
	getEngine()->Execute(expr);

	return fnMap;

}

void subMap(const FileName& fnOut, const FileName& fnIn, const CoordBounds& cb) {
	String expr("%S:=MapSubMapCoords(%S,%f,%f,%f,%f);",fnOut.sPhysicalPath(),fnIn.sPhysicalPath()
										,cb.MinX(), cb.MinY(), cb.MaxX(), cb.MaxY());
	getEngine()->Execute(expr);
}

bool convertNetCDF(const CoordBounds& cb) {
	FilenameIter itr("*.mpr");
	while(itr.fValid()) {
		FileName fn((*itr));
		Map mp(fn);
		FileName fnTemp = FileName::fnUnique(mp->fnObj);
		if ( mp->cs()->pcsProjection() ){
			if ( cb.fValid()) {
				CoordSystem csySource("LatlonWGS84.csy");
				CoordBounds cbPrj = mp->cs()->cbConv(csySource,cb);
				subMap(fnTemp, mp->fnObj,cbPrj);
				fn = fnTemp;
				mp = Map(fn);
			} 
			fn = resampleToLatLonWGS84(mp);
		}  else{
			if ( cb.fValid()) {
				subMap(fnTemp, mp->fnObj, cb);
				fn = fnTemp;
			}
		}
		FileName fnOut(fn, ".nc");
		IlwisNetCdf ncd;
		ncd.addMap(Map(fn));
		ncd.saveToNetCdf(fnOut);
		++itr;
	}
	FilenameIter itr2("*.*");
	while(itr2.fValid()) {
		FileName fn((*itr2));
		if ( fn.sExt != ".nc") {
			fn.fDelete();
		}
		++itr2;
	}
	return true;
}

void createResponseFile(const String& server, const String& outputPath, const String& output) {
	FileName fnOut(output);
	FileName fnTxt(fnOut,".txt");
	ofstream outfile(fnTxt.sFullPath().c_str());

	ILWIS::ServiceConfiguration config("Extensions\\EO-Toolbox","EORequestHandler");
	String serverUrl = config.get("GNC:ServiceContext:" + server);
	Array<String> parts;
	Split(outputPath, parts,"\\");
	String hdr = outputPath;
	if ( parts.size() > 0) {
		hdr = parts[parts.size()-1];
	}
	serverUrl += "/" + hdr;
	FilenameIter iter("*.nc");
	while(iter.fValid()) {
		FileName fn((*iter));
		String url = serverUrl +  "/" + fn.sFile + fn.sExt;
		outfile << url << "\n";
		++iter;
	}
	outfile.close();

}

void executeGNCCommand(const String& cmd) {
	Array<String> parts;
	String outname;
	CoordBounds cb;
	Split(cmd, parts," ");
	String id = parts[0];
	FormatInfo info = GNCObject->get(id);
	String time = createTimeString(parts[1], info);

	bool thredds = false;
	int iParms  = parts.size();
	for(int i = 0; i < parts.size(); ++i) {
		if (parts[i] == "thredds") {
			thredds = true;
			break;
		}
	}
	if (iParms == 3 && !thredds) {
		outname = parts[2];
	} else if ( iParms == 4) {
		outname = parts[3];
	} else if ( iParms >= 6 ) {
		Coord c1(parts[2].rVal(), parts[3].rVal());
		Coord c2(parts[4].rVal(), parts[5].rVal());
		cb = CoordBounds(c1, c2);
	} else if ( iParms == 7 && !thredds) {
		outname = parts[6];
	}
	if ( iParms == 8){
		outname = parts[6];

	} /*else {
		throw ErrorObject(TR("Illegal geonetcast expression"));
	}*/

	FileName fnInput(info.fnInput);
	String inputDrive = fnInput.sDrive;
	String inputPath = removeBeginEndSlash(fnInput.sDir);
	FileName fnOutput(getEngine()->sGetCurDir());
	String outputDrive = fnOutput.sDrive;
	String outputPath = removeBeginEndSlash(fnOutput.sDir);

	String scriptPath = GNCObject->pluginDir() + info.command;
	String ilwdir = getEngine()->getContext()->sIlwDir();
	ilwdir = ilwdir.substr(0, ilwdir.length() - 1);
	FileName fnScript(scriptPath);
	String batCmd = String("run %S %S %S %S %S %S %S %S %S", 
					fnScript.sFullPathQuoted(),
					time,
					inputDrive,
					inputPath,
					outputDrive,
					outputPath,
					GNCObject->gdalLocation(),
					ilwdir,
					GNCObject->utilLocation()
					);
	//getEngine()->Execute(batCmd);
	Script::Exec(batCmd);
	if ( thredds) {
		convertNetCDF(cb);
		createResponseFile(parts[2], fnOutput.sPath(), outname);
	} else {
		if ( cb.fValid()) {
			FilenameIter itr("*.mpr");
			vector<FileName> tobeignored;
			while(itr.fValid()) {
				FileName fn((*itr));
				Map mp(fn);
					FileName fnTemp = FileName::fnUnique(mp->fnObj);
				if ( mp->cs()->pcsProjection()){
					CoordSystem csySource("LatlonWGS84.csy");
					CoordBounds cbPrj = mp->cs()->cbConv(csySource,cb);
					subMap(fnTemp, mp->fnObj,cbPrj);
				}  else{
					subMap(fnTemp, mp->fnObj, cb);
				}
				++itr;
			}
		}
	}
}



String wpsmetadataGeonetcast() {
	WPSMetaData metadata("Geonetcast");

	return metadata.toString();
}

ApplicationMetadata metadataGeonetcast(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataGeonetcast();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotANY;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression = "geonetcast <product> <iso time> [<server type>]";

	return md;
}

extern "C" _export void moduleInitUI(ILWIS::Module *module) {
	getEngine()->getContext()->ComHandler()->AddCommand("geonetcast",executeGNCCommand, metadataGeonetcast);
}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("EO-Toolbox", fnModule,ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifInit, (void *)moduleInitUI);
	module->addMethod(ILWIS::Module::ifService, (void *)serviceInfo);  

	String ilwDir = getEngine()->getContext()->sIlwDir();
	String formType="EO-Toolbox";
	FileName plugin(ilwDir + "Extensions\\EO-Toolbox\\config.xml");
	GNCObject->ReadConfigFile(plugin);

	return module;
}

//-------------------------------------
EOToolbox::EOToolbox() {

}

void EOToolbox::ReadConfigFile(FileName fnConfig) {
	if ( fnConfig.fExist() == false)
		return;

	ILWIS::XMLDocument doc(fnConfig);

		int count = 0;
		for(pugi::xml_node child = doc.first_child(); child; child = child.next_sibling()) {
			build(child,count,""); 
		}
		String ilwDir = getEngine()->getContext()->sIlwDir();
		String formType="EO-Toolbox";
		String pluginDir = ilwDir + "Extensions\\EO-Toolbox";
		String tempName = formType;
		FileName fnIni(pluginDir+ "\\geonetcast.ini");
		if ( fnIni.fExist()) {
			for(map<String, FormatInfo>::iterator cur = formats.begin(); cur != formats.end(); ++cur) {
				String folderId = (*cur).second.folderId;
				if ( folderId == "") 
					continue;
				char name[255];
				GetPrivateProfileString(folderId.c_str(), "InputFolder", 0,
                                     name, 255, fnIni.sFullPath().c_str());
				String tmp(name);
				if ( tmp[tmp.size() - 1] != '\\')
					tmp += "\\";
				(*cur).second.fnInput = FileName(tmp);
				//ObjectInfo::ReadElement(folderId.c_str(), "InputFolder",fnIni, name);
				//ObjectInfo::ReadElement(folderId.c_str(), "OutputFolder",fnIni, (*cur).second.fnOutput);
				GetPrivateProfileString(folderId.c_str(), "OutputFolder", 0,
                                     name, 255, fnIni.sFullPath().c_str());
				tmp = String(name);
				if ( tmp[tmp.size() - 1] != '\\')
					tmp += "\\";
				(*cur).second.fnOutput = FileName(tmp);
			}
		}
			
	
}

void EOToolbox::build(pugi::xml_node node, int& count, const String& idPath) {
	pugi::xml_node_type type = node.type();
	if ( type == pugi::node_element) {
		String nodeName = node.name();
		FormatInfo info;
		String id;
		id = idPath == "" ? node.attribute("id").value() : String("%S:%s", idPath, node.attribute("id").value());
		String name(node.attribute("name").value());
		if ( nodeName == "Product") {
			info.folderId= node.attribute("folderid").value();
			info.command =  node.attribute("script").value();
			info.format = node.attribute("format").value();
			info.type = name;
			info.filePattern = node.attribute("filepattern").value();
			info.id = id;

			//String loc = makeLocator(info.command);
			if ( id != "") {
				formats[id] = info;
			}

			++count;
		} else {
				
		}


		for(pugi::xml_node child = node.first_child(); child; child = child.next_sibling()) {
			build(child,count, id); 
		}
	} 
}

String EOToolbox::makeLocator(const String& path) {
	Array<String> parts;
	Split(path, parts,"\\");
	if ( parts.size() > 1) {
		String temp =  parts[parts.size() - 2] + ":" + parts[parts.size() - 1];
		int index = temp.find_last_of(".");
		if ( index !=  string::npos)
			temp = temp.substr(0, index);
		return temp.toLower();
	}
	return "";
}

String EOToolbox::gdalLocation() const {
	return pluginDir() + "GDAL\\bin";

}

String EOToolbox::utilLocation() const{
	return  pluginDir() + "util";
}

String EOToolbox::pluginDir() const {
	String ilwDir = getEngine()->getContext()->sIlwDir();
	return ilwDir + "Extensions\\EO-Toolbox\\";
}

FormatInfo EOToolbox::get(const String& id) {
	String tmp = id;
	return formats[tmp.toLower()];
}


