#pragma once



namespace ILWIS{

	class _export AbstractObjectDrawer : public ComplexDrawer {
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
		void displayOptionSubItem(CWnd *parent);
	};
}