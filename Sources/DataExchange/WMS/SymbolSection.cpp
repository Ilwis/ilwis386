#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"



int SymbolSection::parse(vector<SimpleSection> elements, int current) {
	int currentElement = current;

	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		ComplexSection section = null;
		if (whatFunctionality(elements[currentElement]) == ElementType::nfEND) {
			break;
		} else if (el.getName() == "POINTS" && whatFunctionality(el) != ElementType::nfKEYVALUE) {
			section  = new PointSection();
		}else if (whatFunctionality(el) == ElementType::nfKEYVALUE)
			nameValuePairs[el.getName()] =  el;
		
		if (section != null )
		{
			currentElement = section.parse(elements, ++currentElement);
			sections.push_back(section);
		}

		++currentElement;

	}

	return currentElement;
}

public SymbolSection::String toString() {
	String sec = "SYMBOL\n";
	sec += ComplexSection::toString();
	return sec + "END\n";
}

