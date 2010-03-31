/* ProjectionLaBelge1972
   Copyright Ilwis System Development ITC
   march 1998, by Jan Hendrikse
	Last change:  WK    9 Apr 98    3:12 pm
*/

#ifndef ILWPRJLaBelge72_H
#define ILWPRJLaBelge72_H
#include "Engine\SpatialReference\LCONCON.H"

class DATEXPORT ProjectionLaBelge1972: public ProjectionLambConfConic
{
public:
  ProjectionLaBelge1972(const Ellipsoid&);
	void Prepare();
	virtual XY xyConv(const PhiLam&) const;
  virtual PhiLam plConv(const XY&) const;
  Datum* datumDefault() const;
};

#endif // ILLWPRJLaBelge72_H




