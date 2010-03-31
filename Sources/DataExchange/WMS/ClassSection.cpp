#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"

int ClassSection::parse(vector<SimpleSection> elements, int current) {
	int currentElement = current;

	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		ComplexSection section = null;
		if (whatFunctionality(elements[currentElement]) == ElementType::nfEND) {
			break;
		} else if (el.getName() == "LABEL") {
			section = new LabelSection();
		} else if (el.getName() == "STYLE") {
			section = new StyleSection();
		} else if (whatFunctionality(el) == ElementType::nfKEYVALUE) {
			nameValuePairs[el.getName()] = el;
		}
		
		if (section != null )
		{
			currentElement = section.parse(elements, ++currentElement);
			sections.add(section);
		}

		++currentElement;

	}
	return currentElement;
}

public String toString() {
	String section = "CLASS\n";
	section += ComplexSection::toString();
	return section += "END\n";
}

}
