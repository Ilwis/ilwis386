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
class LegendSegmentClassDrawer: public LegendClassDrawer
{
public:
  LegendSegmentClassDrawer(LegendLayoutItem& item)
    : LegendClassDrawer(item)
  {}
  ~LegendSegmentClassDrawer()
  {}
  bool fValid() const
  {
    if (!LegendClassDrawer::fValid())
      return false;
    SegmentMapDrawer* sdr = dynamic_cast<SegmentMapDrawer*>(bmd());
    return 0 != sdr;
  }
	virtual void WriteElements(ElementContainer& ec, const char* sSection)
	{
		LegendClassDrawer::WriteElements(ec, sSection);
		ObjectInfo::WriteElement(sSection, "LegendType", ec, "SegmentClass");
	}
protected:
  virtual void DrawItem(CDC* cdc, CPoint p, int i)
  {
    long iRaw = aiKeys[i];
    CPoint p1, p2;
    p1 = p2 = p;
    p1.y += iRowHeight / 2;
    p2.x += rounding(rWidthFact * iRowHeight);
    p2.y = p1.y;
		Representation rpr = bmd()->rpr();
		if (!rpr.fValid())
			return;
    Line line(rpr, iRaw);
    line.drawLine(cdc->GetSafeHdc(), p1, p2);
  }
};

