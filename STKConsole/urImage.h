#ifndef __URIMAGE_H__
#define __URIMAGE_H__

#include "png.h"

class urImage
{
protected:
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth, color_type, interlace_type;
	png_bytep buffer;
public:
	png_uint_32 getWidth() { return width; }
	png_uint_32 getHeight() { return height; }
	int getBitDepth() { return bit_depth; }
	int getColorType() { return color_type; }
	int getInterlaceType() { return interlace_type; }
	const void* getBuffer() { return buffer; }
	urImage(char* file_name);
	~urImage(void);
};

#endif // __URIMAGE_H__