#pragma once



namespace ILWIS{

	class AbstractObjectDrawer : public ComplexDrawer {
		friend void displayOptionsText(NewDrawer &drw, CWnd *parent);
	public:

		virtual ~AbstractObjectDrawer();
		virtual void prepare(PreparationParameters *pp);
		IlwisObject getObject() const;
		void addDataSource(void *bmap, int options=0);
		
	protected:
		IlwisObjectPtr *obj;
		IlwisObject *object; //  only to keep reference to object alive

		AbstractObjectDrawer(DrawerParameters *parms, const String& name);
		//HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		//void displayOptionsText(CWnd *parent);
		void displayOptionSubItem(CWnd *parent);
	};


	class _export DisplayOptionsForm : public FormBaseDialog {
	public:
		DisplayOptionsForm(ComplexDrawer *dr,CWnd *par, const String& title);
		afx_msg virtual void OnCancel();
		int exec();
		virtual void apply();
	protected:
		void updateMapView();
		ComplexDrawer *drw;
		LayerTreeView *view;
	};

}