#include "urTexture.h"

urTexture::urTexture(const void *data, GLenum format, unsigned int width, unsigned int height)
{
	GLint saveName;
	
	glGenTextures(1, &name);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveName);
	glBindTexture(GL_TEXTURE_2D, name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, saveName);

	this->width=width;
	this->height=height;
	this->format=format;
}

urTexture::urTexture(urImage *image) 
{
	GLint saveName;
	
	const void* data=image->getBuffer();
	
	switch(image->getColorType()) {
		case PNG_COLOR_TYPE_RGB:
			format=GL_RGB;
			break;
		case PNG_COLOR_TYPE_RGBA:
			format=GL_RGBA;
			break;
	}

	width=image->getWidth();
	height=image->getHeight();

	glGenTextures(1, &name);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveName);
	glBindTexture(GL_TEXTURE_2D, name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, saveName);

	this->width=width;
	this->height=height;
	this->format=format;
}

urTexture::urTexture(const char *str, const char *fontname, unsigned int size) {
	name=0;
	font.loadFont("arial.ttf",size,true,true,true);
	this->str=str;
}

urTexture::~urTexture(void)
{
	if(name)
		glDeleteTextures(1, &name);
}


void urTexture::drawInRect(CGRect rect) {
	if(name) {	// it's an image
		GLfloat vertices[] = {  rect.origin.x, rect.origin.y, 0.0,
			rect.origin.x + rect.size.width, rect.origin.y, 0.0,
			rect.origin.x, rect.origin.y + rect.size.height, 0.0,
		rect.origin.x + rect.size.width, rect.origin.y + rect.size.height, 0.0 };
	
		glBindTexture(GL_TEXTURE_2D, name);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
	//	glTexCoordPointer(2, GL_FLOAT, 0, coordinates);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	if(font.bLoadedOk) {	// it's a text
		font.drawString(str,rect.origin.x,rect.origin.y);
	}
}

void urTexture::drawAtPoint(CGPoint point, bool tile) {
	if(name) {	// it's an image
		GLfloat         coordinates[] = {0.0f,1.0f,1.0f,1.0f,0.0f,0.0f,1.0f,0.0f};
		GLfloat         vertices[] = {  point.x,                        point.y,        0.0,
			width + point.x,        point.y,        0.0,
			point.x,                        height  + point.y,      0.0,
		width + point.x,        height  + point.y,      0.0 };

		glBindTexture(GL_TEXTURE_2D, name);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glTexCoordPointer(2, GL_FLOAT, 0, coordinates);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	if(font.bLoadedOk) {	// it's a text
		font.drawString(str,point.x,point.y);
	}
}

