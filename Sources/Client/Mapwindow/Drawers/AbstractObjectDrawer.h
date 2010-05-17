#pragma once



namespace ILWIS{

	class AbstractObjectDrawer : public AbstractDrawer {
		friend void displayOptionsText(NewDrawer &drw, CWnd *parent);
	public:

		virtual ~AbstractObjectDrawer();
		virtual void prepare(PreparationParameters *pp);
		IlwisObject getObject() const;
		int getTransparency() const;
		void setTransperency(int value);
		void setDataSource(void *bmap);
		
	protected:
		IlwisObjectPtr *obj;
		int transparency; // in percentage, 100% means transparent
		Color clrText;
		String sFaceName;
		short iFHeight;
		short iFWeight;
		short iFStyle;
		short iWidth;
		bool hasText;

		AbstractObjectDrawer(DrawerParameters *parms, const String& name);
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
	};
	class TransparencyForm : public FormBaseDialog {
		public:
		TransparencyForm(CWnd *wPar, AbstractObjectDrawer *dr);
		int exec();
		afx_msg virtual void OnCancel(); 
	private:
		int transparency;
		AbstractObjectDrawer *drw;

	};

	
}