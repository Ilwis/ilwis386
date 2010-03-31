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
// RoseDiagramDrawer.h: interface for the RoseDiagramDrawer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROSEDIAGRAMDRAWER_H)
#define AFX_ROSEDIAGRAMDRAWER_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif


class GraphView;
class RoseDiagramAxis;

#include "Client\GraphWindow\RoseDiagramLayer.h"
#include "Client\GraphWindow\GraphDrawer.h"

class IMPEXP RoseDiagramDrawer : public GraphDrawer
{
public:
	RoseDiagramDrawer(GraphView*);
	virtual ~RoseDiagramDrawer();
	virtual bool fConfigure();
  bool fConfigure(RoseDiagramLayer* rdl, bool fLastGraph);
	CPoint ptPos(double rRow, double rCol) const;
  void Pt2RowCol(const CPoint& p, double& rRow, double &rCol) const;
	virtual int iSpaceNeededX() const;
	virtual int iSpaceNeededY() const;
  virtual void draw(CDC*);
  virtual String sText(CPoint p) const;
  void SaveSettings(const FileName& fn, const String& sSection);
  void LoadSettings(const FileName& fn, const String& sSection);
  virtual void CalcAxisSizes();
  virtual void SetView(GraphView* gv);
  virtual Area aPoint(CPoint pt) const;
  void drawTitle(CDC*);
	RoseDiagramLayer* rdl(int i) const { return static_cast<RoseDiagramLayer*>(agl[i]); }
	RoseDiagramAxis *rdaxX,*rdaxY;
};

#endif 
