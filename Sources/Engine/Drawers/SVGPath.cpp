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

using namespace ILWIS;

void SVGPath::parsePath(const String& p1) {
	String p2 = preprocess(p1);
	Array<String> parts;
	Split(p2, parts," ");
	type = SVGAttributes::sLINE;
	vector<vector<String>> pathParts;
	for(int i = 0 ; i < parts.size(); ++i) {
		String part = parts[i];
		PathElement::PositionType type;
		getPathElementType(part, type);
		if ( type != PathElement::pUNCHANGED) {
			pathParts.push_back(vector<String>());
		}
		(pathParts.back()).push_back(part);
	}
	int index = 0;
	for(int i = 0; i < pathParts.size(); ++i)
		parsePart(index, pathParts[i]);
	pathElements.back().end = points.size() - 1;
	if ( points.size() > 2 && points[0] == points.back())
		type = SVGAttributes::sPATH;

}

String SVGPath::preprocess(const String& p) {
	String result = "";
	bool skipNextSpaces = false;
	for(int i=0; i < p.size(); ++i) {
		char c = p[i];
		if ( c != ' ')
			skipNextSpaces = false;
		if ( c >= 65 && c != 'z' && c != 'Z') // z is always a singleton, so there always will be a space after it for the next command
			skipNextSpaces = true;
		if ( c == ',')
			skipNextSpaces = true;
		if ( c == ' ' && skipNextSpaces == true)
			continue;
		result += c;
	}
	return result;
}

void SVGPath::parsePart(int& index, const vector<String>& commands) {
	PathElement::PositionType ptype;
	PathElement::ElementType etype;
	etype = getPathElementType(commands[0], ptype);
	switch(etype) {
		case PathElement::eMOVE:
			doMove(index, ptype, commands); break;
		case PathElement::eLINE:
			doLine(index, ptype, commands); break;
		case PathElement::eVLINE:
			doVLine(index, ptype, commands); break;
		case PathElement::eHLINE:
			doHLine(index, ptype, commands); break;
		case PathElement::eELLIPTICARC:
			doEllipictalArc(index, ptype, commands); break;
		case PathElement::eQBEZIER:
			doQBezier(index, ptype, commands); break;
		case PathElement::eCBEZIER:
			doCBezier(index, ptype, commands); break;
		case PathElement::eCLOSE:
			doClose(index, ptype, commands); break;
		case PathElement::eEXTQBEZIER:
			doExtQBezier(index, ptype, commands); break;
		case PathElement::eEXTCBEZIER:
			doExtCBezier(index, ptype, commands); break;
	};

}

void SVGPath::doExtCBezier(int& index, PathElement::PositionType ptype, const vector<String>& commands) {
	PathElement pe;
	pe.type = PathElement::eQBEZIER;
	pe.start = index;
	Coord c1 = cPen;
	Coord c2 = getCoord(index, ptype, commands[0]);
	Coord c3 =  Coord (2 * cPen.x - cpt.x, 2 * cPen.y - cpt.y);
	Coord c4 = getCoord(index, ptype, commands[1]);
	int sections = 20;
	for(int i = 0; i < sections; ++i) {
		double t = (double)i / sections;
		Coord c;
		cbezier(c, c1,c2,c3,c4,t);

		addPoint(index, c);
	}
	addPart(pe);
}

void SVGPath::doExtQBezier(int& index, PathElement::PositionType ptype, const vector<String>& commands) {
	PathElement pe;
	pe.type = PathElement::eQBEZIER;
	pe.start = index;
	Coord c1 = cPen;
	Coord c2 = Coord(2 * cPen.x - cpt.x, 2 * cPen.y - cpt.y);
	Coord c3 = getCoord(index, ptype, commands[0]);
	int sections = 20;
	for(int i = 0; i <= sections; ++i) {
		double t = (double)i / sections;
		Coord c;
		qbezier(c, c1,c2,c3,t);

		addPoint(index, c);
	}
	addPart(pe);

}

void SVGPath::doClose(int& index, PathElement::PositionType ptype, const vector<String>& commands) {
	Coord c = lastMove;
	addPoint(index, c);
	type = SVGAttributes::sPOLYGON; // if no fill color is defined, polygon will anyhow be drawn as a line drawing
}

void SVGPath::doCBezier(int& index, PathElement::PositionType ptype, const vector<String>& commands) {
	PathElement pe;
	pe.type = PathElement::eQBEZIER;
	pe.start = index;
	Coord c1 = cPen;
	Coord c2 = getCoord(index, ptype, commands[0]);
	Coord c3 = getCoord(index, ptype, commands[1]);
	Coord c4 = getCoord(index, ptype, commands[2]);
	cpt = c3;
	int sections = 20;
	for(int i = 0; i < sections; ++i) {
		double t = (double)i / sections;
		Coord c;
		cbezier(c, c1,c2,c3,c4,t);

		addPoint(index, c);
	}
	addPart(pe);
}

void SVGPath::linearInterPol(Coord& dest, const Coord& a, const Coord& b, const double t)
{
    dest.x = a.x + (b.x-a.x)*t;
    dest.y = a.y + (b.y-a.y)*t;
}

// evaluate a Coord on a bezier-curve. t goes from 0 to 1.0
void SVGPath::cbezier(Coord &dest, const Coord& a, const Coord& b, const Coord& c, const Coord& d, const double t)
{
    Coord ab,bc,cd,abbc,bccd;
    linearInterPol(ab, a,b,t);           // Coord between a and b (green)
    linearInterPol(bc, b,c,t);           // Coord between b and c (green)
    linearInterPol(cd, c,d,t);           // Coord between c and d (green)
    linearInterPol(abbc, ab,bc,t);       // Coord between ab and bc (blue)
    linearInterPol(bccd, bc,cd,t);       // Coord between bc and cd (blue)
    linearInterPol(dest, abbc,bccd,t);   // Coord on the bezier-curve (black)
}

void SVGPath::qbezier(Coord &dest, const Coord& a, const Coord& b, const Coord& c, const double t)
{
    Coord ab,bc,cd,abbc,bccd;
    linearInterPol(ab, a,b,t);           // Coord between a and b (green)
    linearInterPol(bc, b,c,t);           // Coord between b and c (green)
    linearInterPol(dest, ab,bc,t);       // Coord between ab and bc (blue)
}

void SVGPath::doQBezier(int& index, PathElement::PositionType ptype, const vector<String>& commands) {
	PathElement pe;
	pe.type = PathElement::eQBEZIER;
	pe.start = index;
	Coord c1 = cPen;
	Coord c2 = getCoord(index, ptype, commands[0]);
	Coord c3 = getCoord(index, ptype, commands[1]);
	cpt = c2;
	int sections = 20;
	for(int i = 0; i <= sections; ++i) {
		double t = (double)i / sections;
		Coord c;
		qbezier(c, c1,c2,c3,t);

		addPoint(index, c);
	}
	addPart(pe);
}

void SVGPath::doEllipictalArc(int& index, PathElement::PositionType ptype, const vector<String>& commands) {
	String p = commands[0];
	String x = p.sHead(",");
	x = x.substr(1,x.size() - 1);
	String y = p.sTail(",");
	double rx = x.rVal();
	double ry = y.rVal();
	PathElement pe;
	pe.type = PathElement::eELLIPTICARC;
	pe.start = index;

	double theta = commands[1].rVal();
	String flags = commands[2];
	bool isLargeArc = flags.sHead(",") == "1";
	bool isSweepArc = flags.sTail(",") == "1";
	Coord cEnd = getCoord(index,ptype, commands[4]);
	makeArc(index, theta, rx, ry, cEnd, isLargeArc, isSweepArc);
	addPart(pe);
}


void SVGPath::doMove(int& index, PathElement::PositionType ptype, const vector<String>& commands) {
	Coord c = getCoord(index,ptype,commands[0]);
	cPen = c;
	lastMove = c;
}

void SVGPath::doLine(int& index, PathElement::PositionType ptype, const vector<String>& commands) {
	PathElement pe = PathElement();
	pe.type = PathElement::eLINE;
	pe.start = index;
	for(int i = 0; i < commands.size(); ++i) {
		Coord c = getCoord(index,ptype,commands[i]);
		addPoint(index, c);
	}
	addPart(pe);
}

void SVGPath::doVLine(int& index, PathElement::PositionType ptype, const vector<String>& commands) {
	PathElement pe = PathElement();
	pe.type = PathElement::eVLINE;
	pe.start = index;
	String command = commands[0];
	String n = command.sSub(1, command.size() - 1);
	double shift = n.rVal();
	Coord c(cPen);
	if ( points.size() == 0)
		addPoint(index, cPen);
	if ( ptype == PathElement::pABSOLUTE) {
		c.y = shift;
	} else {
		c.y += shift;
	}
	addPoint(index, c);
	addPart(pe);
}

void SVGPath::doHLine(int& index, PathElement::PositionType ptype, const vector<String>& commands) {
	PathElement pe;
	pe.type = PathElement::eHLINE;
	pe.start = index;
	String command = commands[0];
	String n = command.sSub(1, command.size() - 1);
	double shift = n.rVal();
	Coord c(cPen);
	if ( points.size() == 0)
		addPoint(index, cPen);
	if ( ptype == PathElement::pABSOLUTE) {
		c.x = shift;
	} else {
		c.x += shift;
	}
	addPoint(index, c);
	addPart(pe);
}

void SVGPath::addPoint(int& index, const Coord& c) {
	points.push_back(c);
	++index;
	cPen = c;
	bounds += c;
}

void SVGPath::addPart(const PathElement& el) {
	if ( pathElements.size() != 0) {
		pathElements.back().end = el.start - 1;
	}
	pathElements.push_back(el);

}

Coord SVGPath::getCoord(int index, PathElement::PositionType ptype, const String& part) const{
	int offset = part[0] < 58 ? 0 : 1;

	String x = part.sHead(",");
	x = x.substr(offset,x.size() - offset);
	String y = part.sTail(",");
	double dx = x.rVal();
	double dy = y.rVal();
	if ( ptype != PathElement::pABSOLUTE && index > 0) {
		dx += cPen.x;
		dy += cPen.y;
	}

	Coord c = Coord(dx, dy);

	return c;
}

PathElement::ElementType SVGPath::getPathElementType(const String& p,PathElement::PositionType& ptype) const{
	String temp = p;
	PathElement::ElementType type = PathElement::eNONE;
	temp.toLower();
	if ( temp[0] == 'm') {
		type = PathElement::eMOVE;
	}
	else if ( temp[0] == 'l') {
		type = PathElement::eLINE;
	}
	else if ( temp[0] == 'c') {
		type = PathElement::eCBEZIER;
	}
	else if ( temp[0] == 'h') {
		type = PathElement::eHLINE;
	}
	else if ( temp[0] == 'v') {
		type = PathElement::eVLINE;
	}	
	else if ( temp[0] == 'q') {
		type = PathElement::eQBEZIER;
	}
	else if ( temp[0] == 'a') {
		type = PathElement::eELLIPTICARC;
	} 
	else if ( temp[0] == 't') {
		type = PathElement::eEXTQBEZIER;
	}
	else if ( temp[0] == 's') {
		type = PathElement::eEXTCBEZIER;
	}
	else if ( temp[0] == 'z') {
		type = PathElement::eCLOSE;
	}
	if ( p[0] >= 65 && p[0] <= 90)
		ptype = PathElement::pABSOLUTE;
	else if ( p[0] >= 97 && p[0] <= 122)
		ptype = PathElement::pRELATIVE;
	else
		ptype = PathElement::pUNCHANGED;


	return type;
}

void SVGPath::makeArc(int& index, double theta, double rx, double ry, const Coord& c2, bool isLargeArc, bool isSweepArc) {
	// Get the current (x, y) coordinates of the path
	Coord p2d = cPen;
	double x0 = p2d.x;
	double y0 = p2d.y;
	// Compute the half distance between the current and the final point
	double dx2 = (x0 - c2.x) / 2.0;
	double dy2 = (y0 - c2.y) / 2.0;
	// Convert theta from degrees to radians
	theta = M_PI / 180.0 * theta;

	//
	// Step 1 : Compute (x1, y1)
	//
	double x1 = cos(theta)  * dx2 + sin(theta) *  dy2;
	double y1 = -sin(theta) *  dx2 + cos(theta) *  dy2;
	// Ensure radii are large enough
	rx = abs(rx);
	ry = abs(ry);
	double Prx = rx * rx;
	double Pry = ry * ry;
	double Px1 = x1 * x1;
	double Py1 = y1 * y1;
	double d = Px1 / Prx + Py1 / Pry;
	if (d > 1) {
		rx = abs(sqrt(d) * rx);
		ry = abs(sqrt(d) * ry);
		Prx = rx * rx;
		Pry = ry * ry;
	}

	//
	// Step 2 : Compute (cx1, cy1)
	//
	double sign = (isLargeArc == isSweepArc) ? -1.0 : 1.0;
	double q = ((Prx * Pry) - (Prx * Py1) - (Pry * Px1)) / ((Prx * Py1) + (Pry * Px1));
	q = max(0.0, q) ; // q may get 0, but due to rounding this also can be very slightly negative. So make it 0 than
	double coef = (sign * sqrt(q));
	double cx1 = coef * ((rx * y1) / ry);
	double cy1 = coef * -((ry * x1) / rx);

	//
	// Step 3 : Compute (cx, cy) from (cx1, cy1)
	//
	double sx2 = (x0 + c2.x) / 2.0f;
	double sy2 = (y0 + c2.y) / 2.0f;
	double cx = sx2 + (cos(theta) * cx1 - sin(theta) * cy1);
	double cy = sy2 + (sin(theta) * cx1 + cos(theta)                               * (double) cy1);

	//
	// Step 4 : Compute the angleStart (theta1) and the angleExtent (dtheta)
	//
	double ux = (x1 - cx1) / rx;
	double uy = (y1 - cy1) / ry;
	double vx = (-x1 - cx1) / rx;
	double vy = (-y1 - cy1) / ry;
	double p, n;
	// Compute the angle start
	n = sqrt((ux * ux) + (uy * uy));
	p = ux; // (1 * ux) + (0 * uy)
	sign = (uy < 0) ? -1 : 1;
	double angleStart = sign * acos(p / n);
	// Compute the angle extent
	n = sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
	p = ux * vx + uy * vy;
	sign = (ux * vy - uy * vx < 0) ? -1 : 1;
	double angleExtent2 =  (180.0 / M_PI ) * (sign * acos(p / n));
	double angleExtent =  sign * acos(p / n);
	if (!isSweepArc && angleExtent > 0) {
		angleExtent -= 2 * M_PI;
	} else if (isSweepArc && angleExtent < 0) {
		angleExtent += 2 * M_PI;
	}
	angleExtent = fmod(angleExtent, 2 * M_PI);
	angleStart = fmod(angleStart, 2 * M_PI);

	int sections = 20;

	for(int i = 0; i <= sections;i++) { // make $section number of circles
		double angle = angleStart  - i * angleExtent / sections;
		double x = cx + rx * cos( angle);
		double y = cy + ry * sin(angle);
		addPoint(index, Coord(x,y));
	}

}

bool SVGPath::isPolygon() const {
	return points.size() > 2 && points[0] == points.back() && fillColor != colorUNDEF;
}
