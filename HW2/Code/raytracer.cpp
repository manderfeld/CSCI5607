//File parsing example

#include "image.h"
#include "vector.h"
#include "objects.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstddef>

#define DEBUG

#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "stb_image_write.h"

using namespace std;

int main(int argc, char* argv[]){

// Defaults
	const char* c = "raytraced.bmp"; // char pointer for image name. default is raytraced.bmp
	int w = 640; 					// default width
	int h = 480; 					// default height
	int rgb[] = {0, 0, 0};			// default backround RGB values
	sphere* sp = NULL; //= new sphere();
	string line;
	material* mat = new material(); // default material
	int md = 5; 					// default max depth
	// Camera default values
		float px = 0.0;
		float py = 0.0;
		float pz = 0.0;
		float dx = 0.0; 
		float dy = 0.0;
		float dz = 1.0;
		float ux = 0.0;
		float uy = 1.0;
		float uz = 0.0;
		float ha = 45.0;
	Camera* cam = NULL;
	cout << argv[1] << endl;

	// open the file containing the scene description
	ifstream input(argv[1]);

	// check for errors in opening the file
	if(input.fail()){
	cout << "Can't open file '" << argv[1] << "'" << endl;
	return 0;
	}

	// determine the file size (this is optional -- feel free to delete the 6 lines below)
	streampos begin,end;
	begin = input.tellg();
	input.seekg(0, ios::end);
	end = input.tellg();
	cout << "File '" << argv[1] << "' is: " << (end-begin) << " bytes long.\n\n";
	input.seekg(0, ios::beg);


	//Loop through reading each line
	string command;
	while(input >> command)
	{ //Read first word in the line (i.e., the command type)
		if (command[0] == '#')
		{
			getline(input, line); //skip rest of line
			printf("\tSkipping comment: %s%s\n", command.c_str(), line.c_str());
			//cout << "Skipping comment: " << command  << line <<  endl;
			continue;
		}
		if (command == "sphere")
		{
		//If the command is a sphere command
			float x,y,z,r;
			input >> x >> y >> z >> r;
			printf("Sphere as position (%f,%f,%f) with radius %f\n",x,y,z,r);
			if(sp == NULL)
				sp = new sphere(x, y, z, r, mat);
			else
				sp->add(x, y, z, r, mat);
		}
		else if (command == "background")
		{
		//If the command is a background command
			float r,g,b;
			input >> r >> g >> b;
			printf("Background color of (%f,%f,%f)\n",r,g,b);
			rgb[0] = r * 255;
			rgb[1] = g * 255;
			rgb[2] = b * 255;
		}
		else if (command == "camera")
		{
		//If the command is a camera command
			float pxi, pyi, pzi, dxi, dyi, dzi, uxi, uyi, uzi, hai;
			input >> pxi >> pyi >> pzi >> dxi >> dyi >> dzi >> uxi >> uyi >> uzi >> hai;
			printf("Camera at (%f,%f,%f) looking at (%f,%f,%f) with up (%f,%f,%f) and ha %f\n",pxi,pyi,pzi, dxi,dyi,dzi, uxi,uyi,uzi, hai);

			cam = new Camera(pxi, pyi, pzi, dxi, dyi, dzi, uxi, uyi, uzi, hai);

			px = pxi; py = pyi; pz = pzi;
			dx = dxi; dy = dyi; dz = dzi;
			ux = uxi; uy = uyi; uz = uzi;
			ha = hai;
		}
		else if (command == "film_resolution")
		{
			int wi, hi;
			input >> wi >> hi;
			printf("Image with width %d and height %d\n", wi, hi);
			w = wi;
			h = hi;
		}
		else if (command == "output_image")
		{
		//If the command is an output_image command
			string outFile;
			//c = outFile.c_str();
			input >> outFile;
			printf("Render to file named: %s\n", outFile.c_str());
			c = outFile.c_str();
		}
		else if (command == "material")
		{
		//If the command is a material command
			float ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior;
			input >> ar >> ag >> ab >> dr >> dg >> db >> sr >> dg >> sb >> ns >> tr >> tg >> tb >> ior;
			delete mat;
			mat = new material(ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior);
			printf("Material as (%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f)\n", ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior);
		}
		else if (command == "directional_light")
		{
		//If the command is a directional light command
			float r, g, b, x, y, z;
			input >> r >> g >> b >> x >> y >> z;
			printf("Directional light directed at (%f,%f,%f) with color of (%f,%f,%f)\n", x, y, z, r, g, b);
		}
		else if (command == "point_light")
		{
		//If the command is a point light command
			float r, g, b, x, y, z;
			input >> r >> g >> b >> x >> y >> z;
			printf("Point light at position (%f,%f,%f) with color of (%f,%f,%f)\n", x, y, z, r, g, b);
		}
		else if (command == "spot_light")
		{
		//If the command is a spot light command
			float r, g, b, px, py, pz, dx, dy, dz, a1, a2;
			input >> r >> g >> b >> px >> py >> pz >> dx >> dy >> dz >> a1 >> a2;
			printf("Spot light at position (%f,%f,%f) with color of (%f,%f,%f), direction (%f,%f,%f), and angles %f, and %f\n", px, py, pz, r, g, b, dx, dy, dz, a1, a2);

		}
		else if (command == "ambient_light")
		{
		//If the command is an ambient light command
			float r = 0, g = 0, b = 0;
			input >> r >> g >> b;
			printf("Ambient light color of (%f,%f,%f)\n", r, g, b);
		}
		else if(command == "max_depth")
		{
			//input >> md;
			//md = 
		}
		else
		{
			getline(input, line); //skip rest of line
			printf("\tWARNING. Do not know command: %s \n", command.c_str());
			//cout << "WARNING. Do not know command: " << command << endl;
		}
	}


	/////////////////
	// Debug stuff //
	/////////////////
	#ifdef DEBUG
		Vec3* a = new Vec3(1,2,3);
		Vec3* anormal = a->UnitVector();

		float mag = anormal->Magnitude();
		printf("mag: %f\n", mag);

		delete a;
		delete anormal;

		Vec3 o(px, py, pz), d(dx, dy, dz);
		Ray* r = new Ray(&o, &d);
		// Ray* r(px, py, pz, dx, dy, dz);

		intersect* surf = sp->hit(r);

		if (surf != NULL)
			printf("HIT (%f,%f,%f)\n", surf->hit.x, surf->hit.y, surf->hit.z);
		else
			printf("MISS\n");

		delete r;
	#endif
	/////////////////
	/////////////////

	
	char* name = (char*)c; // name of the image (convert const char* to char*)
	Image *img = new Image(w, h);
	img->Fill(rgb[0], rgb[1], rgb[2]);
	img->Write(name);

	if (cam == NULL)
		cam = new Camera;

	Vec3 V, pos;	// V is the directional vector of the current ray, pos is the current pixel position
	Vec3 D; 		// D is the direction vector from the camera augmented to fit the trigonometry for the calculation of V
	Vec3* d_u = cam->D.UnitVector();	// *_u denote unit vector
	Vec3* u_u = cam->U.UnitVector();
	Vec3* S = crossProd(*d_u, *u_u);	// vector in direction of positive width
	Vec3* s_u = S->UnitVector();
	float xpos, ypos;
	Ray P0;

	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{	
			// find V
			xpos = i - w/2.0;
			ypos = j - h/2.0;
			D = (h / (2 * atan(cam->ha))) * *d_u;
			pos = cam->O + D + (xpos * *u_u) + (ypos * *s_u);
			V = pos - cam->O;

			// Use V to find hits
			P0.o = cam->O;
			P0.d = V;
			intersect* surf = NULL, *hit = NULL;
			Sphere* now = sp;
			if (sp != NULL)
				hit = now->hit(P0);
				while (hit != NULL);	// finds the closest hit
				{
					now = hit->obj;
					if (hit->hit.Magnitude() > surf->hit.Magnitude)
					{
						delete surf;
						surf = hit;
					}
					else
					{
						delete hit;
					}
					hit = now->hit(P0);
				}
			
			// Image processing
			Pixel p;
			img->GetPixel(i, j) = p;
		}
	}

	delete d_u;
	delete u_u;
	delete S;
	delete s_u;

	return 0;
}
