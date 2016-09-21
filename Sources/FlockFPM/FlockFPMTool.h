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
Bas Retsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

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
// FlockFPMTool.h: interface for the FlockFPMTool class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

ILWIS::DrawerTool *createFlockFPMTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class FieldFlockColumn;
class FieldTableCreate;
class AreaSelector;

namespace ILWIS {

	class FlockFPMDrawer;

	class FlockFPMTool : public DrawerTool {
	public:
		FlockFPMTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		virtual ~FlockFPMTool();
		String getMenuString() const;
	protected:
		void changeFlockFPM();
	};

	class FlockFPMSelectTool;
	class LayerDrawer;

	class FlockFPMForm : public DisplayOptionsForm {
	public:
		FlockFPMForm(CWnd *wPar, ComplexDrawer *dr, HTREEITEM hti);
		~FlockFPMForm();
		void apply();
	private:
		void FillWithSuitableColumns(Table & tbl);
		FlockFPMDrawer * flockFPMDrawer;
		FieldIntSliderEx * slider;
		FieldColor * colorPicker;
		CheckBox * cbVisible;
		CheckBox * cbSelectable;
		FieldFlockColumn * fldCol;
		FieldTableCreate* ftcFlockTable;
		FieldGroup * fgStoreColumn;
		int setTransparency(Event *ev);
		int setColor(Event *ev);
		int setVisible(Event *ev);
		int setSelectable(Event *ev);
		int setDelta(Event *ev);
		int ColCallBackFunc(Event *ev);
		int CallBackAddColumn(Event *ev);
		int StoreFlocks(Event *ev);
		int WriteFlockTable(Event *ev);
		std::string getPointsString(vector<int> & pointIDs);
		int transparency;
		bool visible;
		bool selectable;
		Color color;
		int mu;
		double epsilon;
		int iSampleInterval;
		int delta;
		String sFlockCol;
		String sFlockTable;
		//double rSampleInterval;
		bool fInit;
		FlockFPMSelectTool * ffpmSelect;
	};

	class FlockFPMSelectTool : public MapPaneViewTool {
	public:
		FlockFPMSelectTool(ZoomableView* zv, BaseMapPtr *bmpp, vector<long> * selRaws, LayerDrawer *ldrw, CheckBox * cbSel);
		~FlockFPMSelectTool();
		void setSelectable(bool selectable);
		bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		bool OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
		void OnMouseMove(UINT nFlags, CPoint point);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnEscape();
		virtual void Stop();

	protected:
		void FeatureAreaSelected(CRect rect);
		bool fCtrl;
		bool fShift;
		BaseMapPtr *bmapptr;
		vector<long> * selectedRaws;
		AreaSelector *as;
		LayerDrawer *layerDrawer;
		CheckBox * cbSelectable;
	};
}