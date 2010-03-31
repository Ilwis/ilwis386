#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"

	
int PointSection::parse(ArrayList<SimpleSection> elements, int current) {
	int currentElement = current;

	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		if (whatFunctionality(el) == ElementType::nfEND) {
			break;
		} else if (whatFunctionality(el) == ElementType::nfKEYVALUE)
			points.add(el.toString());

		++currentElement;

	}

	return currentElement;
}

public PointSection::String toString() {
	String section = "POINTS\n";
	section += ComplexSection::toString();
	for(int i = 0; i < points.size(); ++i) {
		String point = points[i];
		section += point + "\n";
	}
	return section + "END\n";
}

