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
	int x, y;
	Pixel p, tp;
	int red, green, blue;
	int r_qe, g_qe, b_qe; // quantization error for r,g,b

	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			p = GetPixel(x,y);
			tp = p;

			int var = pow(2,9-nbits)-1;
			/* round rgb values to nearest threshhold */
			if (p.r%(var+1) > (var+1)/2)
			{
				red = (p.r-(p.r%var)+var);
			}
			else
			{
				red = (p.r-(p.r%var));
			}
			if (p.g%(var+1) > (var+1)/2)
			{
				green = (p.g-(p.g%var)+var);
			}
			else
			{
				green = (p.g-(p.g%var));
			}
			if (p.b%(var+1) > (var+1)/2)
			{
				blue = (p.b-(p.b%var)+var);
			}
			else
			{
				blue = (p.b-(p.b%var));
			}

			tp.SetClamp(red,green,blue);
			GetPixel(x,y) = tp;
			
			r_qe = p.r - tp.r;
			g_qe = p.g - tp.g;
			b_qe = p.b - tp.b;
			// Diffuse the error and check bounds
			
			if (x < Width() - 1)
			{
				this->GetPixel(x+1, y).SetClamp(
					this->GetPixel(x+1, y).r + r_qe * ALPHA,
					this->GetPixel(x+1, y).g + g_qe * ALPHA,
					this->GetPixel(x+1, y).b + b_qe * ALPHA
					);
			}
			if ((x > 0) && (y < Height() - 1))
			{
				this->GetPixel(x-1, y+1).SetClamp(
					this->GetPixel(x-1, y+1).r + r_qe * BETA,
					this->GetPixel(x-1, y+1).g + g_qe * BETA,
					this->GetPixel(x-1, y+1).b + b_qe * BETA
					);
			}
			if (y < Height() - 1)
			{
				this->GetPixel(x, y+1).SetClamp(
					this->GetPixel(x, y+1).r + r_qe * GAMMA,
					this->GetPixel(x, y+1).g + g_qe * GAMMA,
					this->GetPixel(x, y+1).b + b_qe * GAMMA
					);
			}
			if ((x < Width() - 1) && (y < Height() - 1) )
			{
				this->GetPixel(x+1, y+1).SetClamp(
					this->GetPixel(x+1, y+1).r + r_qe * DELTA,
					this->GetPixel(x+1, y+1).g + g_qe * DELTA,
					this->GetPixel(x+1, y+1).b + b_qe * DELTA
					);
			}
		}
	}
}



void Image::Blur(int n)
{
	// 1. create the gaussian filter (n by n)
	float sigma = 0.5; // for now set sigma (std deviation) to 0.5, play around with this value
	sigma = 2*pow(sigma,2); // 2*sigma^2 is used twice in the 2D gaussian equation
	float frc = 1/(2*M_PI*sigma); // the first half of the 2D gaussian equation, doesn't change based on and y

	float filter[n][n];
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			filter[i][j] = frc * exp(-(pow(n/2-i,2)+pow(n/2-j,2))/(2*sigma));
		}
	}

	// 2. apply the filter
	int x,y, n_x, n_y;
	float r,g,b;
	Pixel p, tp;
	Image* temp = new Image(Width(), Height());
		// temp image, image after filtering
		// avoid changing image while applying filters
	int radius;		// calculate radius of the filter, later when we're apply the filter we'll know where to start
	if (n%2 == 0)	// if even just do N/2, if odd do N/2 + 1
	{
		radius = n/2;
	}
	else
	{
		radius = n/2+1;
	}

	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			r = 0.0;
			g = 0.0;
			b = 0.0;
			// then go through each element of the filter
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
				{
					n_x = ((i+x-radius)+Width())%Width();
					n_y = ((j+y-radius)+Height())%Height();
					p = GetPixel(n_x,n_y); // add then mod by width and height ensures wrap around since negative mod-ing not working
					r += p.r * filter[i][j];
					g += p.g * filter[i][j];
					b += p.b * filter[i][j];
				}
			}
			tp.r = fmin(fmax(r,0),255);
			tp.g = fmin(fmax(g,0),255);
			tp.b = fmin(fmax(b,0),255);
			temp->SetPixel(x,y,tp);
		}
	}

	// replace image with temp
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			tp = temp->GetPixel(x,y);
			GetPixel(x,y) = tp;
		}
	}
}

void Image::Sharpen(int n)
{
	// at the moment, the code from sharpen will be almost identical to blur
	float sigma=0.5;sigma=2*pow(sigma,2);float frc=1/(2*M_PI*sigma);float filter[n][n];for(int i=0;i<n;i++){for(int j=0;j<n;j++){filter[i][j] = frc*exp(-(pow(n/2-i,2)+pow(n/2-j,2))/(2*sigma));}}int x,y,n_x,n_y;float r,g,b;Pixel p,tp;Image* temp=new Image(Width(),Height());int radius;if (n%2 == 0){radius = n/2;}else{radius = n/2+1;}for (x=0;x<Width();x++){for(y=0;y< Height();y++){r=0.0;g=0.0;b=0.0;for(int i=0;i<n;i++){for(int j=0;j<n;j++){n_x=((i+x-radius)+Width())%Width();n_y=((j+y-radius)+Height())%Height();p=GetPixel(n_x,n_y);r+=p.r*filter[i][j];g+=p.g*filter[i][j];b+=p.b*filter[i][j];}}tp.r=fmin(fmax(r,0),255);tp.g=fmin(fmax(g,0),255);tp.b=fmin(fmax(b,0),255);temp->SetPixel(x,y,tp);}}
	// the following line is the blur code with all white spaces removed

	// replace image with temp
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			// temp has the blurry pixel
			p = GetPixel(x,y);
			tp = temp->GetPixel(x,y);
			GetPixel(x,y) = PixelLerp(p,tp,-0.5); // interpolation from the normal image and blurry image
												  // set interpolation factor to -0.5 (arbitrary, just has to be negative since 'inverse' interpolation)
		}
	}
}

static int edFilter[3][3] =
{
	{-1, -1, -1},
	{-1, 8, -1},
	{-1, -1, -1}
};


void Image::EdgeDetect()
{
	int x, y, i, j;
	int n_x, n_y;
	float r, g, b;
	Pixel p, bp, tp; // pixels from: current image, blurred image, and temp (edge detected) image
	Image* blur = new Image(Width(), Height());
	Image* temp = new Image(Width(), Height());
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			p = GetPixel(x,y);
			blur->SetPixel(x,y,p);
		}
	}
	// blur the image (get rid of big details), getting ready for edge detection
	blur->Blur(5);

	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			//bp = blur->GetPixel(x,y);
			r = 0.0;
			g = 0.0;
			b = 0.0;
			// then go through each element of the filter
			for (i = 0; i < 3; i++) // limited by the size (3x3) of the filter
			{
				for (j = 0; j < 3; j++)
				{
					n_x = ((i+x-2)+Width())%Width();
					n_y = ((j+y-2)+Height())%Height();
					bp = blur->GetPixel(n_x,n_y);
					r += bp.r * edFilter[i][j];
					g += bp.g * edFilter[i][j];
					b += bp.b * edFilter[i][j];
				}
			}
			tp.r = fmin(fmax(r,0),255);
			tp.g = fmin(fmax(g,0),255);
			tp.b = fmin(fmax(b,0),255);
			temp->SetPixel(x,y,tp);
		}
	}

	// replace image with temp
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			// temp has the blurry pixel
			p = GetPixel(x,y);
			tp = temp->GetPixel(x,y);
			GetPixel(x,y) = tp;
		}
	}
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