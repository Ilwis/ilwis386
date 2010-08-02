#include "Headers\toolspch.h"
#include "Client\Base\OpenGLFont.h"
#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftoutln.h"
#include "freetype/fttrigon.h"


OpenGLFont::OpenGLFont(const String& name, int height) {
	loadFont(name, height);
}

OpenGLFont::~OpenGLFont() {
    glDeleteTextures(128, textures);
    glDeleteLists(displaylist, 128);
}

void OpenGLFont::renderText(const Coordinate& c, const String& text) {
	double z = c.z == rUNDEF ? 0 : c.z;
	glColor3ub(1.0,0,0);
	glRasterPos2d(c.x,c.y);
	glListBase(displaylist);
	//glCallLists(text.size(), GL_UNSIGNED_BYTE, text.scVal());
	glCallLists(3, GL_UNSIGNED_BYTE, "AAP");
}

void OpenGLFont::loadFont(const String& name, int height) {
	FT_Library lib = 0;
	FT_Face face;
	FT_Glyph glyph;
	FT_Bitmap* bitmap;
	FT_BitmapGlyph* bitglyph;
	int w, h, xpos, ypos;
	unsigned char i;
	GLubyte* buffer;
	float texpropx, texpropy;

	glGenTextures(128, textures);
	displaylist = glGenLists(128);

	int err = FT_Init_FreeType(&lib);
	err = FT_New_Face(lib, name.scVal(), 0, &face);
	err = FT_Set_Char_Size(face, height << 6, height << 6, 96, 96);

	for (i=0;i<128;i++){
		// get bitmap
		err = FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT);
		err = FT_Get_Glyph(face->glyph, &glyph);
		err = FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		bitglyph = (FT_BitmapGlyph*)&glyph;
		bitmap = &((FT_BitmapGlyph)glyph)->bitmap;
		w= 1;
		while(w<bitmap->width)
			w *= 2;
		h= 1;
		while(h<bitmap->rows)
			h *= 2;
		// make bitmap
		buffer = (GLubyte*)calloc(sizeof(GLubyte)*2*w*h, 1);
		for (ypos=0;ypos<bitmap->rows;ypos++){
			for (xpos=0;xpos<bitmap->width;xpos++){
				buffer[2*(xpos+ypos*w)] = bitmap->buffer[xpos+ypos*bitmap->width];
				buffer[2*(xpos+ypos*w)+1] = bitmap->buffer[xpos+ypos*bitmap->width];
			}
		}

		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, buffer);

		// make display list (for nicely formatted text)
		glPushMatrix();
		glNewList(displaylist+i, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glPushMatrix();
		glTranslatef((*bitglyph)->left, (*bitglyph)->top-bitmap->rows, 0);
		// proportions of the texture that are the font (not padding)
		texpropx = (float)bitmap->width / (float)w;
		texpropy = (float)bitmap->rows / (float)h;

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);                glVertex3f(0, bitmap->rows, 0);
		glTexCoord2f(0, texpropy);         glVertex3f(0, 0, 0);
		glTexCoord2f(texpropx, texpropy);  glVertex3f(bitmap->width, 0, 0);
		glTexCoord2f(texpropx, 0);         glVertex3f(bitmap->width, bitmap->rows, 0);
		glEnd();
		glPopMatrix();
		glTranslatef(face->glyph->advance.x >> 6, 0, 0);
		glEndList();
		glPopMatrix();

		free(buffer);
	}

	FT_Done_Face(face);

	FT_Done_FreeType(lib);

}