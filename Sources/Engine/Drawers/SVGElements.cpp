#include "Headers\toolspch.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include "Engine\base\system\engine.h"
#include <map>
#include "Drawer_n.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SVGElements.h"
#include "Engine\Base\Algorithm\TriangulationAlg\gpc.h"
#include "Engine\Base\Algorithm\TriangulationAlg\Triangulator.h"
#include "Engine\Drawers\SVGPath.h"
#include <bitset>
#include <limits>



using namespace ILWIS;

map<String, Color> IVGElement::svgcolors;

IVGElement::IVGElement(const String& _id) : defaultScale(1.0), hatch(0), hatchInverse(0)
{
	if ( svgcolors.size() == 0) {
		initSvgData();
	}
	for(int i = 0; i < size(); ++i)
		delete at(i);
	clear();
	id = _id;
}

IVGElement::IVGElement(IVGAttributes::ShapeType t, const String& _id) : id(_id), defaultScale(1.0), hatch(0), hatchInverse(0) {
	if ( svgcolors.size() == 0) {
		initSvgData();
	}
	if ( t != IVGAttributes::sUNKNOWN) {
		push_back(new IVGAttributes(t));
	}
}

IVGElement::~IVGElement() {
	if ( hatch)
		delete [] hatch;
	if ( hatchInverse)
		delete [] hatchInverse;
}

Color IVGElement::getColor(const String& name) const{
	map<String, Color>::const_iterator cur;
	if (( cur = svgcolors.find(name)) != svgcolors.end()) {
		return (*cur).second;
	} else {
		if ( name.substr(0,3) == "rgb") {
			String col = name.sTail("(").sHead(")");
			Array<String> parts;
			Split(col,parts,",");
			return Color(parts[0].iVal(), parts[1].iVal(), parts[2].iVal());
		}
	}
	return Color(0,0,0);
}

void IVGElement::parse(const pugi::xml_node& node) {
	for( pugi::xml_attribute attr = node.first_attribute(); attr; attr = attr.next_attribute()) {
		String name = attr.name();
		if (  name == "scale") {
			defaultScale = attr.as_double();
		}
		if ( name == "use") {
			String typ(attr.value());
			if ( typ == "Point")
				type = IVGElement::ivgPOINT;
			if ( typ == "Hatch")
				type = IVGElement::ivgHATCH;
		}
	}
	if ( type == IVGElement::ivgPOINT) {
		for(pugi::xml_node child = node.first_child(); child;  child = child.next_sibling()) {
			if ( child.attributes_begin() == child.attributes_end())
				continue;

			String nodeName = child.name();
			IVGAttributes *attributes;
			IVGAttributes::ShapeType type = IVGAttributes::sUNKNOWN;
			if ( nodeName == "path")
				type = IVGAttributes::sPATH;
			if ( nodeName == "rect")
				type = IVGAttributes::sRECTANGLE;
			if ( nodeName == "circle")
				type = IVGAttributes::sCIRCLE;
			if ( nodeName == "line")
				type = IVGAttributes::sLINE;
			if ( nodeName == "polyline")
				type = IVGAttributes::sPOLYLINE;
			if ( nodeName == "polygon")
				type = IVGAttributes::sPOLYGON;

			if ( nodeName == "path") {
				attributes = new SVGPath();
				type = IVGAttributes::sPATH;
			} else
				attributes = new IVGAttributes(type);
			parseNode(child, attributes);
			cb += attributes->bounds;
			push_back(attributes);
		}
		normalizePositions();
	} else if ( type == IVGElement::ivgHATCH) {
		parseHatch(node);
	}
}

void  IVGElement::parseHatch(const pugi::xml_node& node) {
	hatch = new byte[128];
	hatchInverse = new byte[128];
	int cnt = 0;
	for(pugi::xml_node child = node.first_child(); child;  child = child.next_sibling()) {
		String line = child.child_value();
		unsigned long n = 0;
		for(int i=0; i < 4; ++i) {
			String s = line.substr(i * 8, 8);
			byte b = bitset<numeric_limits<byte>::digits>(s).to_ulong();
			hatch[cnt] = b;
			hatchInverse[cnt] = 0xff ^ b;
			++cnt;
		}
	}
}

void  IVGElement::parseTransform(IVGAttributes* attributes, const String& tranformString) {
	Array<String> parts;
	Split(tranformString, parts,")");
	for(int i=0; i < parts.size(); ++i) {
		String head = parts[i].sHead("(");
		head.toLower();
		Transform trans;
		if ( head == "rotate") {
			trans.type = Transform::tROTATE;
		}
		else if ( head == "translate") {
			trans.type = Transform::tTRANSLATE;
		} else if ( head == "scale") {
			trans.type = Transform::tSCALE;
		} else if ( head == "matrix" ) {
			trans.type = Transform::tMATRIX;
		}

		Array<String> parts2;
		Split(parts[i].sTail("("),parts2, " ");
		for(int j = 0; j < parts2.size(); ++j) {
			trans.parameters.push_back(parts2[j].rVal());
		}
		attributes->transformations.push_back(trans);
	}
}

void IVGElement::parseNode(const pugi::xml_node& node,IVGAttributes* attributes) {
	if ( node.attributes_begin() == node.attributes_end())
			return;
		String idv = getAttributeValue(node,"id");
		if ( idv != "")
			id = idv;
		String sd = getAttributeValue(node, "d");
		if ( sd != "") {
			((SVGPath *)attributes)->parsePath(sd);
		}
		String sfill = getAttributeValue(node, "fill");
		if ( sfill != "")
			attributes->fillColor = getColor(sfill);
		else
			attributes->fillColor = colorUNDEF;
		String transform = getAttributeValue(node, "transform");
		if ( transform != "") {
			parseTransform(attributes, transform);
		}

		String sstrw = getAttributeValue(node, "stroke-width");
		if ( sstrw != "")
			attributes->strokewidth = sstrw.rVal();
		String sstroke = getAttributeValue(node, "stroke");
		if ( sstroke != "")
			attributes->strokeColor = getColor(sstroke);
		else
			attributes->strokeColor = colorUNDEF;

		String style = getAttributeValue(node, "style"); 
		if ( style != "")
			parseStyle(style, attributes);
		String sx = getAttributeValue(node, "x");
		String sy = getAttributeValue(node, "y");
		if ( sx != "" && sy != "") {
			attributes->points.push_back(Coord(sx.rVal(), sy.rVal()));
		}
		
		String sradius = getAttributeValue(node, "r");
		if ( sradius != "")
			attributes->rx = attributes->ry = sradius.rVal();
		String sradiusx = getAttributeValue(node, "rx");
		if ( sradiusx != "")
			attributes->rx= sradiusx.rVal();
		String sradiusy = getAttributeValue(node, "ry");
		if ( sradiusy != "")
			attributes->ry= sradiusy.rVal();
		if ( attributes->type == IVGAttributes::sCIRCLE || attributes->type == IVGAttributes::sELLIPSE) {
			String scx = getAttributeValue(node, "cx");
			String scy = getAttributeValue(node, "cy");
			Coord center(0,0);
			if ( scx != "" && scy != "")
				center = Coord(sx.rVal(), scy.rVal());
			attributes->points.push_back(center);
		}

		String sx1 = getAttributeValue(node,"x1");
		String sx2 = getAttributeValue(node,"x2");
		String sy1 = getAttributeValue(node,"y1");
		String sy2 = getAttributeValue(node,"y2");
		if ( sx1 != "" && sx2 != "" && sy1 != "" && sy2 != "") {
			attributes->points.push_back(Coord(sx1.rVal(), sy1.rVal()));
			attributes->points.push_back(Coord(sx2.rVal(), sy2.rVal()));
		}
		String spnts = getAttributeValue(node, "points");
		if ( spnts != "") {
			Array<String> parts;
			Split(spnts, parts, " ");
			for(int i=0; i < parts.size(); ++i) {
				String pnt = parts[i];
				Coord c ( pnt.sHead(",").rVal(), pnt.sTail(",").rVal());
				attributes->points.push_back(c);
			}
		}


		
		String swidth = getAttributeValue(node, "width");
		if ( swidth != "" && swidth != "user-defined")
			attributes->rwidth = swidth.iVal();
		String sheight = getAttributeValue(node, "height");
		if ( sheight != "" && sheight != "user-defined")
			attributes->rheight = sheight.iVal();

		if ( attributes->rwidth == 0 && attributes->rx != 0)
			attributes->rwidth = attributes->rx;
		if ( attributes->rheight == 0 && attributes->ry != 0)
			attributes->rheight = attributes->ry;

		if ( attributes->rwidth == 0 && attributes->points.size() > 0) {
			for(int i=0; i < attributes->points.size(); ++i) {
				attributes->bounds.MinX() = min(attributes->bounds.MinX(),attributes->points[i].x);
				attributes->bounds.MinY() = min(attributes->bounds.MinY(),attributes->points[i].y);
				attributes->bounds.MaxX() = max(attributes->bounds.MaxX(),attributes->points[i].x);
				attributes->bounds.MaxY() = max(attributes->bounds.MaxY(),attributes->points[i].y);
				
			}
			attributes->rwidth = attributes->bounds.width();
			attributes->rheight = attributes->bounds.height();
		} else {
			attributes->bounds = CoordBounds(Coord(0,0), Coord(attributes->rwidth,attributes->rheight)); 
		}

}

String IVGElement::parseStyle(const String& style,IVGAttributes* attributes) {
	Array<String> parts;
	Split(style, parts,";");
	for(int i = 0; i < parts.size(); ++i) {
		String attr = parts[i].sHead(":");
		attr = attr.sTrimSpaces();
		String val = parts[i].sTail(":");
		if ( attr == "fill") {
			attributes->fillColor = getColor(val);
		}
		else if ( attr == "stroke-width") {
			attributes->strokewidth = val.iVal();
		}
		else if ( attr == "stroke") {
			attributes->strokeColor = getColor(val);
		}
		else if (attr == "fill-opacity") {
			attributes->opacity = val.rVal();
		}
	}

	return ""; 
}

String IVGElement::getAttributeValue(const pugi::xml_node& node, const String& key) const{
	if ( node.attributes_begin() == node.attributes_end())
		return "";
	pugi:: xml_attribute attr = node.attribute(key.c_str());
	if ( attr.empty())
		return "";
	return attr.value();
}



void IVGElement::initSvgData() {
	if ( svgcolors.size() != 0) 
		return;
	int k = svgcolors.max_size();
	svgcolors["user-defined"] = colorUSERDEF;
	svgcolors["none"] = colorUNDEF;
	svgcolors["aliceblue"] = Color(240,248,255);
	svgcolors["antiquewhite"] = Color(250,235,215);
	svgcolors["aqua"] = Color(0,255,255);
	svgcolors["aquamarine"] = Color(127,255,212);
	svgcolors["azure"] = Color(240,255,255);
	svgcolors["beige"] = Color(245,245,220);
	svgcolors["bisque"] = Color(255,228,196);
	svgcolors["black"] = Color(0,0,0);
	svgcolors["blanchedalmond"] = Color(255,235,205);
	svgcolors["blue"] = Color(0,0,255);
	svgcolors["blueviolet"] = Color(138,43,226);
	svgcolors["brown"] = Color(165,42,42);
	svgcolors["burlywood"] = Color(222,184,135);
	svgcolors["cadetblue"] = Color(95,158,160);
	svgcolors["chartreuse"] = Color(127,255,0);
	svgcolors["chocolate"] = Color(210,105,30);
	svgcolors["coral"] = Color(255,127,80);
	svgcolors["cornflowerblue"] = Color(100,149,237);
	svgcolors["cornsilk"] = Color(255,248,220);
	svgcolors["crimson"] = Color(220,20,60);
	svgcolors["cyan"] = Color(0,255,255);
	svgcolors["darkblue"] = Color(0,0,139);
	svgcolors["darkcyan"] = Color(0,139,139);
	svgcolors["darkgoldenrod"] = Color(184,134,11);
	svgcolors["darkgray"] = Color(169,169,169);
	svgcolors["darkgreen"] = Color(0,100,0);
	svgcolors["darkgrey"] = Color(169,169,169);
	svgcolors["darkkhaki"] = Color(189,183,107);
	svgcolors["darkmagenta"] = Color(139,0,139);
	svgcolors["darkolivegreen"] = Color(85,107,47);
	svgcolors["darkorange"] = Color(255,140,0);
	svgcolors["darkorchid"] = Color(153,50,204);
	svgcolors["darkred"] = Color(139,0,0);
	svgcolors["darksalmon"] = Color(233,150,122);
	svgcolors["darkseagreen"] = Color(143,188,143);
	svgcolors["darkslateblue"] = Color(72,61,139);
	svgcolors["darkslategray"] = Color(47,79,79);
	svgcolors["darkslategrey"] = Color(47,79,79);
	svgcolors["darkturquoise"] = Color(0,206,209);
	svgcolors["darkviolet"] = Color(148,0,211);
	svgcolors["deeppink"] = Color(255,20,147);
	svgcolors["deepskyblue"] = Color(0,191,255);
	svgcolors["dimgray"] = Color(105,105,105);
	svgcolors["dimgrey"] = Color(105,105,105);
	svgcolors["dodgerblue"] = Color(30,144,255);
	svgcolors["firebrick"] = Color(178,34,34);
	svgcolors["floralwhite"] = Color(255,250,240);
	svgcolors["forestgreen"] = Color(34,139,34);
	svgcolors["fuchsia"] = Color(255,0,255);
	svgcolors["gainsboro"] = Color(220,220,220);
	svgcolors["ghostwhite"] = Color(248,248,255);
	svgcolors["gold"] = Color(255,215,0);
	svgcolors["goldenrod"] = Color(218,165,32);
	svgcolors["gray"] = Color(128,128,128);
	svgcolors["green"] = Color(0,128,0);
	svgcolors["greenyellow"] = Color(173,255,47);
	svgcolors["grey"] = Color(128,128,128);
	svgcolors["honeydew"] = Color(240,255,240);
	svgcolors["hotpink"] = Color(255,105,180);
	svgcolors["indianred"] = Color(205,92,92);
	svgcolors["indigo"] = Color(75,0,130);
	svgcolors["ivory"] = Color(255,255,240);
	svgcolors["khaki"] = Color(240,230,140);
	svgcolors["lavender"] = Color(230,230,250);
	svgcolors["lavenderblush"] = Color(255,240,245);
	svgcolors["lawngreen"] = Color(124,252,0);
	svgcolors["lemonchiffon"] = Color(255,250,205);
	svgcolors["lightblue"] = Color(173,216,230);
	svgcolors["lightcoral"] = Color(240,128,128);
	svgcolors["lightcyan"] = Color(224,255,255);
	svgcolors["lightgoldenrodyellow"] = Color(250,250,210);
	svgcolors["lightgray"] = Color(211,211,211);
	svgcolors["lightgreen"] = Color(144,238,144);
	svgcolors["lightgrey"] = Color(211,211,211);
	svgcolors["lightpink"] = Color(255,182,193);
	svgcolors["lightsalmon"] = Color(255,160,122);
	svgcolors["lightseagreen"] = Color(32,178,170);
	svgcolors["lightskyblue"] = Color(135,206,250);
	svgcolors["lightslategray"] = Color(119,136,153);
	svgcolors["lightslategrey"] = Color(119,136,153);
	svgcolors["lightsteelblue"] = Color(176,196,222);
	svgcolors["lightyellow"] = Color(255,255,224);
	svgcolors["lime"] = Color(0,255,0);
	svgcolors["limegreen"] = Color(50,205,50);
	svgcolors["linen"] = Color(250,240,230);
	svgcolors["magenta"] = Color(255,0,255);
	svgcolors["maroon"] = Color(128,0,0);
	svgcolors["mediumaquamarine"] = Color(102,205,170);
	svgcolors["mediumblue"] = Color(0,0,205);
	svgcolors["mediumorchid"] = Color(186,85,211);
	svgcolors["mediumpurple"] = Color(147,112,219);
	svgcolors["mediumseagreen"] = Color(60,179,113);
	svgcolors["mediumslateblue"] = Color(123,104,238);
	svgcolors["mediumspringgreen"] = Color(0,250,154);
	svgcolors["mediumturquoise"] = Color(72,209,204);
	svgcolors["mediumvioletred"] = Color(199,21,133);
	svgcolors["midnightblue"] = Color(25,25,112);
	svgcolors["mintcream"] = Color(245,255,250);
	svgcolors["mistyrose"] = Color(255,228,225);
	svgcolors["moccasin"] = Color(255,228,181);
	svgcolors["navajowhite"] = Color(255,222,173);
	svgcolors["navy"] = Color(0,0,128);
	svgcolors["oldlace"] = Color(253,245,230);
	svgcolors["olive"] = Color(128,128,0);
	svgcolors["olivedrab"] = Color(107,142,35);
	svgcolors["orange"] = Color(255,165,0);
	svgcolors["orangered"] = Color(255,69,0);
	svgcolors["orchid"] = Color(218,112,214);
	svgcolors["palegoldenrod"] = Color(238,232,170);
	svgcolors["palegreen"] = Color(152,251,152);
	svgcolors["paleturquoise"] = Color(175,238,238);
	svgcolors["palevioletred"] = Color(219,112,147);
	svgcolors["papayawhip"] = Color(255,239,213);
	svgcolors["peachpuff"] = Color(255,218,185);
	svgcolors["peru"] = Color(205,133,63);
	svgcolors["pink"] = Color(255,192,203);
	svgcolors["plum"] = Color(221,160,221);
	svgcolors["powderblue"] = Color(176,224,230);
	svgcolors["purple"] = Color(128,0,128);
	svgcolors["red"] = Color(255,0,0);
	svgcolors["rosybrown"] = Color(188,143,143);
	svgcolors["royalblue"] = Color(65,105,225);
	svgcolors["saddlebrown"] = Color(139,69,19);
	svgcolors["salmon"] = Color(250,128,114);
	svgcolors["sandybrown"] = Color(244,164,96);
	svgcolors["seagreen"] = Color(46,139,87);
	svgcolors["seashell"] = Color(255,245,238);
	svgcolors["sienna"] = Color(160,82,45);
	svgcolors["silver"] = Color(192,192,192);
	svgcolors["skyblue"] = Color(135,206,235);
	svgcolors["slateblue"] = Color(106,90,205);
	svgcolors["slategray"] = Color(112,128,144);
	svgcolors["slategrey"] = Color(112,128,144);
	svgcolors["snow"] = Color(255,250,250);
	svgcolors["springgreen"] = Color(0,255,127);
	svgcolors["steelblue"] = Color(70,130,180);
	svgcolors["tan"] = Color(210,180,140);
	svgcolors["teal"] = Color(0,128,128);
	svgcolors["thistle"] = Color(216,191,216);
	svgcolors["tomato"] = Color(255,99,71);
	svgcolors["turquoise"] = Color(64,224,208);
	svgcolors["violet"] = Color(238,130,238);
	svgcolors["wheat"] = Color(245,222,179);
	svgcolors["white"] = Color(255,255,255);
	svgcolors["whitesmoke"] = Color(245,245,245);
	svgcolors["yellow"] = Color(255,255,0);
	svgcolors["yellowgreen"] = Color(154,205,50);
}

void IVGElement::normalizePositions() {
	double shiftx = (cb.cMax.x + cb.cMin.x) / 2.0;
	double shifty = (cb.cMax.y + cb.cMin.y) / 2.0;
	for(int j = 0; j < size(); ++j) {
		IVGAttributes *att = at(j);
		for(int i = 0; i < att->points.size(); ++i) {
			att->points[i].x = att->points[i].x - shiftx;
			att->points[i].y = att->points[i].y - shifty;
		}
		if ( att->isPolygon()) { // need tot triangulate
			Triangulator tri;
			tri.getTriangulation(att->points, att->triangleStrips);
		}
	}
}

bool IVGAttributes::isPolygon() const {
	return type == IVGAttributes::sPOLYGON;
}

const byte * IVGElement::getHatch() const{
	return (const byte *) hatch;
}

const byte * IVGElement::getHatchInverse() const{
	return (const byte *) hatchInverse;
}