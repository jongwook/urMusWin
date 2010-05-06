#ifndef __URTEXTURE_H__
#define __URTEXTURE_H__

#include <windows.h>
#include <gl/gl.h>

#include "urImage.h"
#include "urFont.h"


typedef enum {
	kTexture2DPixelFormat_Automatic = 0,
	kTexture2DPixelFormat_RGBA8888,
	kTexture2DPixelFormat_RGB565,
	kTexture2DPixelFormat_A8,
} Texture2DPixelFormat;

typedef enum {
   TextAlignmentLeft,
   TextAlignmentCenter,
   TextAlignmentRight,
} TextAlignment;

typedef struct tagCGPoint {
	float x;
	float y;
} CGPoint;

typedef struct tagCGSize {
	float width;
	float height;
} CGSize;

typedef struct tagCGRect {
   CGPoint origin;
   CGSize size;
} CGRect;


typedef enum {
   LineBreakModeWordWrap = 0,
   LineBreakModeCharacterWrap,
   LineBreakModeClip,
   LineBreakModeHeadTruncation,
   LineBreakModeTailTruncation,
   LineBreakModeMiddleTruncation,
} LineBreakMode;




class urTexture
{
protected:
	GLuint name;
	unsigned int width, height;
	GLenum format;	// GL_RGB or GL_RGBA
	urFont font;
	string str;

public:
	urTexture(const void *data, GLenum format, unsigned int width, unsigned int height);
	urTexture(urImage *image);
	urTexture(const char *str, const char *fontname, unsigned int size);
	~urTexture(void);

	GLuint getName() { return name; }
	unsigned int getWidth() { return width; }
	unsigned int getHeight() { return (name)?height:(font.bLoadedOk)?font.getLineHeight():0; }
	void bind() { glBindTexture(GL_TEXTURE_2D, name); }
	void unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
	
	void drawInRect(CGRect rect);
	void drawAtPoint(CGPoint point, bool tile);
};

#endif // __URTEXTURE_H__