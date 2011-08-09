#pragma once 

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Engine\Map\Mapview.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "textbmpDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTextbmpDoc, CDocument)

BEGIN_MESSAGE_MAP(CTextbmpDoc, CDocument)
END_MESSAGE_MAP()

CTextbmpDoc::CTextbmpDoc()
{

}

CTextbmpDoc::~CTextbmpDoc()
{
}

BOOL CTextbmpDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

void CTextbmpDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

MapCompositionDoc * CTextbmpDoc::getMapDoc() const{
	return mdoc;
}

void CTextbmpDoc::setMapDoc(MapCompositionDoc *d){
	mdoc = d;
}


#ifdef _DEBUG
void CTextbmpDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTextbmpDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


