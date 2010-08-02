#pragma once

#include <gl/gl.h>
#include <gl/glu.h>
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Domain\dm.h"

class LayerTreeView;
class LayerTreeItem;
class SetChecks;
class RootDrawer;

namespace ILWIS {
	class DrawerContext;
	struct PreparationParameters;

	class _export SimpleDrawer : public NewDrawer {
	public:
		String getType() const;
		DrawerContext *getDrawerContext();
		void draw(bool norecursion = false) = 0;
		void prepare(PreparationParameters *);
		void prepareChildDrawers(PreparationParameters *) {};
		int getDrawerCount() const { return 0;}
		NewDrawer * getDrawer(int index) { return 0;}
		virtual String addDrawer(NewDrawer *drw) { return sUNDEF; }
		virtual void removeDrawer(const String& did) {}
		virtual void addPostDrawer(int order, NewDrawer *drw) {}
		virtual void addPreDrawer(int order, NewDrawer *drw) {}
		virtual void addDataSource(void *,int options=0) {};
		virtual void removeDataSource(void *){}
		String getId() const;
		NewDrawer *getDrawer(const String& did) { return 0;}
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
		virtual int getUICode() const { return iUNDEF;}
		virtual void setUICode(int c) {};
		virtual NewDrawer *getRootDrawer() const { return (NewDrawer *)rootDrawer; }

		CCriticalSection cs;
	protected:
	
		DrawerContext *drawcontext;
		NewDrawer *parentDrawer;
		RootDrawer *rootDrawer;
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



