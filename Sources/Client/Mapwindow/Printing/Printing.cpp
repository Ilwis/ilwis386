#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Engine\Map\Mapview.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "textbmpDoc.h"
#include "MainFrm.h"
#include "textbmpDoc.h"
#include "textbmpView.h"
#include "printing.h"

Printing::Printing(){
}

void Printing::print(MapCompositionDoc *doc){
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new PrintDocTemplate(
		RUNTIME_CLASS(CTextbmpDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CTextbmpView));
	IlwWinApp()->AddDocTemplate(pDocTemplate);
	CTextbmpDoc *tdoc = (CTextbmpDoc *)pDocTemplate->CreateNewDocument();
	tdoc->setMapDoc(doc);
	CMainFrame *frame = (CMainFrame *)pDocTemplate->CreateNewFrame(tdoc,0);
	frame->InitialUpdateFrame(tdoc, TRUE);

}

PrintDocTemplate::PrintDocTemplate(CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass) :
CSingleDocTemplate(1,pDocClass, pFrameClass, pViewClass) 
{
}

CFrameWnd* PrintDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther){
	if (pDoc != NULL)
		ASSERT_VALID(pDoc);
	// create a frame wired to the specified document

	//	ASSERT(m_nIDResource != 0); // must have a resource ID to load from
	CCreateContext context;
	context.m_pCurrentFrame = pOther;
	context.m_pCurrentDoc = pDoc;
	context.m_pNewViewClass = m_pViewClass;
	context.m_pNewDocTemplate = this;

	if (m_pFrameClass == NULL)
	{
		TRACE0("Error: you must override CDocTemplate::CreateNewFrame.\n");
		ASSERT(FALSE);

		return NULL;
	}
	CFrameWnd* pFrame = (CFrameWnd*)m_pFrameClass->CreateObject();
	if (pFrame == NULL)
	{
		TRACE1("Warning: Dynamic create of frame %hs failed.\n",
			m_pFrameClass->m_lpszClassName);
		return NULL;
	}
	ASSERT_KINDOF(CFrameWnd, pFrame);

	if (context.m_pNewViewClass == NULL)
		TRACE0("Warning: creating frame with no default view.\n");

	String clsName("ILWIS");
	if (!pFrame->Create(NULL, clsName.c_str(), WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE, CFrameWnd::rectDefault,
		::AfxGetMainWnd(), NULL, 0, &context)) 
	{
		TRACE0("Warning: BaseDocTemplate couldn't create a frame.\n");
		// frame will be deleted in PostNcDestroy cleanup
		return NULL;
	}
	// it worked !
	return pFrame;
}