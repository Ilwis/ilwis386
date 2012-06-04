#include "Headers\toolspch.h"
#include "Engine\base\system\engine.h"
#include <map>
#include "Engine\Drawers\SVGContainer.h"


using namespace ILWIS;


IVGElement::IVGElement(const FileName& fn) {

}

//-----------------------------
SVGContainer::SVGContainer() {
}
SVGContainer::~SVGContainer() {
}
void SVGContainer::load() {
	String ilwDir = getEngine()->getContext()->sIlwDir();
	String pathToSvg = ilwDir + "\\Resources\\symbols\\*.svg";


		CFileFind finder;
		BOOL fFound = finder.FindFile(pathToSvg.c_str());
		while(fFound) {
			fFound = finder.FindNextFile();
			if (fFound)
			{
				FileName fnSvg(finder.GetFilePath());
				parseFile(fnSvg);
			}
		}

}

void SVGContainer::parseFile(const FileName& fn) {

}
