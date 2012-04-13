// Texture.h: interface for the Texture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURE_WMS_H__E4F62490_7FB3_423D_B131_349672E6F490__INCLUDED_)
#define AFX_TEXTURE_WMS_H__E4F62490_7FB3_423D_B131_349672E6F490__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <GL/gl.h>

#include "Engine\Drawers\ComplexDrawer.h" // for DrawMethod
#include "Drawers\Texture.h"

namespace ILWIS {

	class DrawingColor;
	class DrawerContext;

	class WMSTexture : public Texture
	{
	public:
		WMSTexture(const Map & mp, const DrawingColor * drawColor, const ComplexDrawer::DrawMethod drm, const CoordBounds & cb, const CoordBounds & _cbImageBounds);
		virtual void CreateTexture(DrawerContext * drawerContext, bool fInThread, volatile bool * fDrawStop);
		virtual void ReCreateTexture(DrawerContext * drawerContext, bool fInThread, volatile bool * fDrawStop);
		virtual void BindMe(DrawerContext * drawerContext); // To be called before glBegin
		const CoordBounds & cb() {return cbBounds;};

	protected:
		virtual bool DrawTexture(int textureSize, char * outbuf, volatile bool* fDrawStop);
		const CoordBounds cbBounds;
		const CoordBounds cbImageBounds;
	};
}

#endif // !defined(AFX_TEXTURE_WMS_H__E4F62490_7FB3_423D_B131_349672E6F490__INCLUDED_)
