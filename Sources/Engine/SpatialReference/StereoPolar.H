/* ProjectionStereoPolar
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JHE   3 Oct 97    3:02 pm
*/

#ifndef ILWPRJStereoPolar_H
#define ILWPRJStereoPolar_H
#include "Engine\SpatialReference\stereo.h"

class DATEXPORT ProjectionStereoPolar: public ProjectionStereoGraphic
{
public:
  ProjectionStereoPolar(const Ellipsoid&);
	virtual XY xyConv(const PhiLam&) const;
  virtual PhiLam plConv(const XY&) const;
  virtual void Param(ProjectionParamValue, long);
protected:
	double Z_Bw;
	double g_Bw;
	double G_Bw;
};

#endif // ILWPRJStereoPolar_H




