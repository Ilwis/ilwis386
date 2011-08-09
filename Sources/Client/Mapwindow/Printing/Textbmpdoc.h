#pragma once

class MapCompositionDoc;

class CTextbmpDoc : public CDocument
{
protected: // create from serialization only
	CTextbmpDoc();
	MapCompositionDoc *mdoc;
	DECLARE_DYNCREATE(CTextbmpDoc)


public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	MapCompositionDoc * getMapDoc() const;
	void setMapDoc(MapCompositionDoc *d);
	virtual ~CTextbmpDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

protected:
	DECLARE_MESSAGE_MAP()
};

