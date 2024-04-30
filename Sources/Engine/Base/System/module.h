#pragma once

namespace ILWIS {

class _export Module {
public:
	enum InterfaceFunctions{ifgetCommandInfo,ifInit,ifgetCommandInfoUI,ifInitUI, ifDrawers,getFFInfo, 
		                    getImpDrivers, getOptionsUI, getFFInit, getOptionsUIInit,ifMapEditors,ifDrawerTools,ifService};
	enum ModuleInterface{mi36,mi37,mi38};
	Module(const String& _name, const FileName& _fnModule, ModuleInterface ivers, const String& vers);
	void addMethod(InterfaceFunctions func, void *p);
	void *getMethod(InterfaceFunctions func);
	ModuleInterface getInterfaceVersion() const;
	String getName() const;
	String getVersion() const;
	String getNameAndVersion() const;
	FileName getLocation() const;

private:
	String name;
	ModuleInterface interfaceVersion;
	String version;
	FileName fnModule;
	map<InterfaceFunctions, void *> interfaceFunctions;

};

typedef ILWIS::Module::ModuleInterface (*ModuleInterfaceVersion)();
};

typedef ILWIS::Module *(*ModuleInfo)(const FileName& fnModule);