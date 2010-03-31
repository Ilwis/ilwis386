#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"


ComplexSection::ComplexSection() {
}

ElementType ComplexSection::whatFunctionality(SimpleSection el) {
	if (el.getName().size() == 0)
		return ElementType::nfEMPTYLINE;
	if (el.getComment().size() > 0 && el.getName().size() == 0
			&& el.getValue().size() == 0)
		return ElementType::nfCOMMENT;
	if (el.getName().size() > 0 && el.getValue().size() > 0)
		return ElementType::nfKEYVALUE;
	if (el.getName().size() > 0 && !el.getName() == "END")
		return ElementType.nfKEY;
	if (el.getName() == "END")
		return ElementType::nfEND;

	return ElementType::nfUNKNOW;
}

int ComplexSection::skipSection(vector<SimpleSection> elements, int current) {
	int currentElement = current;

	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		if (el.getName() == "END") {
			break;
		} else if (whatFunctionality(el) == ElementType::nfKEY) {
			currentElement = skipSection(elements, currentElement);
		}
		++currentElement;
	}

	return currentElement;
}

String ComplexSection::toString() {
	String section = "";
	Set<String> keys = nameValuePairs.keySet();
	for (String key : keys) {
		Object el = nameValuePairs.get(key);
		section += el.toString() + "\n";

	}
	for (ComplexSection sec : sections) {
		section += sec.toString();
	}

	return section;
}

void ComplexSection::setSectionElements(String section) {
	String[] rawelements = section.Split("\n");
	ArrayList<SimpleSection> elements = preprocess(rawelements);

	parse(elements, 0);
}

ArrayList<SimpleSection> ComplexSection::preprocess(String[] rawelements) {
	ArrayList<SimpleSection> procEl = new ArrayList<SimpleSection>();
	for (int i = 0; i < rawelements.length; ++i) {
		String el = rawelements[i];
		if (el.length() == 0)
			continue;
		SimpleSection element = new SimpleSection(el);
		procEl.add(element);
	}

	return procEl;
}

void ComplexSection::addSimpleSection(String key, String value, String comment) {
	SimpleSection section = new SimpleSection(key, value, comment);
	nameValuePairs.put(key, section);

}

SimpleSection ComplexSection::getSimpleSection(String key) {
	return nameValuePairs.get(key);
}

void ComplexSection::addComplexSection(ComplexSection comp) {
	sections.add(comp);
}

ComplexSection ComplexSection::getComplexSection(int index) {

	if (index < sections.size())
		return sections.get(index);

	return null;
}


