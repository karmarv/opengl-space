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

/* this function dumps the data array to the screen using glDrawPixels() */
void FrameBuffer :: dumpToScreen(void) {
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);

	return;
}


void FrameBuffer::makeCheckImage(void) {
	int sz = width * height * 3;
	//printf("Size: %d\n", sizeof(u08) * width * height * 3);
	int brush_size = 1;
	int i = 0, j = 0, k = 0, c = 0;
	for (i = 0; i < (width); i++) {
		for (j = 0; j < (height); j++) {
			for (k = 0; k < 3; k++) {
				// c= ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
				if (k == 0)
					c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
				else
					c = 0;
				*(color_buffer++) = (GLubyte)c;
			}
		}
	}
	color_buffer -= sz;
}
