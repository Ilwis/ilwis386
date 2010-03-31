#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"

int LayerSection::parse(vector<SimpleSection> elements, int current) {
	int currentElement = current;

	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		ComplexSection section = null;
		if (whatFunctionality(el) == ElementType.nfEND) {
			break;
		}else if (el.getName() == "CLASS") {
			section = new ClassSection();
		} else if (el.getName() == "METADATA") {
			section = new MetaDataSection();
		} else if (el.getName() == "PROJECTION") {
			section = new ProjectionSection();
		}else if (whatFunctionality(el) == ElementType::nfKEYVALUE) {
			nameValuePairs[el.getName()] =  el;
		}
		
		if (section != null )
		{
			currentElement = section.parse(elements, ++currentElement);
			sections.push_back(section);
		}

		++currentElement;

	}
	return currentElement;
}

public String toString() {
	String section = "LAYER\n";
	section += ComplextSection::toString();
	return section + "END\n" ;
}


