//
//  EAGLView.m
//  urMus
//
//  Created by Georg Essl on 6/20/09.
//  Copyright Georg Essl 2009. All rights reserved. See LICENSE.txt for license details.
//

//#import <QuartzCore/QuartzCore.h>
//#import <OpenGLES/EAGLDrawable.h>

#include "opengl.h"
#include "urAPI.h"
//#include "Texture2d.h"
#include "urTexture.h"
#include "MachTimer.h"
#include "urSound.h"

#include <cmath>


#define LOAD_FUNC(type, name) name = (type) wglGetProcAddress(#name);

/*
PFNGLISRENDERBUFFEREXTPROC             glIsRenderbufferEXT;
PFNGLBINDRENDERBUFFEREXTPROC           glBindRenderbufferEXT;
PFNGLDELETERENDERBUFFERSEXTPROC        glDeleteRenderbuffersEXT;
PFNGLGENRENDERBUFFERSEXTPROC           glGenRenderbuffersEXT;
PFNGLRENDERBUFFERSTORAGEEXTPROC        glRenderbufferStorageEXT;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT;
PFNGLISFRAMEBUFFEREXTPROC              glIsFramebufferEXT;
PFNGLBINDFRAMEBUFFEREXTPROC            glBindFramebufferEXT;
PFNGLDELETEFRAMEBUFFERSEXTPROC         glDeleteFramebuffersEXT;
PFNGLGENFRAMEBUFFERSEXTPROC            glGenFramebuffersEXT;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC     glCheckFramebufferStatusEXT;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC       glFramebufferTexture1DEXT;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC       glFramebufferTexture2DEXT;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC       glFramebufferTexture3DEXT;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC    glFramebufferRenderbufferEXT;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
PFNGLGENERATEMIPMAPEXTPROC             glGenerateMipmapEXT;
*/
void initExtensions() {
/*	char *extensions=(char *)glGetString(GL_EXTENSIONS);

	LOAD_FUNC(PFNGLISRENDERBUFFEREXTPROC,      glIsRenderbufferEXT)
	LOAD_FUNC(PFNGLBINDRENDERBUFFEREXTPROC,    glBindRenderbufferEXT)
	LOAD_FUNC(PFNGLDELETERENDERBUFFERSEXTPROC, glDeleteRenderbuffersEXT)
	LOAD_FUNC(PFNGLGENRENDERBUFFERSEXTPROC,    glGenRenderbuffersEXT)
	LOAD_FUNC(PFNGLRENDERBUFFERSTORAGEEXTPROC, glRenderbufferStorageEXT)
	LOAD_FUNC(PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC, glGetRenderbufferParameterivEXT)
	LOAD_FUNC(PFNGLISFRAMEBUFFEREXTPROC,      glIsFramebufferEXT)
	LOAD_FUNC(PFNGLBINDFRAMEBUFFEREXTPROC,    glBindFramebufferEXT)
	LOAD_FUNC(PFNGLDELETEFRAMEBUFFERSEXTPROC, glDeleteFramebuffersEXT)
	LOAD_FUNC(PFNGLGENFRAMEBUFFERSEXTPROC,    glGenFramebuffersEXT)
	LOAD_FUNC(PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC,  glCheckFramebufferStatusEXT)
	LOAD_FUNC(PFNGLFRAMEBUFFERTEXTURE1DEXTPROC,    glFramebufferTexture1DEXT)
	LOAD_FUNC(PFNGLFRAMEBUFFERTEXTURE2DEXTPROC,    glFramebufferTexture2DEXT)
	LOAD_FUNC(PFNGLFRAMEBUFFERTEXTURE3DEXTPROC,    glFramebufferTexture3DEXT)
	LOAD_FUNC(PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC, glFramebufferRenderbufferEXT)
	LOAD_FUNC(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC, glGetFramebufferAttachmentParameterivEXT)
	LOAD_FUNC(PFNGLGENERATEMIPMAPEXTPROC, glGenerateMipmapEXT)
*/

}

















#define USE_DEPTH_BUFFER 0

extern int currentPage;
extern urAPI_Region_t* firstRegion[];
extern urAPI_Region_t* lastRegion[];

extern urAPI_Region_t* UIParent;

/* The pixel dimensions of the backbuffer */
static GLint backingWidth;
static GLint backingHeight;

/* OpenGL names for the renderbuffer and framebuffers used to render to this view */
static GLuint viewRenderbuffer, viewFramebuffer;

/* OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist) */
static GLuint depthRenderbuffer;

static MachTimer* mytimer=NULL;

static const double ACCELEROMETER_RATE = 0.030;
static const int ACCELEROMETER_SCALE = 256;
static const int HEADING_SCALE = 256;
static const int LOCATION_SCALE = 256;

#define MAX_FINGERS 5

struct urDragTouch
{
	urAPI_Region_t* dragregion;
//	UITouch* touch1;
//	UITouch* touch2;
	float left;
	float top;
	float right;
	float bottom;
	float dragwidth;
	float dragheight;
	bool active;
	bool flagged;
	urDragTouch() { active = false; flagged = false; }
};

typedef struct urDragTouch urDragTouch_t;

#define MAX_DRAGS 5
urDragTouch_t dragtouches[MAX_DRAGS];

int FindDragRegion(urAPI_Region_t*region)
{
	for(int i=0; i< MAX_DRAGS; i++)
	{
		if(dragtouches[i].active && dragtouches[i].dragregion == region)
			return i;
	}
	return -1;
}

/*
void AddDragRegion(int idx, UITouch* t)
{
	if(dragtouches[idx].touch1 == NULL && dragtouches[idx].touch2!=t)
		dragtouches[idx].touch1 = t;
	else if(dragtouches[idx].touch2 == NULL && dragtouches[idx].touch1!=t)
		dragtouches[idx].touch2 = t;
}
*/

void ClearAllDragFlags()
{
	for(int i=0; i< MAX_DRAGS; i++)
	{
		dragtouches[i].flagged = false;
	}
}

int FindAvailableDragTouch()
{
	for(int i=0; i< MAX_DRAGS; i++)
		if(dragtouches[i].active == false)
			return i;
	
	int a=0;
	return -1;
}
/*
int FindDoubleDragTouch(UITouch* t1, UITouch* t2)
{
	for(int i=0; i< MAX_DRAGS; i++)
		if(dragtouches[i].active && ((dragtouches[i].touch1 == t1 && dragtouches[i].touch2 == t2) || (dragtouches[i].touch1 == t2 && dragtouches[i].touch2 == t1)))
		{
			return i;
		}
	return -1;
}

int FindSingleDragTouch(UITouch* t)
{
	for(int i=0; i< MAX_DRAGS; i++)
		if((dragtouches[i].active && dragtouches[i].touch1 == t ) || ( dragtouches[i].touch2 == t))
		{
			return i;
		}
	return -1;
}
*/

float cursorpositionx[MAX_FINGERS];
float cursorpositiony[MAX_FINGERS];

float cursorscrollspeedx[MAX_FINGERS];
float cursorscrollspeedy[MAX_FINGERS];

// Arrays to pass multi-touch finger to enter/leave handling. This allows smart decisions for enter/leave based on all fingers being considered. Should never be more than 5 and is fixed to avoid problems if MAX_FINGERS should be set to less for some reason.
int argmoved[5];
float argcoordx[5];
float argcoordy[5];
float arg2coordx[5];
float arg2coordy[5];

// The lua state
lua_State* lua;

// This is the texture to hold DPrint and lua error messages.
urTexture       *errorStrTex = NULL;

// Hard-wired screen dimension constants. This will soon be system-dependent variable!
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

// Enables/Disables that error and DPrint texture is rendered. Should always be on really.
#define RENDERERRORSTRTEXTUREFONT
// Enables/Disables debug output for multi-touch debugging. Should be always off now.
#undef DEBUG_TOUCH

	
// Various texture font strongs
/*
NSString *errorstr = @"";
NSString *textlabelstr = @"";
NSString *fontname = @"";
NSString *texturepathstr; // = @"Ship.png";
*/
static char errorstar[256];
static char textlabelstr[256];
static char fontname[256];
static char texturepathstr[256];
bool newerror = true;

// Below is modeled after GLPaint

#define kBrushOpacity		(1.0 / 3.0)
#define kBrushPixelStep		3
#define kBrushScale			2
#define kLuminosity			0.75
#define kSaturation			1.0

static urTexture* brushtexture = NULL;
static float brushsize = 1;

// 2D Painting functionality

// Brush handling

void SetBrushTexture(urTexture * texture)
{
	brushtexture = texture;
	brushsize = texture->getWidth();
}

void SetBrushSize(float size)
{
	brushsize = size;
}

float BrushSize()
{
	return brushsize;
}

void SetupBrush()
{
	if(brushtexture != NULL)
	{
		glBindTexture(GL_TEXTURE_2D, brushtexture->getName());
		glDisable(GL_DITHER);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
//		glDisable(GL_BLEND);
		// Make the current material colour track the current color
//		glEnable( GL_COLOR_MATERIAL );
		// Multiply the texture colour by the material colour.
//		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_POINT_SPRITE);
		glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	}	
	else
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}
	glPointSize(brushsize);
}

GLuint textureFrameBuffer;

// Render point drawing into a texture

void drawPointToTexture(urTexture *texture, float x, float y)
{
	y = 480 - y;
//	glGenFramebuffersEXT(1, &textureFrameBuffer);
//	glBindFramebufferEXT(GL_FRAMEBUFFER, textureFrameBuffer);
	
	// attach renderbuffer
//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getName(), 0);
	
	// unbind frame buffer
//	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
//	glBindFramebufferEXT(GL_FRAMEBUFFER, textureFrameBuffer);
	
	SetupBrush();
	
	static GLfloat		vertexBuffer[2];
	
	vertexBuffer[0] = x;
	vertexBuffer[1] = y;
	
	//Render the vertex array
	glVertexPointer(2, GL_FLOAT, 0, vertexBuffer);
	glDrawArrays(GL_POINTS, 0, 1);
}

// Render line drawing to a texture

void drawLineToTexture(urTexture *texture, float startx, float starty, float endx, float endy)
{
	starty = 480 - starty;
	endy = 480 - endy;
	// create framebuffer
//	glGenFramebuffersEXT(1, &textureFrameBuffer);
//	glBindFramebufferEXT(GL_FRAMEBUFFER, textureFrameBuffer);
	
	// attach renderbuffer
//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getName(), 0);
	
	// unbind frame buffer
//	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
//	glBindFramebufferEXT(GL_FRAMEBUFFER, textureFrameBuffer);

	SetupBrush();

	if(brushtexture==NULL)
	{
		static GLfloat		vertexBuffer[4];
		
		vertexBuffer[0] = startx;
		vertexBuffer[1] = starty;
		vertexBuffer[2] = endx;
		vertexBuffer[3] = endy;

		glLineWidth(brushsize);
		//Render the vertex array
		glVertexPointer(2, GL_FLOAT, 0, vertexBuffer);
		glDrawArrays(GL_LINES, 0, 2);
	}
	else
	{

		static GLfloat*		vertexBuffer = NULL;
		static unsigned int	vertexMax = 577; // Sqrt(480^2+320^2)
		unsigned int		vertexCount = 0, count,	i;
		
		//Allocate vertex array buffer
		if(vertexBuffer == NULL)
			vertexBuffer = (GLfloat*)malloc(vertexMax * 2 * sizeof(GLfloat));
		
		//Add points to the buffer so there are drawing points every X pixels
		count = max(ceilf(sqrtf((endx - startx) * (endx - startx) + (endy - starty) * (endy - starty)) / kBrushPixelStep), 1);
		for(i = 0; i < count; ++i) {
			if(vertexCount == vertexMax) {
				vertexMax = 2 * vertexMax;
				vertexBuffer = (GLfloat*)realloc(vertexBuffer, vertexMax * 2 * sizeof(GLfloat));
			}
			
			vertexBuffer[2 * vertexCount + 0] = startx + (endx - startx) * ((GLfloat)i / (GLfloat)count);
			vertexBuffer[2 * vertexCount + 1] = starty + (endy - starty) * ((GLfloat)i / (GLfloat)count);
			vertexCount += 1;
		}
		
		//Render the vertex array
		glVertexPointer(2, GL_FLOAT, 0, vertexBuffer);
//		glDrawArrays(GL_LINES, 0, vertexCount);
		glDrawArrays(GL_POINTS, 0, vertexCount);
	}
	
//	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
}

// Clear a texture with a given RGB color

void clearTexture(urTexture * texture, float r, float g, float b)
{
//	glGenFramebuffersEXT(1, &textureFrameBuffer);
//	glBindFramebufferEXT(GL_FRAMEBUFFER, textureFrameBuffer);
	
	// attach renderbuffer
//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getName(), 0);
	
	// unbind frame buffer
//	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
//	glBindFramebufferEXT(GL_FRAMEBUFFER, textureFrameBuffer);
	
	glClearColor(r, g, b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

// Create a texture instance for a given region

void instantiateTexture(urAPI_Region_t* t)
{
	sprintf(texturepathstr,"..\\textures\\%s", t->texture->texturepath);
	urImage *textureimage = new urImage(texturepathstr);
	if(textureimage->getBuffer()) {
		t->texture->backgroundTex=new urTexture(textureimage);
	}
//	UIImage* textureimage = [UIImage imageNamed:texturepathstr];
/*	if(textureimage)
	{
		CGSize rectsize;
		rectsize.width = t->width;
		rectsize.height = t->height;
		t->texture->backgroundTex = 0; // TODO: new Texture2D(textureimage, rectsize);
	}*/
}

// Convert line break modes to UILineBreakMode enums

LineBreakMode tolinebreakmode(int wrap)
{
	switch(wrap)
	{
		case WRAP_WORD:
			return LineBreakModeWordWrap;
		case WRAP_CHAR:
			return LineBreakModeCharacterWrap;
		case WRAP_CLIP:
			return LineBreakModeClip;
	}
	return LineBreakModeWordWrap;
}
#define coord(x,y) glTexCoord2f((x),(y));glVertex2f((x),(y))

// Main drawing loop. This does everything but brew coffee.
//#include <gl/glut.h>
void drawView() {/*
glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Background color
    glClear(GL_COLOR_BUFFER_BIT);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
    
	glColor3f(1.0,1.0,1.0);
	glRectf(0.0,0.0,0.5,0.5);
return;
//***/
//
	urs_PullVis(); // update vis data before we call events, this way we have a rate based pulling that is available in all events.

	// init mytiper
	if(!mytimer) {
		mytimer=new MachTimer();
		mytimer->start();
	}
	// Clock ourselves.
	if(mytimer) {
 		float elapsedtime = mytimer->elapsedSec();
		mytimer->start();
		callAllOnUpdate(elapsedtime); // Call lua APIs OnUpdates when we render a new region. We do this first so that stuff can still be drawn for this region.
	}
	
	CGRect  bounds = {{0, 0}, {SCREEN_WIDTH,SCREEN_HEIGHT}};
	
    // Replace the implementation of this method to do your own custom drawing
    
    GLfloat squareVertices[] = {
        -0.5f, -0.5f,
        0.5f,  -0.5f,
        -0.5f,  0.5f,
        0.5f,   0.5f,
    };
    GLubyte squareColors[] = {
        255, 255,   0, 255, 
        0,   255, 255, 255,
        0,     0,   0,   0,
        255,   0, 255, 255,
    };
	
	GLfloat shadowColors[] = {
		0.0, 0.0, 0.0, 50.0
	};
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glBindFramebufferEXT(GL_FRAMEBUFFER, viewFramebuffer);
//	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
    //glViewport(0, 0, 320, 480);
	//glViewport(-1000,-1000,2000,2000);
	
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	//glOrtho(0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
 //   glRotatef(0.0f, 0.0f, 0.0f, 1.0f); - reason?
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Background color
    glClear(GL_COLOR_BUFFER_BIT);
	
	// Render all (visible and unclipped) regions on a given page.
	
	for(urAPI_Region_t* t=firstRegion[currentPage]; t != NULL; t=t->next)
	{
		if(t->isClipping)
		{
			glScissor(t->clipleft,t->clipbottom,t->clipwidth,t->clipheight);
			glEnable(GL_SCISSOR_TEST);
		}
		else
		{
			glDisable(GL_SCISSOR_TEST);
		}
		
		if(t->isVisible)
		{
			squareVertices[0] = t->left;
			squareVertices[1] = t->bottom;
			squareVertices[2] = t->left;
			squareVertices[3] = t->bottom+t->height;
			squareVertices[4] = t->left+t->width;
			squareVertices[5] = t->bottom;
			squareVertices[6] = t->left+t->width;
			squareVertices[7] = t->bottom+t->height;
			
			glVertexPointer(2, GL_FLOAT, 0, squareVertices);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);
			float alpha = t->alpha;
			if(t->texture!=NULL)
			{
				if(t->texture->texturepath == TEXTURE_SOLID)
				{
					squareColors[0] = t->texture->texturesolidcolor[0];
					squareColors[1] = t->texture->texturesolidcolor[1];
					squareColors[2] = t->texture->texturesolidcolor[2];
					squareColors[3] = t->texture->texturesolidcolor[3]*alpha;
					
					squareColors[4] = t->texture->texturesolidcolor[0];
					squareColors[5] = t->texture->texturesolidcolor[1];
					squareColors[6] = t->texture->texturesolidcolor[2];
					squareColors[7] = t->texture->texturesolidcolor[3]*alpha;
					
					squareColors[8] = t->texture->texturesolidcolor[0];
					squareColors[9] = t->texture->texturesolidcolor[1];
					squareColors[10] = t->texture->texturesolidcolor[2];
					squareColors[11] = t->texture->texturesolidcolor[3]*alpha;
					
					squareColors[12] = t->texture->texturesolidcolor[0];
					squareColors[13] = t->texture->texturesolidcolor[1];
					squareColors[14] = t->texture->texturesolidcolor[2];
					squareColors[15] = t->texture->texturesolidcolor[3]*alpha;
				}
				else
				{
					squareColors[0] = t->texture->gradientBL[0];
					squareColors[1] = t->texture->gradientBL[1];
					squareColors[2] = t->texture->gradientBL[2];
					squareColors[3] = t->texture->gradientBL[3]*alpha;
					
					squareColors[4] = t->texture->gradientBR[0];
					squareColors[5] = t->texture->gradientBR[1];
					squareColors[6] = t->texture->gradientBR[2];
					squareColors[7] = t->texture->gradientBR[3]*alpha;
					
					squareColors[8] = t->texture->gradientUL[0];
					squareColors[9] = t->texture->gradientUL[1];
					squareColors[10] = t->texture->gradientUL[2];
					squareColors[11] = t->texture->gradientUL[3]*alpha;
					
					squareColors[12] = t->texture->gradientUR[0];
					squareColors[13] = t->texture->gradientUR[1];
					squareColors[14] = t->texture->gradientUR[2];
					squareColors[15] = t->texture->gradientUR[3]*alpha;
				}
				glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
				glEnableClientState(GL_COLOR_ARRAY);
				
				if(t->texture->backgroundTex == NULL && t->texture->texturepath != TEXTURE_SOLID)
				{
					instantiateTexture(t);
				}
				
				switch(t->texture->blendmode)
				{
					case BLEND_DISABLED:
						glDisable(GL_BLEND);
						break;
					case BLEND_BLEND:
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						break;
					case BLEND_ALPHAKEY:
						// NYI
						glAlphaFunc(GL_GEQUAL, 0.5f); // UR! This may be different
						glEnable(GL_ALPHA_TEST);
						break;
					case BLEND_ADD:
						glBlendFunc(GL_ONE, GL_ONE);
						break;
					case BLEND_MOD:
						glBlendFunc(GL_DST_COLOR, GL_ZERO);
						break;
					case BLEND_SUB: // Experimental blend category. Can be changed wildly NYI marking this for revision.
						glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
						break;
				}

				if(t->texture->backgroundTex)
				{
					glEnable(GL_TEXTURE_2D);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					GLfloat coordinates[]={0,1,1,1,0,0,1,0};
					//GLfloat  coordinates[] = {  t->texture->texcoords[0],          t->texture->texcoords[1],
				//		t->texture->texcoords[2],  t->texture->texcoords[3],
				//		t->texture->texcoords[4],              t->texture->texcoords[5],
				//	t->texture->texcoords[6],  t->texture->texcoords[7]  };
					
					
					
					glTexCoordPointer(2, GL_FLOAT, 0, coordinates);
					CGRect rect={{t->left,t->bottom},{t->width,t->height}};
					t->texture->backgroundTex->drawInRect(rect);
					
					glEnable(GL_BLEND);
					glDisable(GL_ALPHA_TEST);
				}
				else
				{
					glDisable(GL_TEXTURE_2D);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
					glEnableClientState(GL_COLOR_ARRAY);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
					glEnable(GL_BLEND);
					glDisable(GL_ALPHA_TEST);
				}
				// switch it back to GL_ONE for other types of images, rather than text because Texture2D uses CG to load, which premultiplies alpha
				glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
			}
			
			if(t->textlabel!=NULL)
			{
				// texturing will need these
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnable(GL_TEXTURE_2D);
				
				if(t->textlabel->updatestring)
				{
					if(t->textlabel->textlabelTex)
						delete t->textlabel->textlabelTex;
					TextAlignment align = TextAlignmentCenter;
					switch(t->textlabel->justifyh)
					{
						case JUSTIFYH_CENTER:
							align = TextAlignmentCenter;
							break;
						case JUSTIFYH_LEFT:
							align = TextAlignmentLeft;
							break;
						case JUSTIFYH_RIGHT:
							align = TextAlignmentRight;
							break;
					}
					strcpy(textlabelstr, t->textlabel->text); // Leak here. Fix.
					strcpy(fontname, t->textlabel->font);
					t->textlabel->updatestring = false;
					if(t->textlabel->drawshadow==false)
					{
						t->textlabel->textlabelTex = new urTexture(textlabelstr, fontname, t->textlabel->textheight); //TODO [[Texture2D alloc] initWithString:textlabelstr
															//				  dimensions:CGSizeMake(t->width, t->height) alignment:align
															//				  fontName:fontname fontSize:t->textlabel->textheight lineBreakMode:tolinebreakmode(t->textlabel->wrap)];
					}
					else
					{
						CGSize shadowoffset = {t->textlabel->shadowoffset[0],t->textlabel->shadowoffset[1]};
						shadowColors[0] = t->textlabel->shadowcolor[0];
						shadowColors[1] = t->textlabel->shadowcolor[1];
						shadowColors[2] = t->textlabel->shadowcolor[2];
						shadowColors[3] = t->textlabel->shadowcolor[3];
						t->textlabel->textlabelTex = new urTexture(textlabelstr, fontname, t->textlabel->textheight); ; //TODO [[Texture2D alloc] initWithString:textlabelstr
														//					  dimensions:CGSizeMake(t->width, t->height) alignment:align
														//						fontName:fontname fontSize:t->textlabel->textheight lineBreakMode:tolinebreakmode(t->textlabel->wrap)
														//					shadowOffset:CGSizeMake(t->textlabel->shadowoffset[0],t->textlabel->shadowoffset[1]) shadowBlur:t->textlabel->shadowblur shadowColor:t->textlabel->shadowcolor];
					}
				}
				
				// text will need blending
				glEnable(GL_BLEND);
				
				// text from Texture2D uses A8 tex format, so needs GL_SRC_ALPHA
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				for(int i=0;i<4;i++) // default regions are white
				{
					squareColors[4*i] = t->textlabel->textcolor[0];
					squareColors[4*i+1] = t->textlabel->textcolor[1];
					squareColors[4*i+2] = t->textlabel->textcolor[2];
					squareColors[4*i+3] = t->textlabel->textcolor[3]*t->alpha;
				}
				glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
				glEnableClientState(GL_COLOR_ARRAY);
				
				int bottom = t->bottom;
				int fontheight = t->textlabel->textlabelTex->getHeight();
				switch(t->textlabel->justifyv)
				{
					case JUSTIFYV_MIDDLE:
						bottom -= t->height/2-fontheight/2;
						break;
					case JUSTIFYV_TOP:
						bottom = bottom;
						break;
					case JUSTIFYV_BOTTOM:
						bottom -= t->height-fontheight; 
						break;
				}
				 
				CGPoint point={t->left, bottom};
				if(t->textlabel->textlabelTex)
					t->textlabel->textlabelTex->drawAtPoint(point, true);
				
				// switch it back to GL_ONE for other types of images, rather than text because Texture2D uses CG to load, which premultiplies alpha
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
		}
	}
	
	glDisable(GL_TEXTURE_2D);
#ifdef RENDERERRORSTRTEXTUREFONT
	// texturing will need these
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_TEXTURE_2D);
	
	if (errorStrTex == NULL)
	{
		newerror = false;
		errorStrTex = 0;//TODO [[Texture2D alloc] initWithString:errorstr
						//				 dimensions:CGSizeMake(320, 128) alignment:UITextAlignmentCenter
						//				   fontName:@"Helvetica" fontSize:14 lineBreakMode:UILineBreakModeWordWrap ];
	}
	else if(newerror)
	{
		if(errorStrTex) delete errorStrTex;
		newerror = false;
		errorStrTex = 0;// TODO [[Texture2D alloc] initWithString:errorstr
						//				 dimensions:CGSizeMake(320, 128) alignment:UITextAlignmentCenter
						//				   fontName:@"Helvetica" fontSize:14 lineBreakMode:UILineBreakModeWordWrap];
	}
	
	// text will need blending
	glEnable(GL_BLEND);
	
	// text from Texture2D uses A8 tex format, so needs GL_SRC_ALPHA
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for(int i=0;i<16;i++) // default regions are white
		squareColors[i] = 200;
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
	glEnableClientState(GL_COLOR_ARRAY);
	//CGPoint point={0.0, bounds.size.height * 0.5f};
	//errorStrTex->drawAtPoint(point, true);
	
	// switch it back to GL_ONE for other types of images, rather than text because Texture2D uses CG to load, which premultiplies alpha
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
#endif
	
	
	
//    glBindRenderbufferEXT(GL_RENDERBUFFER, viewRenderbuffer);

}


bool createFramebuffer() {
    
//    glGenFramebuffersEXT(1, &viewFramebuffer);
//    glGenRenderbuffersEXT(1, &viewRenderbuffer);
    
//    glBindFramebufferEXT(GL_FRAMEBUFFER, viewFramebuffer);
//    glBindRenderbufferEXT(GL_RENDERBUFFER, viewRenderbuffer);
//    [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
//    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, viewRenderbuffer);
    
//    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
//    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
    
    if (USE_DEPTH_BUFFER) {
//        glGenRenderbuffersEXT(1, &depthRenderbuffer);
//        glBindRenderbufferEXT(GL_RENDERBUFFER, depthRenderbuffer);
//        glRenderbufferStorageEXT(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
//        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    }
    
//    if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
 //       NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
///        return false;
//    }
    
    return true;
}


void destroyFramebuffer() {
    if(viewFramebuffer==0) return;
 //   glDeleteFramebuffersEXT(1, &viewFramebuffer);
    viewFramebuffer = 0;
//    glDeleteRenderbuffersEXT(1, &viewRenderbuffer);
    viewRenderbuffer = 0;

    if(depthRenderbuffer) {
  //      glDeleteRenderbuffersEXT(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}


void layoutSubviews() {
    destroyFramebuffer();
    createFramebuffer();
    drawView();
}


#define NR_FINGERS 2

float distanceBetweenPoints(CGPoint first, CGPoint second)
{
	float deltax = second.x-first.x;
	float deltay = second.y-first.y;
	return sqrt(deltax*deltax + deltay*deltay);
}

int NumHitMatches(urAPI_Region_t* hitregion[], int max, int idx, int repeat)
{
	int count = 0;
	for(int i=0; i<max; i++)
		if(hitregion[idx] == hitregion[i])
			count++;
	return 0;
}

// preliminary
urAPI_Region_t* dragregion;
float &dragx=cursorpositionx[0];
float &dragy=cursorpositiony[0];

void touchesBegan(int x, int y) {	// preliminary - no multi-touch yet
	dragregion=findRegionHit(x, y);
	if(dragregion!=NULL) {
		callScript(dragregion->OnTouchDown, dragregion);
		dragx=x;
		dragy=y;
	}
}

void touchesMoved(int x, int y) {
	int positionx=x;
	int positiony=y;
	int speedx=x-dragx;
	int speedy=y-dragy;

	urAPI_Region_t* scrollregion = findRegionXScrolled(x,480-y,speedx);
	if(scrollregion != NULL)
	{
		callScriptWith1Args(scrollregion->OnHorizontalScroll, scrollregion, speedx);
	}
	scrollregion = findRegionYScrolled(x,480-y,-speedy);
	if(scrollregion != NULL)
	{
		callScriptWith1Args(scrollregion->OnVerticalScroll, scrollregion, -speedy);
	}

	dragx=x;
	dragy=y;
}

void touchesEnded(int x, int y) {
	dragregion=findRegionHit(x, y);
	if(dragregion!=NULL) {
		callScript(dragregion->OnTouchUp, dragregion);
		dragx=x;
		dragy=y;
	}
}

void doubleClick(int x, int y) {
 	urAPI_Region_t* hitregion=findRegionHit(x, y);
	if(hitregion!=NULL)
		callScript(hitregion->OnDoubleTap, hitregion);
}

/*
// Handles the start of a touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (ActiveTouches == nil)
        ActiveTouches = [[NSMutableArray alloc] init];
    
    for (UITouch *touch in touches) {
        if (![ActiveTouches containsObject:touch])
            [ActiveTouches addObject:touch];
    }
	NSUInteger numTouches = [touches count];

#ifdef DEBUG_TOUCH
	errorstr = @"Begin";
	[errorstr stringByAppendingFormat:@": %d", numTouches];
	newerror = true;
#endif
	
	// Event for all fingers (global). We do this first so people can choose to create/remove regions that can also receive events for the locations (yay)
	for(int t =0; t<numTouches; t++)
	{
		UITouch *touch = [[touches allObjects] objectAtIndex:t];
		CGPoint position = [touch locationInView:self];
		callAllTouchSources(position.x/160.0-1.0, 1.0-position.y/240.0,t);
	}
	
	urAPI_Region_t* hitregion[MAX_FINGERS];
	for(int t=0; t< numTouches; t++)
	{
		hitregion[t] = NULL;
		UITouch *touch = [[touches allObjects] objectAtIndex:t];
		NSUInteger numTaps = [touch tapCount];
		UITouchPhase phase = [touch phase];
		CGPoint position = [touch locationInView:self];
		cursorpositionx[t] = position.x;
		cursorpositiony[t] = position.y;
		
		if(phase == UITouchPhaseBegan) // Hope this works ...
		{
			hitregion[t] = findRegionHit(position.x, 480-position.y);
			if(hitregion[t]!=nil)
			{
				// A double tap.
				if (numTaps == 2 && hitregion[t]->OnDoubleTap) 
				{
					callScript(hitregion[t]->OnDoubleTap, hitregion[t]);
				}
				else if (numTaps == 3 && false)
				{
					// Tripple Tap NYI
				}
				else
					callScript(hitregion[t]->OnTouchDown, hitregion[t]);
			}
		}
		else
		{
			int a = 0;
		}
	}
	
	// Find two-finger drags
	for(int t1 = 0; t1<numTouches-1; t1++)
	{
		for(int t2 = t1+1; t2<numTouches; t2++)
		{
			if(hitregion[t1] != NULL && hitregion[t1] == hitregion[t2] && hitregion[t1]->isMovable && hitregion[t1]->isResizable) // Pair of fingers on draggable region?
			{
				hitregion[t1]->isDragged = true; // YAYA
				hitregion[t1]->isResized = true;
				int dragidx = FindAvailableDragTouch();
				dragtouches[dragidx].dragregion = hitregion[t1];
				dragtouches[dragidx].touch1 = [[touches allObjects] objectAtIndex:t1];
				dragtouches[dragidx].touch2 = [[touches allObjects] objectAtIndex:t2];
				dragtouches[dragidx].dragwidth = hitregion[t1]->width-fabs(cursorpositionx[t2]-cursorpositionx[t1]);
				dragtouches[dragidx].dragheight = hitregion[t1]->height-fabs(cursorpositiony[t2]-cursorpositiony[t1]);
				dragtouches[dragidx].active = true;
			}
		}
	}
	
	// Find single finger drags (not already classified as two-finger ones.
	for(int t = 0; t<numTouches; t++)
	{
		if(hitregion[t]!=nil && hitregion[t]->isMovable)
		{
			hitregion[t]->isDragged = true; // YAYA
			int dragidx = FindDragRegion(hitregion[t]);
			if(dragidx == -1)
			{
				dragidx = FindAvailableDragTouch();
				dragtouches[dragidx].dragregion = hitregion[t];
				dragtouches[dragidx].touch1 = [[touches allObjects] objectAtIndex:t];
				dragtouches[dragidx].touch2 = NULL;
				dragtouches[dragidx].active = true;
			}
			else
			{
				AddDragRegion(dragidx,[[touches allObjects] objectAtIndex:t]);
				if(dragtouches[dragidx].touch2 != NULL)
				{
					CGPoint position1 = [dragtouches[dragidx].touch1 locationInView:self];
					CGPoint position2 = [dragtouches[dragidx].touch2 locationInView:self];
					dragtouches[dragidx].dragwidth = dragtouches[dragidx].dragregion->width-fabs(position2.x-position1.x);
					dragtouches[dragidx].dragheight = dragtouches[dragidx].dragregion->height-fabs(position2.y-position1.y);
				}
			}4
		}
	}		
}
*/

void ClampRegion(urAPI_Region_t*region)
{
	if(region->left < 0) region->left = 0;
	if(region->bottom < 0) region->bottom = 0;
	if(region->width > SCREEN_WIDTH) region->width = SCREEN_WIDTH;
	if(region->height > SCREEN_HEIGHT) region->height = SCREEN_HEIGHT;
	if(region->left+region->width > SCREEN_WIDTH) region->left = SCREEN_WIDTH-region->width;
	if(region->bottom+region->height > SCREEN_HEIGHT) region->bottom = SCREEN_HEIGHT-region->height;
}

/*
// Handles the continuation of a touch.
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{  
	
	NSUInteger numTouches = [touches count];
#ifdef DEBUG_TOUCH
	errorstr = @"Move";
	newerror = true;
	
	[errorstr stringByAppendingFormat:@": %d", numTouches];
#endif

	
	// Event for all fingers (global)
	for(int t =0; t<numTouches; t++)
	{
		UITouch *touch = [[touches allObjects] objectAtIndex:t];
		CGPoint position = [touch locationInView:self];
		callAllTouchSources(position.x/160.0-1.0, 1.0-position.y/240.0,t);
	}

//	urAPI_Region_t* hitregion[MAX_FINGERS];
	int arg = 0;
	for(int t=0; t< numTouches; t++)
	{
		UITouch *touch = [[touches allObjects] objectAtIndex:t];
		UITouchPhase phase = [touch phase];
		CGPoint position = [touch locationInView:self];
		if(phase == UITouchPhaseMoved) 
		{
			CGPoint oldposition = [[[touches allObjects] objectAtIndex:t] previousLocationInView:self];
			int t2 = t;
			if(oldposition.x != cursorpositionx[t] || oldposition.y != cursorpositiony[t])
			{
				for(t2=0; t2<MAX_FINGERS && (oldposition.x != cursorpositionx[t2] || oldposition.y != cursorpositiony[t2]); t2++);
				if(t2==MAX_FINGERS)
				{
					int a=0;
					t2=t; vb
				}
			}	
			cursorscrollspeedx[t2] = position.x - oldposition.x;
			cursorscrollspeedy[t2] = position.y - oldposition.y;
			cursorpositionx[t2] = position.x;
			cursorpositiony[t2] = position.y;
			argmoved[arg] = t;
			argcoordx[arg] = position.x;
			argcoordy[arg] = 480-position.y;
			arg2coordx[arg] = oldposition.x;
			arg2coordy[arg] = 480-oldposition.y;
			arg++;
		}
		else
		{
			int a=0;
		}
	}
	
	for(int i=0; i < arg; i++)
	{
		int t = argmoved[i];
		int dragidx = FindSingleDragTouch([[touches allObjects] objectAtIndex:t]);
		if(dragidx != -1)
		{
			if(dragtouches[dragidx].touch2 != NULL) // Double Touch here.
			{
				float dx = cursorscrollspeedx[t];
				float dy = -(cursorscrollspeedy[t]);
				if( dx !=0 || dy != 0)
				{
					urAPI_Region_t* dragregion = dragtouches[dragidx].dragregion;
					dragregion->left += dx;
					dragregion->bottom += dy;
					CGPoint position1 = [dragtouches[dragidx].touch1 locationInView:self];
					CGPoint position2 = [dragtouches[dragidx].touch2 locationInView:self];
					float cursorpositionx2 = position2.x;
					float cursorpositiony2 = position2.y;
					if(dragregion->isResizable)
					{
						float deltanewwidth = fabs(cursorpositionx2-position1.x);
						float deltanewheight = fabs(cursorpositiony2-position1.y);
						dragregion->width = dragtouches[dragidx].dragwidth + deltanewwidth;
						dragregion->height = dragtouches[dragidx].dragheight + deltanewheight;
					}
					dragregion->right = dragregion->left + dragregion->width;
					dragregion->top = dragregion->bottom + dragregion->height;
					if(dragregion->isClamped) ClampRegion(dragregion);
					callScript(dragregion->OnSizeChanged, dragregion);
				}
			}
			else
			{
				float dx = cursorscrollspeedx[t];
				float dy = -(cursorscrollspeedy[t]);
				if( dx !=0 || dy != 0)
				{
					urAPI_Region_t* dragregion = dragtouches[dragidx].dragregion;
					dragregion->left += dx;
					dragregion->bottom += dy;
					dragregion->right += dx;
					dragregion->top += dy;
				}
			}
		}
		else 
		{
			urAPI_Region_t* scrollregion = findRegionXScrolled(cursorpositionx[t],480-cursorpositiony[t],cursorscrollspeedx[t]);
			if(scrollregion != nil)
			{
				callScriptWith1Args(scrollregion->OnHorizontalScroll, scrollregion, cursorscrollspeedx[t]);
			}
			scrollregion = findRegionYScrolled(cursorpositionx[t],480-cursorpositiony[t],-cursorscrollspeedy[t]);
			if(scrollregion != nil)
			{
				callScriptWith1Args(scrollregion->OnVerticalScroll, scrollregion, -cursorscrollspeedy[t]);
			}
		}
	}
	
	callAllOnEnterLeaveRegions(arg, argcoordx, argcoordy,arg2coordx,arg2coordy);
}
*/

/*
// Handles the end of a touch event.
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
#ifdef DEBUG_TOUCH
	errorstr = @"End";
	newerror = true;
#endif
    for (UITouch *touch in touches) {
        [ActiveTouches removeObject:touch];
    }
	NSUInteger numTouches = [touches count];

	// Event for all fingers (global). We do this first so people can choose to create/remove regions that can also receive events for the locations (yay)
	for(int t =0; t<numTouches; t++)
	{
		UITouch *touch = [[touches allObjects] objectAtIndex:t];
		CGPoint position = [touch locationInView:self];
		callAllTouchSources(position.x/160.0-1.0, 1.0-position.y/240.0,t);
	}
	
	int arg = 0;
	for(int t=0; t< numTouches; t++)
	{
		UITouch *touch = [[touches allObjects] objectAtIndex:t];
		UITouchPhase phase = [touch phase];
		CGPoint position = [touch locationInView:self];
		cursorpositionx[t] = position.x;
		cursorpositiony[t] = position.y;

		if(phase == UITouchPhaseEnded)		{
			
			int dragidx = FindSingleDragTouch(touch);
			if(dragidx != -1)
			{
				if(dragtouches[dragidx].touch1 == touch)
					dragtouches[dragidx].touch1 = NULL;
				if(dragtouches[dragidx].touch2 == touch)
					dragtouches[dragidx].touch2 = NULL;
				if(	dragtouches[dragidx].touch1 == NULL && dragtouches[dragidx].touch2 == NULL)
				{
					dragtouches[dragidx].active = false;
					dragtouches[dragidx].dragregion->isDragged = false;
					callScript(dragtouches[dragidx].dragregion->OnDragStop, dragtouches[dragidx].dragregion);
				}
				else if(dragtouches[dragidx].touch2 != NULL)
				{
					dragtouches[dragidx].touch1 = dragtouches[dragidx].touch2;
					dragtouches[dragidx].touch2 = NULL;
				}
				dragtouches[dragidx].dragregion->isResized = false;
			}
			
			CGPoint oldposition = [touch previousLocationInView:self];
			urAPI_Region_t* hitregion = findRegionHit(position.x, 480-position.y);
			NSUInteger numTaps = [touch tapCount];
			if(hitregion)
			{
				callScript(hitregion->OnTouchUp, hitregion);
				callAllOnLeaveRegions(position.x, 480-position.y);
			}
			else
			{
				argcoordx[arg] = position.x;
				argcoordy[arg] = 480-position.y;
				arg2coordx[arg] = oldposition.x;
				arg2coordy[arg] = 480-oldposition.y;
				arg++;
				
			}
		}
		else
		{
			int a = 0;
		}
	}

	callAllOnEnterLeaveRegions(arg, argcoordx, argcoordy,arg2coordx,arg2coordy);
	
}
*/

/*
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    // Enumerates through all touch object
    for (UITouch *touch in touches){
		// Sends to the dispatch method, which will make sure the appropriate subview is acted upon
	}
}
*/

#ifdef SANDWICH_SUPPORT
// sandwich update Delegate functions
- (void) rearTouchUpdate: (SandwichEventManager * ) sender;
{
	
	CGPoint touchCoords = [sender touchCoordsForTouchAtIndex: 0];
	//	tx = touchCoords.x; gessl disabling rear
	//	ty = touchCoords.y;
}

- (void) pressureUpdate: (SandwichEventManager * ) sender;
{
	pressure[0] = sender.pressureValues[0];
	pressure[1] = sender.pressureValues[1];
	pressure[2] = sender.pressureValues[2];
	pressure[3] = sender.pressureValues[3];
	
	float avg = pressure[3];	
	
	// This feeds the lua API events
	callAllOnPressure(avg);
	
	// We call the UrSound pipeline second so that the lua engine can actually change it based on acceleration data before anything happens.
//	callAllPressureSources(avg);
	
}
#endif


