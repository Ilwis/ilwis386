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
#ifndef ILWGEO3DED_H
#define ILWGEO3DED_H

class _export GeoRef3DForm;
class _export GeoRef3DEditor;
class _export GeoRef3DDirectionPane;
class _export GeoRef3DDirectionTool;

class GeoRef3DEditorBar : public CSizingControlBar
{
	
public:
	enum ItemToUpdate{ituFORM=1, ituMINIPANE=2, ituANY=0};
	enum ValueToUpdate{vtuVIEWPOINT, vtuFOCALPOINT, vtuANY};

	GeoRef3DEditorBar();
	~GeoRef3DEditorBar();
	BOOL Create(MapPaneView *pane, GeoRef3D *grf);
	void SetValue(int items, ValueToUpdate v, double rValue1, double rValue2=rUNDEF);
	void RedrawMiniMap();
	void OnSetViewPoint();
	MapPaneView *MainPane();
	void OnExtend()	;

protected:
	MapPaneView      *mpMapPane;
	MapCompositionDoc *mcdMiniMap;
	GeoRef3DDirectionPane *mpvMiniMap;
	GeoRef3DForm    *form;
	CButton         butExtend;
	CButton         butViewPoint;
	Bevel           stSeperator;
  GeoRef3D*   gr3d;

	void OnSize( UINT nType, int cx, int cy );
	DECLARE_MESSAGE_MAP()			
	
};

class GeoRef3DConfigForm : public FormWithDest
{
	public:
		GeoRef3DConfigForm(CWnd *par);

	private:
		Color clrViewAxis;
		Color clrAngle;

		int exec();
};	

class GeoRef3DEditor : public Editor
{
    public:
			GeoRef3DEditor(MapPaneView *);
			~GeoRef3DEditor();
			void        Copy();
			void        Clear();
			void        Paste();
			//        Menu*      menEdit();
			//        Menu*      menEditor(Menu *);
			//        void        SetupMenu(Menu*);
			int         Update(Event *ev);
			int         draw(CDC *dc, zRect, Positioner* psn, bool volatile *);
			String      sTitle() const;

    protected:
			GeoRef3D					*grf;
			GeoRef3DEditorBar bar3DEdit;
			bool              fSetFocalPoint;
			void							OnConfigure();
			void              OnSetViewPoint();
			bool							OnLButtonUp(UINT nFlags, CPoint point);			
			void              OnSetFocalPoint();
			void              OnUpdateGrd3DBar(CCmdUI* pCmdUI);
			void              OnGrd3DBar();
			bool              OnContextMenu(CWnd* pWnd, CPoint point) ;

	DECLARE_MESSAGE_MAP()						
};

class GeoRef3DForm : public FormBaseView
{
  friend class GeoRef3DEditorBar;

public:
  GeoRef3DForm(MapPaneView *win, GeoRef3D* grf, GeoRef3DEditorBar * );
  ~GeoRef3DForm();
  int         exec();
  int         Redraw(Event *ev);
  int         Grid(Event *ev);
	void				CreateForm();
//  long dispatch(Event*);

protected:
  GeoRef3D*   gr3d;
  int RotCallBack(Event*);
  int LocCallBack(Event*);
  int CallBack(Event*);
	int GridDistCallBack(Event *ev);
	int DispOptCallBack(Event *ev);
	BOOL PreTranslateMessage(MSG* pMsg)	;
  FieldCoord  *fcLoc; // *fcLocY;
	FieldCoord   *fcView; //, *fcViewY;
  FieldReal   *frLocHeight; 
  FieldReal   *frViewAngle;
  FieldReal   *frScaleHeight;
  FieldReal   *frHorRot;
  FieldReal   *frVerRot;
  FieldReal   *frDistance;
  MapPaneView     *pane;
	GeoRef3DEditorBar	*bar;
private:        
  bool fInsideCallback;
};

#endif
