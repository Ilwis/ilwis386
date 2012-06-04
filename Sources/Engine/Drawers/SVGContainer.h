#pragma once


namespace ILWIS {

	class IVGElement {
	public:
		IVGElement(const FileName& fn) ;
		
	};

	class SVGContainer : public map<String, IVGElement *> {
	public:
		SVGContainer();
		~SVGContainer();
		void load();
	private:
		void parseFile(const FileName& fn);

	};

}