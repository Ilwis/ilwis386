
// Engine
#include "Headers\toolspch.h"
#include "Engine\Base\System\Appcont.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\DataExchange\ForeignFormatMap.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\Table\Col.h"
#include "Engine\DataExchange\hdfincludes\hdf.h"
#include "Engine\DataExchange\HDFReader.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\DataExchange\WMSCollection.h"
#include "Engine\Base\System\commandhandler.h"
#include "Engine\DataExchange\gdalproxy.h"
#include "Engine\DataExchange\curlincludes\curlproxy.h"
#include <set>
#include <fstream>

ModuleMap Engine::modules = ModuleMap();
ForeignFormatMap Engine::formats = ForeignFormatMap();
HMODULE Engine::engineHandle = 0;
Engine *Engine::engine = 0;

BOOL WINAPI DllMain(
  __in  HINSTANCE hinstDLL,
  __in  DWORD fdwReason,
  __in  LPVOID lpvReserved
  ) 
{
	if ( fdwReason == DLL_PROCESS_ATTACH) {
		Engine::engineHandle = hinstDLL;
	}
	return TRUE;
}

// Engine construction

Engine* getEngine() {
	if ( Engine::engine == 0) {
		Engine::engine = new Engine();
		Engine::engine->getContext()->ComHandler()->init();
		Engine::engine->modules.addModules();
		Engine::engine->formats.AddFormats();
		Engine::engine->modules.initModules();

	}
	return Engine::engine;
}

Engine::Engine() : stayResident(true),debugMode(false)
{
	context = 0;
	logger = NULL;
	version = 0;
	Init();
}

Engine::~Engine()
{
	for(ModuleIter iter= modules.begin(); iter != modules.end(); ++iter)
		delete (*iter).second;
	
	delete context;
	context = NULL;
	delete logger;
	logger = NULL;
	delete version;
	version = NULL;
	delete db;
	delete gdal;
	delete curl;
}

// Engine initialization


void Engine::Init() {
	if (context == 0) {

		context = new IlwisAppContext(getModuleHandle());
		version = new ILWIS::Version();
		version->addBinaryVersion(ILWIS::Version::bvFORMAT20);
		version->addBinaryVersion(ILWIS::Version::bvFORMAT30);
		version->addBinaryVersion(ILWIS::Version::bvFORMATFOREIGN);
		version->addBinaryVersion(ILWIS::Version::bvPOLYGONFORMAT37);
		version->addODFVersion("3.1");
		version->addModuleInterfaceVersion(ILWIS::Module::mi36);
		version->addModuleInterfaceVersion(ILWIS::Module::mi37);
		version->addModuleInterfaceVersion(ILWIS::Module::mi38);
		String ilwDir = context->sIlwDir();
		context->InitThreadLocalVars();

		db = ILWIS::Database::create("spatiallite",":memory:","System");
		String stmt = "create table Datums \
					  (\
					  name TEXT,\
					  code TEXT, \
					  area TEXT, \
					  ellipsoid TEXT, \
					  dx REAL, \
					  dy REAL, \
					  dz REAL, \
					  rx REAL, \
					  ry REAL, \
					  rz REAL,\
					  ds REAL,\
					  description TEXT \
					  )";
	  db->executeStatement(stmt);
	  stmt = "create table DatumAliasses \
					  (\
					  name TEXT,\
					  alias TEXT)";
	  db->executeStatement(stmt);
	  loadSystemTables(ilwDir);
	  loadServiceLocations(ilwDir);

	  gdal = new GdalProxy();
	  gdal->loadMethods(ilwDir);

	  curl = new CurlProxy();
	  curl->loadMethods(ilwDir);
	}
}

void Engine::loadServiceLocations(const String& dir) {
	String iniFile = dir + "services.ini";
	char buffer[5000];
	UINT n = GetPrivateProfileInt("Services","count",0,iniFile.c_str());
	for(int i = 0; i < n; ++i) {
		String name("service%d",i);
		GetPrivateProfileString("Services", name.c_str(),"",buffer,5000,iniFile.c_str());
		if ( buffer[0] == 0) 
			continue;
		String serviceType(buffer);
		String section(buffer);
		DWORD nchars = GetPrivateProfileSection(section.c_str(),buffer,5000,iniFile.c_str());
		String key, value;
		bool iskey = true;
		for(int j=0; j < nchars; ++j) {
			char c = buffer[j];
			if ( c == '=' && iskey) {
				iskey = false;
			} else if ( c == 0 ) {
				if (key.size() != 0 && value.size() != 0) {
					serviceLocations[serviceType + "::" + key] = value;
				}else {
					continue;
					iskey = true;
				}

			} else if ( c== '\r' || c == '\n') {
				iskey = true;
				continue;
			}
			if ( iskey) {
				key += c;

			} else {
				value += c;
			}

		}
	}
}

void Engine::getServicesFor(const String& serviceType, map<String,String>& services, bool getUrl) const{
	for(map<String,String>::const_iterator cur = serviceLocations.begin(); cur != serviceLocations.end(); ++cur) {
		String key = (*cur).first;
		if ( key.find(serviceType + "::") !=  string::npos) {
			if ( getUrl)
				services[serviceType.sTail("::")] = (*cur).second;
			else
				services[serviceType.sTail("::")] = (*cur).first;

		}
	}
}

#define NORM(n) ((parts[n].size() == 0 || parts[n].rVal() == rUNDEF) ? 0 : parts[n].rVal())

void split(const String& in, const char delim, vector<String>& parts) {
	string current;
	for(unsigned int i = 0; i < in.size(); ++i) {
		char c = in[i];
		if ( c == delim) {
			parts.push_back(current);
			current.clear();
		} else {
			current += c;
		}
	}
	parts.push_back(current);
}

void Engine::loadSystemTables(const String& ilwDir) {

	ifstream in(String("%SResources\\def\\datum.csv",ilwDir).c_str());
	if (!in.is_open()) {
		throw ErrorObject(TR(String("Can't open datum.csv %S")).c_str(),ilwDir);
	}
	bool skip = true;
	String pp("is open %d, eof %d", (int)in.is_open(), (int)in.eof());
	while(in.is_open() && !in.eof()) {
		String line;
		getline(in,line);
		if ( skip || line.size() == 0) {
			skip = false; // first line is headers, ignore
			continue;
		}
		vector<String> parts;
		split(line,',',parts);
		double dx = NORM(4);
		double dy = NORM(5);
		double dz = NORM(6);
		double rx = NORM(7);
		double ry = NORM(8);
		double rz = NORM(9);
		double ds = NORM(10);

		String plist("'%S','%S','%S','%S',%f,%f,%f,%f,%f,%f,%f,'%S'",parts[0],parts[1],parts[2],parts[3],dx,dy,dz,rx,ry,rz,ds,parts[12]);
		String stmt("INSERT INTO Datums VALUES(%S)", plist);
		bool res = db->executeStatement(stmt);
		if ( parts[11] != "") {
			Array<String> aliassen;
			Split(parts[11],aliassen,";");
			for(int j = 0; j < aliassen.size(); ++j) {
				stmt = String("INSERT INTO DatumAliasses Values('%S','%S')",parts[0],aliassen[j]);
				db->executeStatement(stmt);
			}
		}
	}
	in.close();

	ifstream epsgFile(String("%SResources\\def\\epsg-sqlite.sql",ilwDir).c_str());
	String stmt;
   // getline(in,stmt);  // Get the frist line from the file, if any.
    while ( epsgFile ) {
		String str;
        getline(epsgFile,str); 
		stmt += str;
    }
	db->executeStatement(stmt);

	epsgFile.close();
}

void Engine::SetCurDir(const String& sDir) {
	if ( context)
		context->SetCurDir(sDir);
}
String Engine::sGetCurDir() const
{
	if ( context)
		return context->sGetCurDir();
	return "";
}

void *Engine::pGetThreadLocalVar(IlwisAppContext::ThreadLocalVars tvType) {
	if ( context)
		return context->pGetThreadLocalVar(tvType);
	return NULL;
}

void Engine::SetThreadLocalVar(IlwisAppContext::ThreadLocalVars tvType, void *var) {
	if ( context)
		context->SetThreadLocalVar(tvType, var);

}
void Engine::InitThreadLocalVars() {
	if ( context)
		context->InitThreadLocalVars();
}

void Engine::RemoveThreadLocalVars() {
	if ( context)
		context->RemoveThreadLocalVars();
}

void Engine::Execute(const String& command) {
	if ( context)
		context->Execute(command);
}

void Engine::RemoveSearchPath(const FileName& fn)
{
	if ( context)
		context->RemoveSearchPath(fn);
}			

void Engine::AddSearchPath(const FileName& fn)
{
	if ( context)
		context->AddSearchPath(fn);	    
}

FileName Engine::fnGetSearchPath(int iIndex)
{
	if ( context)
		return context->fnGetSearchPath(iIndex);
	return FileName();
}

IlwisAppContext * Engine::getContext() {
	return context;
}

void Engine::PostMessage(UINT msg, WPARAM p1, LPARAM p2) 
{ 
	CWinApp *app = AfxGetApp();
	CWnd *wnd = app->GetMainWnd();
	if ( wnd != NULL && ::IsWindow(wnd->m_hWnd))
		wnd->PostMessage(msg, p1, p2); 
}

LRESULT Engine::SendMessage(UINT msg, WPARAM p1, LPARAM p2) 
{ 
	CWinApp *app = AfxGetApp();
	CWnd *wnd = app->GetMainWnd();
	if ( wnd != NULL && ::IsWindow(wnd->m_hWnd))
		return wnd->SendMessage(msg, p1, p2);
	return 0;
}

bool Engine::hasClient() {
	CWinApp *app = AfxGetApp();
	CWnd *wnd = app->GetMainWnd();
	return wnd != NULL;
}

bool Engine::fStayResident() {
	return stayResident;
}

void Engine::setStayResident(bool stay) {
	stayResident = stay;
}

bool Engine::getDebugMode(char *s) {
	if( s!= NULL && debugClasses.size() > 0) {
		return false;
	}
	return debugMode;
}

void Engine::setDebugMode(bool debug, const String& classes){
	Split(classes,debugClasses,",");
	debugMode = debug;
	
}

Logger *Engine::getLogger() {
	if (!logger)
		logger = new Logger();
	return logger;
}

ILWIS::Version *Engine::getVersion() {
	return version;
}

ILWIS::Module * Engine::getModule(const String& name){
	if ( modules.find(name) != modules.end())
		return modules[name];
	return NULL;
}

ILWIS::Module * Engine::getModule(int index){
	if ( index < 0 || index >= modules.size())
		return NULL;
	int i =0;
	for ( ModuleIter iter = modules.begin(); iter != modules.end(); ++iter)
		if ( i++ == index)
			return (*iter).second;
	return NULL;
}

void Engine::addModule(ILWIS::Module *m) {
	if ( m->getName() == "")
		throw ErrorObject("Improper module definition : No name");
	if ( getModule(m->getName()) == NULL) {
		modules[m->getName()] = m;
	}
}

bool Engine::fServerMode() const{
	bool *fServerMode = (bool*)context->pGetThreadLocalVar(IlwisAppContext::tlvSERVERMODE);
	if( fServerMode != 0 && *fServerMode)
		return true;
	return false;

}

ILWIS::Database *Engine::pdb() {
	return db;
}
