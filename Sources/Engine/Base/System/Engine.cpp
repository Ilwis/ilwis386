
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
#include <set>

ModuleMap Engine::modules = ModuleMap();
ForeignFormatMap Engine::formats = ForeignFormatMap();

// Engine construction

Engine *engine = new Engine();

Engine* getEngine() {
	return engine;
}

Engine::Engine() : stayResident(true),debugMode(false)
{
	context = 0;
	logger = NULL;
	version = 0;
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
}

// Engine initialization


void Engine::Init(const String& prog, const String& sCmdLn) {
	context = new IlwisAppContext(prog, sCmdLn);
	version = new ILWIS::Version();
//	version->addBinaryVersion(ILWIS::Version::bvFORMAT20);
	version->addBinaryVersion(ILWIS::Version::bvFORMAT30);
	version->addBinaryVersion(ILWIS::Version::bvFORMATFOREIGN);
	version->addBinaryVersion(ILWIS::Version::bvPOLYGONFORMAT37);
	version->addODFVersion("3.1");
	version->addModuleInterfaceVersion(ILWIS::Module::mi36);
	version->addModuleInterfaceVersion(ILWIS::Module::mi37);
	String ilwDir = context->sIlwDir();
	modules.addModules();
    formats.AddFormats();
	modules.initModules();

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

void Engine::Execute(String& command) {
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

