#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"

int ProjectionSection::parse(vector<SimpleSection> elements, int current) {
	int currentElement = current;
	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		if (whatFunctionality(el) == ElementType::nfEND) {
			break;
		} else if (whatFunctionality(el) == ElementType::nfKEYVALUE) {
			nameValuePairs.put(el.getName(), el);
		} else if ( whatFunctionality(el) == ElementType::nfKEY) {
			nameValuePairs[el.getName()] = el;
		}

		++currentElement;

	}
	return currentElement;
}
	
String ProjectionSection::toString() {
	String section = "PROJECTION\n";
	section += ComplextSection::toString();
	return section + "END\n";
}

}
