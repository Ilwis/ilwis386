// TextureHeap.h: interface for the TextureHeap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WMS_TEXTUREHEAP_H__7D250323_C396_4502_BC72_5038E8F85506__INCLUDED_)
#define AFX_WMS_TEXTUREHEAP_H__7D250323_C396_4502_BC72_5038E8F85506__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Engine\Drawers\Drawer_n.h" // for DrawMethod

#include <GL/gl.h>

namespace ILWIS {

	class WMSTextureHeap : public TextureHeap
	{
	public:
		Texture * GetTexture(const CoordBounds & cb, bool fInThread);
		Texture * GetTexture(const Coord & crd);
		void SetCBImage(const CoordBounds & cb);
	private:
		Texture * GenerateTexture(const CoordBounds & cb, bool fInThread);
		CoordBounds cbImage;
	};
}

#endif // !defined(AFX_WMS_TEXTUREHEAP_H__7D250323_C396_4502_BC72_5038E8F85506__INCLUDED_)
