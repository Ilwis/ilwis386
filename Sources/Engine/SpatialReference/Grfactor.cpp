/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
/* $Log: /ILWIS 3.0/GeoReference/Grfactor.cpp $
 * 
 * 8     13-12-02 18:38 Hendrikse
 * implemented virtual bool fGeoRefNone() const;
 * 
 * 7     13-08-01 17:05 Koolhoven
 * GeoRefSubMap, Factor and MirrorRotate now have a fDependent() function
 * which always returns true. Ensuring that they will have a Dependeny tab
 * in the property form
 * 
 * 6     13-03-01 12:55p Martin
 * GetObjectStructure had not a proper boolean to prevent Associated files
 * to be taken along when deleting
 * 
 * 5     26-02-01 9:10a Martin
 * added correct GetObjectStructure functions for these two
 * 
 * 4     17/08/99 9:33 Willem
 * Added setter function for the GeoRef size
 * 
 * 3     13/08/99 16:38 Willem
 * Extended GeoRefFactor with getter/setter function for the Factor and
 * the Offset
 * 
 * 2     10/08/99 16:35 Willem
 * - The factor now ignores the sign: it is turned into a positiv number
 * - The rPixelSize() is now calculated more accurately for factors
 * smaller than zero.
 Revision 1.3  1998/09/16 17:24:43  Wim
 */
// 22beta2
//
// Revision 1.2  1997/08/25 08:41:02  Wim
// Added ObjectDependency in Store() to be sure that Copy() works
// properly
//
/* GeoRefFactor
   Copyright Ilwis System Development ITC
   november 1995, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:04 pm
*/
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grfactor.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\objdepen.h"

GeoRefFactor::GeoRefFactor(const FileName& fn)
: GeoRefPtr(fn)
{
  ReadElement("GeoRefFactor", "GeoRef", gr);
  if (!gr.fValid())
    gr = GeoRef(rcSize());
  rFact = rReadElement("GeoRefFactor", "Factor");
  rFact = abs(rFact);
  iRowOffset = iReadElement("GeoRefFactor", "RowOffset");
  iColOffset = iReadElement("GeoRefFactor", "ColOffset");
  if (iRowOffset == iUNDEF)
    iRowOffset = 0;
  if (iColOffset == iUNDEF)
    iColOffset = 0;
}

GeoRefFactor::GeoRefFactor(const FileName& fn, const GeoRef& grf, double rEnl, long iRowOff, long iColOff)
: GeoRefPtr(fn, grf->cs(), RowCol()),
  gr(grf), rFact(rEnl), iRowOffset(iRowOff), iColOffset(iColOff)
{
  RowCol rc = grf->rcSize();
  rFact = abs(rFact);
  if (rFact > 1) {
    rc.Row *= rFact;
    rc.Col *= rFact;
  }
  else {
    long iInvFact = longConv(1.0 / rFact);
    rc.Row -= iRowOffset;
    rc.Row /= iInvFact;
    rc.Col -= iColOffset;
    rc.Col /= iInvFact;
  }
  SetRowCol(rc);
}

// Getter/Setter functions
void GeoRefFactor::SetRowCol(RowCol rcSize)
{
	GeoRefPtr::SetRowCol(rcSize);
}

double GeoRefFactor::rFactor() const
{
	return rFact;
}

void GeoRefFactor::SetFactor(double rFactor)
{
	rFact = rFactor;
}

RowCol GeoRefFactor::rcOffset() const
{
	return RowCol(iRowOffset, iColOffset);
}

void GeoRefFactor::SetOffset(RowCol rcOffset)
{
	iRowOffset = rcOffset.Row;
	iColOffset = rcOffset.Col;
}

// Property function
String GeoRefFactor::sType() const
{
  return "GeoReference Factor";
}

void GeoRefFactor::Store()
{
  GeoRefPtr::Store();
  WriteElement("GeoRef", "Type", "GeoRefFactor");
  WriteElement("GeoRefFactor", "GeoRef", gr);
  WriteElement("GeoRefFactor", "Factor", rFact);
  WriteElement("GeoRefFactor", "RowOffset", iRowOffset);
  WriteElement("GeoRefFactor", "ColOffset", iColOffset);
  ObjectDependency objdep;
  objdep.Add(gr);
  objdep.Store(this);
}

void GeoRefFactor::Coord2RowCol
  (const Coord& c, double& rRow, double& rCol) const
{
  gr->Coord2RowCol(c, rRow, rCol);
  rRow -= iRowOffset;
  rCol -= iColOffset;
  rRow *= rFact;
  rCol *= rFact;
}

void GeoRefFactor::RowCol2Coord
  (double rRow, double rCol, Coord& c) const
{
  rRow /= rFact;
  rRow += iRowOffset;
  rCol /= rFact;
  rCol += iColOffset;
  gr->RowCol2Coord(rRow, rCol, c);
}

double GeoRefFactor::rPixSize() const
{
	double r = gr->rPixSize();
	if (r != rUNDEF)
		if (rFact > 1)
			r /= rFact;
		else
		{
			long iInvFact = longConv(1.0 / rFact);
			r *= iInvFact;
		}
	return r;  
}

bool GeoRefFactor::fEqual(const IlwisObjectPtr& ptr) const
{
  if (!GeoRefPtr::fEqual(ptr))
    return false;
  const GeoRefFactor* grf = dynamic_cast<const GeoRefFactor*>(&ptr);
  if (0 == grf)
    return false;
  return (rFact == grf->rFact) && (gr == grf->gr);
}

bool GeoRefFactor::fLinear() const
{
  return gr->fLinear();
}

bool GeoRefFactor::fNorthOriented() const
{
  return gr->fNorthOriented();
}

void GeoRefFactor::GetObjectStructure(ObjectStructure& os)
{
	GeoRefPtr::GetObjectStructure(os);
	if ( os.fGetAssociatedFiles())
		os.AddFile(fnObj, "GeoRefFactor", "GeoRef");
}

bool GeoRefFactor::fDependent() const
{
  return true;
}

bool GeoRefFactor::fGeoRefNone() const
{
   return gr->fGeoRefNone();
}


