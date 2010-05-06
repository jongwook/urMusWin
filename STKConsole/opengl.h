
//
//  EAGLView.h
//  urMus
//
//  Created by Georg Essl on 6/20/09.
//  Copyright Georg Essl 2009. All rights reserved. See LICENSE.txt for license details.
//

#ifndef __OPENGL_H__
#define __OPENGL_H__


#include <windows.h>
#include <gl/gl.h>

//#define GL_GLEXT_PROTOTYPES
///#include <gl/glext.h>
#include "glext.h"
/*
extern PFNGLISRENDERBUFFEREXTPROC             glIsRenderbufferEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC           glBindRenderbufferEXT;
extern PFNGLDELETERENDERBUFFERSEXTPROC        glDeleteRenderbuffersEXT;
extern PFNGLGENRENDERBUFFERSEXTPROC           glGenRenderbuffersEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC        glRenderbufferStorageEXT;
extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT;
extern PFNGLISFRAMEBUFFEREXTPROC              glIsFramebufferEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC            glBindFramebufferEXT;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC         glDeleteFramebuffersEXT;
extern PFNGLGENFRAMEBUFFERSEXTPROC            glGenFramebuffersEXT;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC     glCheckFramebufferStatusEXT;
extern PFNGLFRAMEBUFFERTEXTURE1DEXTPROC       glFramebufferTexture1DEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC       glFramebufferTexture2DEXT;
extern PFNGLFRAMEBUFFERTEXTURE3DEXTPROC       glFramebufferTexture3DEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC    glFramebufferRenderbufferEXT;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
extern PFNGLGENERATEMIPMAPEXTPROC             glGenerateMipmapEXT;
*/
void initExtensions();
void layoutSubviews();
void touchesBegan(int x, int y);
void touchesMoved(int x, int y);
void touchesEnded(int x, int y);
void doubleClick(int x, int y);

#endif // __OPENGL_H__