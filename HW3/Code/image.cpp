#include "image.h"
#include "pixel.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

/**
 * Image
 **/
Image::Image (int width_, int height_){

    assert(width_ > 0);
    assert(height_ > 0);

    width           = width_;
    height          = height_;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;
    
    data.raw = new uint8_t[num_pixels*4];
	int b = 0; //which byte to write to
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			data.raw[b++] = 0;
			data.raw[b++] = 0;
			data.raw[b++] = 0;
			data.raw[b++] = 0;
		}
	}

    assert(data.raw != NULL);
}

Image::Image (const Image& src){
	
	width           = src.width;
    height          = src.height;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;
    
    data.raw = new uint8_t[num_pixels*4];
    
    //memcpy(data.raw, src.data.raw, num_pixels);
    *data.raw = *src.data.raw;
}

Image::Image (char* fname){

	int numComponents; //(e.g., Y, YA, RGB, or RGBA)
	data.raw = stbi_load(fname, &width, &height, &numComponents, 4);
	
	if (data.raw == NULL){
		printf("Error loading image: %s", fname);
		exit(-1);
	}
	

	num_pixels = width * height;
	sampling_method = IMAGE_SAMPLING_POINT;
	
}

Image::~Image (){
    delete data.raw;
    data.raw = NULL;
}

void Image::Write(char* fname){
	
	int lastc = strlen(fname);

	switch (fname[lastc-1]){
	   case 'g': //jpeg (or jpg) or png
	     if (fname[lastc-2] == 'p' || fname[lastc-2] == 'e') //jpeg or jpg
	        stbi_write_jpg(fname, width, height, 4, data.raw, 95);  //95% jpeg quality
	     else //png
	        stbi_write_png(fname, width, height, 4, data.raw, width*4);
	     break;
	   case 'a': //tga (targa)
	     stbi_write_tga(fname, width, height, 4, data.raw);
	     break;
	   case 'p': //bmp
	   default:
	     stbi_write_bmp(fname, width, height, 4, data.raw);
	}
}

void Image::Fill (int r, int g, int b)
{
	int x,y;
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			Pixel p = GetPixel(x, y);
			p.r = r;
			p.g = g;
			p.b = b;
			p.a = 255;
			GetPixel(x,y) = p;
		}
	}
}

Image* Image::Scale(float sx, float sy)
{
	int width_n = sx*Width();
	int height_n = sy*Height();

	float u,v;

	Image* ret = new Image(width_n, height_n);

	for (int x = 0; x < width_n; x++)
	{
		for (int y = 0; y < height_n; y++)
		{
			u = x/sx;
			v = y/sy;
			ret->GetPixel(x,y) = Sample(u,v);
		}
	}
	return ret;
}

Pixel Image::Sample (float u, float v){
    // take two floats and make them ints

	Pixel p; // pixel
	int x, y;

	// for bilinear interpolation
	Pixel p1, p2, p3, p4;
	int x_l, x_h, y_l, y_h; // x and y low and high (floor and ceil)
	int w1, w2, w3, w4;
	float fx, fy, fx1, fy1;

    //if (sampling_method == 0) // nearest-neighbor (I think)
    //{
    	//printf("point\n");
    	// cast doubles as ints (will round down)
    	//x = (int)u;
    	//y = (int)v;
    	//p = GetPixel(x,y);
    //}
    //else if (sampling_method == 1) // 2D bilinear interpolation
    //{
    	int r, g, b;
 		x_l = (int)u%Width();
 		y_l = (int)v%Height();
    	x_h = ceil(u);
    	x_h = x_h%Width();
    	y_h = ceil(v);
    	y_h = y_h%Height();
 
		p1 = GetPixel(x_l, y_l);
		p2 = GetPixel(x_h, y_l);
		p3 = GetPixel(x_l, y_h);
		p4 = GetPixel(x_h, y_h);
 
		// Calculate the weights for each pixel
		fx = u - x_l;
		fy = v - y_l;
		fx1 = 1.0f - fx;
		fy1 = 1.0f - fy;

		w1 = fx1 * fy1 * 256.0f;
		w2 = fx  * fy1 * 256.0f;
		w3 = fx1 * fy  * 256.0f;
		w4 = fx  * fy  * 256.0f;
 
		// Calculate the weighted sum of pixels (for each color channel)
		r = p1.r * w1 + p2.r * w2 + p3.r * w3 + p4.r * w4;
		g = p1.g * w1 + p2.g * w2 + p3.g * w3 + p4.g * w4;
		b = p1.b * w1 + p2.b * w2 + p3.b * w3 + p4.b * w4;

		return Pixel(r >> 8, g >> 8, b >> 8, 255);
    //}
}