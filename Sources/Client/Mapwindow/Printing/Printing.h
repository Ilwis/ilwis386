#pragma once

class Printing {
public:
	Printing();
	void print(MapCompositionDoc *doc);
};

class PrintDocTemplate : public CSingleDocTemplate {
public:
	PrintDocTemplate(CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
	virtual CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther);
};