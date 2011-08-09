#pragma once

namespace ILWIS {
	class SVGElement ;

	class _export SVGLoader : public map<String, SVGElement *> {
	public:
		SVGLoader();
		~SVGLoader();
		void load(const String& folder="") ;
		SVGElement *getSVGSymbol(const String& name);
	private:
		String getAttributeValue(const pugi::xml_node& n, const String& key) const;
		SVGElement *parseFile(const FileName& fn);
		static map<String,Color> svgcolors;
	};

}