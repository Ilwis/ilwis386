// FallbackView.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "FallbackView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FallbackView

IMPLEMENT_DYNCREATE(FallbackView, FormBaseView)

FallbackView::FallbackView()
{
	fbs |= fbsNOCANCELBUTTON;
}

void FallbackView::CreateForm()
{
  new StaticText(root, "There is no viewer for this object");
}  

FallbackView::~FallbackView()
{
}
