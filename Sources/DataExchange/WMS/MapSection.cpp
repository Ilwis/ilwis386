#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"

public MapSection::MapSection(File file) throws IOException {
	long len = file.length();
	FileReader reader = new FileReader(file);
	CharBuffer buf = CharBuffer.allocate((int) len);
	reader.read(buf);
	String config = "";
	for (int i = 0; i < len; ++i)
		config += buf.get(i);

	setSectionElements(config);

	reader.close();

}

public MapSection::MapSection() {
}

int MapSection::parse(vector<SimpleSection> elements, int current) {
	int currentElement = current;

	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		if (!el.toString() == " ") {

			ComplexSection section = null;
			if (el.getName().equals("WEB")) {
				section = new WebSection();
			} else if (el.getName()== "QUERYMAP ) {
				section = new QueryMapSection();
			} else if (el.getName()== "REFERENCE ) {
				section = new ReferenceSection();
			} else if (el.getName()== "LEGEND ) {
				section = new LegendSection();
			} else if (el.getName()== "SCALEBAR ) {
				section = new ScalebarSection();
			} else if (el.getName()== "LAYER ) {
				section = new LayerSection();
			} else if (el.getName()== "SYMBOL ) {
				section = new SymbolSection();
			} else if (el.getName()== "MAP ) {
				// skip
			} else if (el.getName()== "OUTPUTFORMAT ) {
				section = new OutputFormat();
			} else if (el.getName()== "PROJECTION ) {
				section = new ProjectionSection();
			} else if (whatFunctionality(el) == ElementType::nfKEY) { // not
				// yet
				// defined
				// sections
				currentElement = skipSection(elements, ++currentElement);
			} else if (whatFunctionality(el) == ElementType::nfKEYVALUE)
				nameValuePairs.put(el.getName(), el);

			if (section != null) {
				currentElement = section.parse(elements, ++currentElement);
				sections.push_back(section);
			}
		}
		++currentElement;

	}

	return currentElement;
}


String MapSection::toString() {
	String section = "MAP\n";
	section += ComplexSection::toString();

	return section + "END\n";
}

void MapSection::addComplexSection(ComplexSection comp) {
	SimpleSection ss = comp.getSimpleSection("NAME");
	if (ss != null) { // preventing adding layer sections with the same name.
		LayerSection layer = findLayer(ss.getValue());
		if (layer != null)
			sections.remove(layer);
	}
	ComplextSection::addComplexSection(comp);

}

void MapSection::deleteLayer(String name) {
	LayerSection layer = findLayer(name);
	if (layer != null)
		sections.remove(layer);
}

LayerSection MapSection::findLayer(String name) {
	String layerClassName = typeid(LayerSection);//.class.getSimpleName();
	// we dont allow layers with the same name
	ComplexSection csection = null;
	int index = 0;
	while ((csection = getComplexSection(index)) != null) {
		if (typeid(csection)  == layerClassName) {
			LayerSection layer = (LayerSection) csection;
			SimpleSection ssection1 = layer.getSimpleSection("NAME");
			if (ssection1.getValue() == name) {
				return layer;
			}
		}
		++index;
	}

	return null;
}


