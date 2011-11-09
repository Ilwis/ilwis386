#pragma once

namespace ILWIS {

	struct PathElement {
		enum ElementType{eLINE, eHLINE, eVLINE, eCBEZIER,eQBEZIER, eELLIPTICARC,eMOVE, eCLOSE, eEXTQBEZIER, eEXTCBEZIER, eNONE};
		enum PositionType{pABSOLUTE,pRELATIVE,pUNCHANGED, pUNKNOWN};

		PathElement() : start(iUNDEF), end(iUNDEF), type(eNONE), ptype(pUNKNOWN){}
		ElementType type;
		PositionType ptype;
		int start;
		int end;
	};

	class SVGPath : public SVGAttributes {
	public:
		void parsePath(const String& pathElements);
		int noOfElements() const{return pathElements.size();}
		PathElement getElement(int i) const {return pathElements[i];}
	private:
		PathElement::ElementType getPathElementType(const String& p, PathElement::PositionType& ptype) const;
		void parsePart(int& index, const vector<String>& commands);
		void doMove(int& index, PathElement::PositionType ptype, const vector<String>& commands);
		void doLine(int& index, PathElement::PositionType ptype, const vector<String>& commands);
		void doVLine(int& index, PathElement::PositionType ptype, const vector<String>& commands);
		void doHLine(int& index, PathElement::PositionType ptype, const vector<String>& commands);
		void doEllipictalArc(int& index, PathElement::PositionType ptype, const vector<String>& commands);
		void doQBezier(int& index, PathElement::PositionType ptype, const vector<String>& commands);
		void doCBezier(int& index, PathElement::PositionType ptype, const vector<String>& commands);
		void doClose(int& index, PathElement::PositionType ptype, const vector<String>& commands);
		void doExtQBezier(int& index, PathElement::PositionType ptype, const vector<String>& commands);
		void doExtCBezier(int& index, PathElement::PositionType ptype, const vector<String>& commands);
		Coord getCoord(int index, PathElement::PositionType ptype, const String& part) const;
		void addPart(const PathElement& el);
		String preprocess(const String& p);
		void makeArc(int& index, double theta, double rx, double ry, const Coord& c2, bool isLargeArc, bool isSweepArc);
		void addPoint(int& index, const Coord& c);
		void linearInterPol(Coord& dest, const Coord& a, const Coord& b, const double t);
		void cbezier(Coord &dest, const Coord& a, const Coord& b, const Coord& c, const Coord& d, const double t);
		void qbezier(Coord &dest, const Coord& a, const Coord& b, const Coord& c, const double t);
		bool isPolygon() const;

		vector<PathElement> pathElements;
		Coord cPen;
		Coord cpt; // for cubic bezier, keeping last ctp for possible further uses
		Coord lastMove;

	};

}