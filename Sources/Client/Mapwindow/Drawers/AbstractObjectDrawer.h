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
		void setDataSource(void *bmap, int options=0);
		
	protected:
		IlwisObjectPtr *obj;
		IlwisObject *object; //  only to keep reference to object alive
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

	class _export DisplayOptionsForm : public FormBaseDialog {
	public:
		DisplayOptionsForm(AbstractObjectDrawer *dr,CWnd *par, const String& title);
		afx_msg virtual void OnCancel();
		int exec();
		virtual void apply();
	protected:
		void updateViews();
		AbstractObjectDrawer *drw;
		LayerTreeView *view;
	};

	class TransparencyForm : public DisplayOptionsForm {
		public:
		TransparencyForm(CWnd *wPar, AbstractObjectDrawer *dr);
		void apply(); 
	private:
		int transparency;
	};

	
}