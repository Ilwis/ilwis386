#pragma once

#include "drawer_n.h" 

class LayerTreeView;
class LayerTreeItem;
class SetChecks;

namespace ILWIS {
	class DrawerContext;
	struct PreparationParameters;

	class _export SimpleDrawer : public NewDrawer {
	public:
		String getType() const;
		DrawerContext *getDrawerContext();
		DrawerContext *getDrawerContext() const;
		bool draw(bool norecursion = false, const CoordBounds& cb=CoordBounds()) const = 0;
		void prepare(PreparationParameters *);
		virtual void addDataSource(void *,int options=0) {};
		virtual void removeDataSource(void *){}
		String getId() const;
		String getName() const;
		virtual void setName(const String&);
		bool isActive() const {return parentDrawer->isActive(); }
	    void setActive(bool yesno) {};
		DrawMethod getDrawMethod() const { return parentDrawer->getDrawMethod(); }
		void setDrawMethod(DrawMethod method=drmINIT) {};
		virtual bool isEditable() const;
		virtual void setEditable(bool yesno);
		bool hasInfo() const;
		void setInfo(bool yesno);
		NewDrawer *getParentDrawer() const;
		double getTransparency() const { return parentDrawer->getTransparency();}
		void setTransparency(double value) {};
		virtual NewDrawer *getRootDrawer() const { return (NewDrawer *)rootDrawer; }

		CCriticalSection cs;
	protected:
	
		DrawerContext *drawcontext;
		NewDrawer *parentDrawer;
		NewDrawer *rootDrawer;
		String type;
		String id;
		String name;
		bool info;
		bool editable;

		SimpleDrawer(DrawerParameters *context, const String& ty);
		SimpleDrawer();
		virtual ~SimpleDrawer();
	private:
		void init();
	} ;

}



