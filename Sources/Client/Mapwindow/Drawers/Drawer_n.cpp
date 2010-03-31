#include "headers\toolspch.h"
#include "Drawer_n.h"

using namespace ILWIS;

AbstractDrawer::AbstractDrawer(DrawerContext *context, const String& ty) : type(ty), drawcontext(context){
}

String AbstractDrawer::getType() const {
	return type;
}

AbstractDrawer::~AbstractDrawer() {
	for(int i=0; i < drawers.size(); ++i)
		delete drawers[i];
}
//----------------------------------------------------------------------------




