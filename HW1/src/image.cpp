#include "image.h"
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

void Image::AddNoise (double factor)
{
	int x,y;
	Pixel p;
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			double r = (double)rand() / (RAND_MAX); // random double 0 to 1
			if (r <= factor)
			{
				p = PixelRandom();
				p.a = 255;
				GetPixel(x,y) = p;
			}

		}
	}
}

void Image::Brighten (double factor)
{
	int x,y;
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			Pixel p = GetPixel(x, y);
			Pixel scaled_p = p*factor;
			scaled_p.a = 255;
			GetPixel(x,y) = scaled_p;
		}
	}
}

float Image::AvgLuminance()
{
	float lum = 0;
	int x, y;
	Pixel p;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			p = GetPixel(x,y);
			lum += p.Luminance();
		}
	}
	// lum is the sum of all of the values of luminance right now
	// divide by Width() * Height() to get the average
	lum = lum / (Width() * Height());
	return lum;
}

void Image::ChangeContrast (double factor)
{
	// average gray image for linear interpolation (lerp)
	float avglum = AvgLuminance();
	Pixel avgGray = Pixel(avglum, avglum, avglum);

	int x, y;
	Pixel p;
	Pixel scaled_p;
	float lum;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			p = GetPixel(x,y);
			lum = p.Luminance();
			scaled_p = PixelLerp(p, avgGray, (1-factor));
				// linear interpolation between the current pixel and average pixel by given factor
				// do (1-factor) so that factor of 0 will result in a gray image
			scaled_p.a = 255;
				// set alpha channel to 255, avoid magenta pixels
			GetPixel(x,y) = scaled_p;
		}
	}
}


void Image::ChangeSaturation(double factor)
{
	// Three ways to get a gray scale image: lightness, average, and luminosity
	// Implementing luminosity – accounts for differences in how the brightness of colors is pereived
	int x, y;
	Pixel p, gray, scaled_p;
	float lum;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			p = GetPixel(x,y);
			// grayscale pixel for linear interpolation
			lum = p.Luminance();
			gray = Pixel(lum, lum, lum);
			gray.a = 255;
				// set alpha channel to 255, avoid magenta pixels
			scaled_p = PixelLerp(p, gray, (1-factor));
				// linear interpolation between the current pixel and grayscale pixel by given factor
				// do (1-factor) so that factor of 0 will result in a grayscale image
			GetPixel(x,y) = scaled_p;
		}
	}
}


Image* Image::Crop(int x, int y, int w, int h)
{
	Image* result = new Image(w,h);
	int x_i = 0;
	int y_i = 0;
	
	for (x_i = 0; x_i < w; x_i++)
	{
		for (y_i = 0; y_i < h; y_i++)
		{
			result->GetPixel(x_i, y_i) = GetPixel(x+x_i, y+y_i);
		}
	}

	// TODO: Error checking

	return result;
}


// ExtractChannel
// TODO: add a part to the -help text about these assumptions
// **  NOTE THE FOLLOWING ASSUMPTIONS  **
// Assumptions: channels indexed starting at 0 for R, G, B
// 				this EXCLUDES the alpha channel (otherwise results wouldn't be visible)
void Image::ExtractChannel(int channel)
{
	int x,y;
	Pixel q;
	q.SetClamp(0,0,0);
	if (channel == 0) 		// R
		q.SetClamp(1,0,0);
	else if (channel == 1)  // B
		q.SetClamp(0,1,0);
	else if (channel == 2)	// G
		q.SetClamp(0,0,1);
	Pixel p, extracted_p;
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			p = GetPixel(x, y);
			extracted_p = p * q;
			GetPixel(x,y) = extracted_p;
		}
	}
}


void Image::Quantize (int nbits)
{
	int x, y;
	Pixel p;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			p = GetPixel(x,y);
			GetPixel(x,y) = PixelQuant(p, nbits);
		}
	}
}

void Image::RandomDither (int nbits)
{
	int x, y;
	Pixel p, pq;
	int r, g, b;
	//double r;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			p = GetPixel(x,y);
			GetPixel(x,y) = PixelRandQuant(p, nbits);
		}
	}
}


static int Bayer4[4][4] =
{
    {15,  7, 13,  5},
    { 3, 11,  1,  9},
    {12,  4, 14,  6},
    { 0,  8,  2, 10}
};


void Image::OrderedDither(int nbits)
{
	/* WORK HERE */
}

/* Error-diffusion parameters */
const double
    ALPHA = 7.0 / 16.0,
    BETA  = 3.0 / 16.0,
    GAMMA = 5.0 / 16.0,
    DELTA = 1.0 / 16.0;

void Image::FloydSteinbergDither(int nbits)
{
	/* WORK HERE */
}

void Image::Blur(int n)
{
	/* WORK HERE */
}

void Image::Sharpen(int n)
{
	/* WORK HERE */
}

void Image::EdgeDetect()
{
	/* WORK HERE */
}

Image* Image::Scale(double sx, double sy)
{
	/* WORK HERE */
	return NULL;
}

Image* Image::Rotate(double angle)
{
	/* WORK HERE */
	return NULL;
}

void Image::Fun()
{
	/* WORK HERE */
}

/**
 * Image Sample
 **/
void Image::SetSamplingMethod(int method)
{
    assert((method >= 0) && (method < IMAGE_N_SAMPLING_METHODS));
    sampling_method = method;
}


Pixel Image::Sample (double u, double v){
    /* WORK HERE */
	return Pixel();
}