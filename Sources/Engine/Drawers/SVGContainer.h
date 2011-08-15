#pragma once


namespace ILWIS {

	class SVGElement {
	public:
		SVGElement(const FileName& fn) ;
		
	};

	class SVGContainer : public map<String, SVGElement *> {
	public:
		SVGContainer();
		~SVGContainer();
		void load();
	private:
		void parseFile(const FileName& fn);

	};

}