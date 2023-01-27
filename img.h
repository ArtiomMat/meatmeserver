#pragma once

// Library that supports decoding and encoding of:
// PNGs and JPEGs.

enum {
	IMG_FORMAT_RGB888,
	IMG_FORMAT_RGBA8888,
	IMG_FORMAT_YUV888,
	IMG_FORMAT_YUVA888,
	
	IMG_FORMAT_RGB565,
	
	IMG_FORMAT_PALETTE,
};

typedef struct {
	int width, height;
	char format;
	char** palette;
	char* pixels;
} img_t;

// 0 if fails
char img_open(img_t* img, const char *fp);
// 0 if fails
char img_save(img_t* img, const char *fp);
