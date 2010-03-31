#include "Headers\toolspch.h"
#include "Engine\Base\System\module.h"

ILWIS::Module::Module(const String& _name, const FileName& _fnModule, ModuleInterface ivers, const String& vers) : 
	fnModule(_fnModule), 
	version(vers), 
	name(_name), 
	interfaceVersion(ivers){
}


void ILWIS::Module::addMethod(InterfaceFunctions func, void * p) {
	interfaceFunctions[func] = p;
}

void * ILWIS::Module::getMethod(InterfaceFunctions func) {
	map<InterfaceFunctions, void *>::iterator cur = interfaceFunctions.find(func);
	if ( cur != interfaceFunctions.end())
		return (*cur).second;
	return NULL;
}

ILWIS::Module::ModuleInterface ILWIS::Module::getInterfaceVersion() const {
	return interfaceVersion;
}

String ILWIS::Module::getVersion() const {
	return version;
}

String ILWIS::Module::getName() const {
	return name;
}

String ILWIS::Module::getNameAndVersion() const {
	return name + " " + version;
}
