#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "Engine\base\system\engine.h"
#include <map>
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SVGLoader.h"
#include "Engine\Drawers\SVGElements.h"


using namespace ILWIS;


//-----------------------------
SVGLoader::SVGLoader() {
	IVGElement *element = new IVGElement(IVGAttributes::sRECTANGLE,"rectangle");
	(*this)["rectangle"] = element;
	element = new IVGElement(IVGAttributes::sELLIPSE,"ellipse");
	(*this)["ellipse"] = element;
	element = new IVGElement(IVGAttributes::sCIRCLE,"circle");
	(*this)["circle"] = element;

}
SVGLoader::~SVGLoader() {
}
void SVGLoader::load(const String& folder) {
	String pathToSvg;
	if ( folder == "") {
		String ilwDir = getEngine()->getContext()->sIlwDir();
		pathToSvg = ilwDir + "\\Resources\\symbols\\*.*";
	} else
		pathToSvg = folder;

	CFileFind finder;
	BOOL fFound = finder.FindFile(pathToSvg.c_str());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (finder.IsDirectory())
		{
			FileName fnFolder(finder.GetFilePath());
			if ( fnFolder.sFile != "." && fnFolder.sFile != ".." && fnFolder.sFile != "")
				load(fnFolder.sFullPath());
		}
		else {
			FileName fnSvg(finder.GetFilePath());
			String ext = fnSvg.sExt;
			if ( ext.toLower() == ".ivg" || ext.toLower() == ".ivh" || ext.toLower() == ".ivn")
				parseFile(fnSvg);
		}
	}
}

IVGElement *SVGLoader::parseFile(const FileName& fn) {
	if ( !fn.fExist())
		return 0;

	ILWIS::XMLDocument doc(fn);

	String id = fn.sFile;
	IVGElement *element = new IVGElement(id);
	element->parse(doc.first_child());
	(*this)[id] = element;

	return element;
}

String SVGLoader::getAttributeValue(const pugi::xml_node& node, const String& key) const{
	if ( node.attributes_begin() == node.attributes_end())
		return "";
	pugi:: xml_attribute attr = node.attribute(key.c_str());
	if ( attr.empty())
		return "";
	return attr.value();
}

IVGElement *SVGLoader::getSVGSymbol(const String& name) {
	FileName fn(name);
	String shortName = fn.sFile;
	map<String, IVGElement *>::const_iterator iter = find(shortName);
	if ( iter != end())
		return (*iter).second;
	if ( name.sHead("\\") == "" && name.sHead("/") == "") {
		String ilwDir = getEngine()->getContext()->sIlwDir();
		String pathToSvg = ilwDir + "\\Resources\\symbols\\";
		fn  = FileName(pathToSvg + name, ".ivg");
	} else {
		fn = FileName(name, ".ivg");
	}
	IVGElement *el =  parseFile(fn);
	if ( el != 0)
		(*this)[shortName] = el;
	return el;
}


