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
#ifndef TREESELECTOR_H
#define TREESELECTOR_H

#include "Client\Base\ZappToMFC.h"

class Tree : public CTreeCtrl, public BaseZapp
{
public:
	Tree(FormEntry *f,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );

private:
	void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)	;
	void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);

  DECLARE_MESSAGE_MAP();

};


class TreeSelector: public FormEntry
{
public:
  _export ~TreeSelector();
  _export TreeSelector(FormEntry* par, int depth, bool _expandAll=false);
  void _export show(int);
  virtual void StoreData() {};
  void setDirty() { tree->Invalidate(); }
	void _export Add(const String& sValue, HTREEITEM hti=TVI_ROOT, DWORD data=0, bool fLeaf=false, int depth=0);
	DWORD _export GetData(HTREEITEM hti);
	String _export sBranchValue(HTREEITEM _it=0);
	String _export sLeafValue();
	bool _export fValid();
	void _export SetNotifyExpansion(NotifyProc np, FormEntry *altHandler=NULL);
	void setDynamicNodes(bool fDyn) { fDynamic = true; }
	DWORD _export GetBranchValueData();
	void _export expand(HTREEITEM hti, int depth);
	void _export SelectNode(const String& path,HTREEITEM hti=0);
	void _export ExpandTo(int depth=0);

protected:
  void _export create();
  void SetFocus() { tree->SetFocus(); } // overriden
  virtual String sName(int id) { return String();}
  Tree *tree;
  DWORD style;
  bool expandAll;
  NotifyProc npExpansion;
  FormEntry *alternativeHandler;
  bool fDynamic;
  int maxExpansionDepth;
  bool keepSelection;
};

#endif

