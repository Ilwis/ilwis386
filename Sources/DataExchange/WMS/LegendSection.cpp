#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"

int LegendSection::parse(ArrayList<SimpleSection> elements, int current) {
	int currentElement = current;

	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		ComplexSection section;
		if (whatFunctionality(el) == ElementType::nfEND) {
			break;
		} else if (el.getName() == "LABEL") {
			section = new LabelSection();
		} else if (whatFunctionality(el) == ElementType::nfKEYVALUE) {
			nameValuePairs[el.getName()] == el;
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
	String section = "LEGEND\n";
	section += ComplexSection::toString();
	return section + "END\n";
}

}
