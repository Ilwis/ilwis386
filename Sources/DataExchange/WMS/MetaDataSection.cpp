#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"

import java.util.ArrayList;

int MetaDataSection::parse(ArrayList<SimpleSection> elements, int current) {
	int currentElement = current;

	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		if (whatFunctionality(el) == ElementType::nfEND) {
			break;
		} else if (whatFunctionality(el) == ElementType::nfKEYVALUE) {
			nameValuePairs[el.getName()] = el;
		}

		++currentElement;

	}
	return currentElement;
}

String MetaDataSection::toString() {
	String section = "METADATA\n";
	section += ComplexSection::toString();
	return section + "END\n";
}

