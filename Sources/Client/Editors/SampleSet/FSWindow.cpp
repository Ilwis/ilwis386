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
/* FSWindow.cpp : Implementation of FSDrawer and FeatureSpaceWindow

	*/
#include "Client\Headers\formelementspch.h"
#include "Headers\constant.h"
#include "Client\Mapwindow\InfoLine.h"
#include "Client\GraphWindow\GraphAxis.h"
#include "Client\GraphWindow\GraphLayer.h"
#include "Client\GraphWindow\GraphForms.h"
#include "Client\GraphWindow\GraphDrawer.h"
#include "Client\GraphWindow\GraphLegend.h"
#include "Engine\SampleSet\FSPACE.H"
#include "Client\FormElements\syscolor.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Editors\SampleSet\FSWindow.h"
#include "Client\Editors\Utils\GeneralBar.h"


class FeatureSpaceX : public FeatureSpace
{
public:
	FeatureSpaceX(const SampleSet& sms, int b1, int b2) :
			FeatureSpace(sms, b1, b2) {}
	virtual void    Changed(XY& xy) {}
};

BEGIN_MESSAGE_MAP(FeatureSpaceWindow, GraphView)
//  ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


#define sMen(ID) ILWSF("men",ID).c_str()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 
//FSLayer

class FSLayer : public CartesianGraphLayer
{
public:
	FSLayer(CartesianGraphDrawer* cgd, const SampleSet& sms, int b1, int b2);
	~FSLayer();
	virtual void draw(CDC*);
	void CreateFS(const SampleSet& sms, int b1, int b2);
	FeatureSpace* fs;
	Representation rpr;
};

FSLayer::FSLayer(CartesianGraphDrawer* cgd, const SampleSet& sms, int b1, int b2)
: CartesianGraphLayer(cgd)
{
	CreateFS(sms, b1, b2);;
}

FSLayer::~FSLayer() 
{
	delete fs;
}


void FSLayer::draw(CDC* cdc)
{
	// show fs
	ByteBuf bb(256);
	Array<Color> acol(256);
	for (int i=0; i <= 254; i++)
		acol[i] = rpr->clrRaw(i);
	acol[255] = Color(SysColor(COLOR_WINDOWTEXT));

	double dx = cgd->gaxX->rConv(1) - cgd->gaxX->rConv(0);
	double dy = cgd->gaxYLeft->rConv(0) - cgd->gaxYLeft->rConv(1);
	CPoint ps1 = cgd->ptPos(0, 0);
	CPoint ps2 = cgd->ptPos(dy, dx);
	CSize cs(ps2.x-ps1.x+2,ps2.y-ps1.y+2);
	for (int i=0; i <= 255; i++) {
		fs->GetLineRaw(i, bb);
		for (int j=0; j <= 255; j++) {
			if (bb[j] > 0) {
				Color col = acol[bb[j]];
				CBrush br(col);
				CPen pen(PS_SOLID, 1, col);
				CBrush* brOld = cdc->SelectObject(&br);
				CPen* penOld = cdc->SelectObject(&pen);
				double x, y;
				x = cgd->gaxX->rConv(j);
				y = cgd->gaxYLeft->rConv(i);
				CPoint pt = cgd->ptPos(y, x);
				cdc->Rectangle(pt.x, pt.y, pt.x+cs.cx, pt.y+cs.cy);
				cdc->SelectObject(penOld);
				cdc->SelectObject(brOld);
			}
		}
	}
}

void FSLayer::CreateFS(const SampleSet& sms, int b1, int b2)
{
	fs = new FeatureSpaceX(sms, b1, b2);
	fs->Compute();
	rpr = sms->dc()->rpr();
}

// FSDrawer

class FSDrawer : public CartesianGraphDrawer
{
public:
	FSDrawer(GraphView* gv) : CartesianGraphDrawer(gv) {}
	FSDrawer(GraphView* gv, const SampleSet& sms, int b1, int b2);
	void Init(const SampleSet& sms, int b1, int b2);
	void SaveSettings(const FileName& fn, const String& sSection);
	void LoadSettings(const FileName& fn, const String& sSection);
	virtual bool fConfigure();
	int iBand1();
	int iBand2();
};

FSDrawer::FSDrawer(GraphView* gv, const SampleSet& sms, int b1, int b2)
: CartesianGraphDrawer(gv)
{
  Init(sms, b1, b2);	
}

void FSDrawer::Init(const SampleSet& sms, int b1, int b2)
{
  grleg->fShow = false;
	gaxX->Set(Domain("image"));
	gaxX->SetMinMax(sms->mpl()->map(b1)->rrMinMax(BaseMapPtr::mmmCALCULATE));
	gaxX->dvrsData = DomainValueRangeStruct(Domain("image"));
	gaxX->sTitle = sms->mpl()->map(b1)->sName();
	gaxYLeft->Set(Domain("image"));
	gaxYLeft->SetMinMax(sms->mpl()->map(b2)->rrMinMax(BaseMapPtr::mmmCALCULATE));
	gaxYLeft->dvrsData = DomainValueRangeStruct(Domain("image"));
	gaxYLeft->sTitle = sms->mpl()->map(b2)->sName();
	gaxYRight->Set(Domain("image"));
	gaxYRight->SetMinMax(sms->mpl()->map(b2)->rrMinMax(BaseMapPtr::mmmCALCULATE));
	gaxYRight->dvrsData = DomainValueRangeStruct(Domain("image"));
	gaxYRight->sTitle = sms->mpl()->map(b2)->sName();
	agl.push_back(new FSLayer(this, sms, b1, b2));
}

bool FSDrawer::fConfigure()
{
  CartesianGraphOptionsForm frm(this, true); // axis info only
  return IDOK == frm.DoModal();

}

int FSDrawer::iBand1()
{
  FSLayer* fsl= dynamic_cast<FSLayer*>(agl[0]);
	return fsl->fs->iBand1();
}

int FSDrawer::iBand2()
{
  FSLayer* fsl= dynamic_cast<FSLayer*>(agl[0]);
	return fsl->fs->iBand2();
}

void FSDrawer::SaveSettings(const FileName& fn, const String& sSection)
{
	FSLayer* fsl= dynamic_cast<FSLayer*>(agl[0]);
	FeatureSpace* fs = fsl->fs;
	ObjectInfo::WriteElement(sSection.c_str(), "X", fn, fs->iBand1());
	ObjectInfo::WriteElement(sSection.c_str(), "Y", fn, fs->iBand2());
	CartesianGraphDrawer::SaveSettings(fn, sSection);
}

void FSDrawer::LoadSettings(const FileName& fn, const String& sSection)
{
	CartesianGraphDrawer::LoadSettings(fn, sSection);
	int b1, b2;
	ObjectInfo::ReadElement(sSection.c_str(), "X", fn, b1);
	ObjectInfo::ReadElement(sSection.c_str(), "Y", fn, b2);
	SampleSet sms(fn);
	FSLayer* fsl = new FSLayer(this, sms, b1, b2);
	agl.push_back(fsl);
}

// FeatureSpaceWindow

FeatureSpaceWindow::FeatureSpaceWindow(const SampleSet& sms, int b1, int b2) 
: fsdrw(0)
{
	fsdrw = new FSDrawer(this, sms, b1, b2);
	grdrw = fsdrw;
}

FeatureSpaceWindow::FeatureSpaceWindow()
: fsdrw(0)
{
}

FeatureSpaceWindow::~FeatureSpaceWindow() 
{
  delete fsdrw;
}


void FeatureSpaceWindow::OnContextMenu(CWnd* pWnd, CPoint point)
{
  CMenu men, menSub;
	men.CreatePopupMenu();
	add(ID_GRPH_OPTIONS);
	add(ID_FILE_PRINT);
	add(ID_GRPH_COPY);
  men.AppendMenu(MF_SEPARATOR);
	add(ID_NORMAL);
	add(ID_ZOOMIN);
	add(ID_ZOOMOUT);
	add(ID_PANAREA);
	add(ID_GRPH_FITINWINDOW);
	GeneralBar* gb = dynamic_cast<GeneralBar*>(GetParent());
	if (gb) {
	  men.AppendMenu(MF_SEPARATOR);
		add(ID_ALLOWDOCKING);
		men.CheckMenuItem(ID_ALLOWDOCKING, gb->fAllowDocking() ? MF_CHECKED : MF_UNCHECKED);
	}
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

int FeatureSpaceWindow::iBand1()
{
	return fsdrw->iBand1();
}

int FeatureSpaceWindow::iBand2()
{
	return fsdrw->iBand2();
}

void FeatureSpaceWindow::SaveSettings(const FileName& fn, int iFSWindow)
{
	String sSection("FSWindow%i", iFSWindow);
	fsdrw->SaveSettings(fn, sSection);
}

void FeatureSpaceWindow::LoadSettings(const FileName& fn, int iFSWindow)
{
	String sSection("FSWindow%i", iFSWindow);
	fsdrw = new FSDrawer(this);
	fsdrw->LoadSettings(fn, sSection);
	grdrw = fsdrw;
}

