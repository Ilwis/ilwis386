/* ProjectionLaBelge2008
   Copyright Ilwis System Development ITC
   march 1998, by Jan Hendrikse
	Last change:  WK    9 Apr 98    3:12 pm
*/

#ifndef ILWPRJLaBelge08_H
#define ILWPRJLaBelge08_H
#include "Engine\SpatialReference\LCONCON.H"

class DATEXPORT ProjectionLaBelge2008: public ProjectionLambConfConic
{
public:
  ProjectionLaBelge2008(const Ellipsoid&);
	void Prepare();
	virtual XY xyConv(const PhiLam&) const;
  virtual PhiLam plConv(const XY&) const;
  Datum* datumDefault() const;
};

#endif // ILLWPRJLaBelge08_H




