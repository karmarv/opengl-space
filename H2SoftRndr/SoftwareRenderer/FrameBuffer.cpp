#include <atlimage.h>
#include "FrameBuffer.h"

/* this function copies the color buffer to the given CImage */
void FrameBuffer :: copyColorBuffer(CImage *image) {
	int i, j;
	int pitch = image->GetPitch();
	u08 *ptr_dst = (u08 *)image->GetBits();
	u08 *ptr_src = getColorPtr(0, height - 1);
	/* note that CImage is upside down, so we start reading the framebuffer
	   from the top of the screen */

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			*(ptr_dst++) = *(ptr_src + 2);
			*(ptr_dst++) = *(ptr_src + 1);
			*(ptr_dst++) = *(ptr_src);

			ptr_src += 3;
		}

		ptr_src += -(width * 3) * 2;
		ptr_dst += pitch - (width * 3); 
	}
	return;
}

/* this function dumps the color framebuffer to the specified file.  I will
   use the CImage class to do this */
void FrameBuffer :: dumpColorBufferToFile(char *name) {
	CImage image;

	image.Create(width, height, 24);


	/* now copy color buffer to the CImage bit array */
	copyColorBuffer(&image);

	/* save the image to the file */
	image.Save(name);

	/* clean up the CImage */
	image.Destroy();

	return;
}


#include <windows.h>
#include "gl.h"

/* This function dumps the data array to the screen using glDrawPixels() */
void FrameBuffer :: dumpToScreen(void) {
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);
	return;
}

/* Clear the framebuffer */
void FrameBuffer::clear(void) {
	memset(color_buffer, 0, (sizeof(u08) * width * height * 3));
}

/* Set the pixel color in the framebuffer */
void FrameBuffer::setColorBuffer(int x, int y, int color[])
{
	if (y >= window_height || x >= window_width ||
		y < 0 || x < 0) {
		// Cannot set this buffer

	}else{
		int offset = ((y) * width + x) * 3;
		color_buffer[offset  ] = 255;
		color_buffer[offset+1] = 0;
		color_buffer[offset+2] = 0;
	}
}

/* Test buffer */
void FrameBuffer::makeCheckImage(void) {
	int h = height, w = width;
	int sz = w * h * 3;
	//printf("Size: %d\n", sizeof(u08) * width * height * 3);
	int brush_size = 1;
	int i = 0, j = 0, k = 0, c = 0;
	for (i = 0; i < w; i++) {
		for (j = 0; j < h; j++) {
			for (k = 0; k < 3; k++) {
				if (k == 2)
					c =  ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
				else
					c = 0;
				*(color_buffer++) = (GLubyte)c;
			}
		}
	}
	color_buffer -= sz;
}
