#pragma once

namespace ILWIS {
	class IVGElement ;

	class _export SVGLoader : public map<String, IVGElement *> {
	public:
		SVGLoader();
		~SVGLoader();
		void load(const String& folder="") ;
		IVGElement *getSVGSymbol(const String& name);
	private:
		String getAttributeValue(const pugi::xml_node& n, const String& key) const;
		IVGElement *parseFile(const FileName& fn);
		static map<String,Color> svgcolors;
	};

}