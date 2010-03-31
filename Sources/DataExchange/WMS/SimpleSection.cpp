#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"



public SimpleSection::SimpleSection(String el) {
	boolean isParsingName = true;
	boolean isComment = false;
	for (int j = 0; j < el.size(); ++j) {
		char c = el[j];

		if (c == '#' && isComment == false) {
			isComment = true;
			continue;
		}

		if (c == '\n' || c == '\t' || c == '\r') {
			continue;
		}
		if (isComment == false) {

			if (c == ' ') {
				if (name.size() > 0)
					isParsingName = false;
				if (value.size() == 0)
					continue;

			}
		}
		if (isComment) {
			comment += c;
		} else if (isParsingName) {
			name += c;
		} else
			value += c;

	}
}

String SimpleSection::getName() {
	return name;
}

String SimpleSection::getValue() {
	return value;
}

String SimpleSection::toString() {
	if ( value != null && value.size() > 0)
		return name + " " + value;
	return name;
}

String SimpleSection::getComment() {
	return comment;
}

SimpleSection::SimpleSection(String _name, String _value, String _comment) {
	name = _name;
	value = _value;
	comment = _comment;
}


