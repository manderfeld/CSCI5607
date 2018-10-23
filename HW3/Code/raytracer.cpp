//File parsing example

#include "image.h"
#include "vector.h"
#include "objects.h"
#include <vector>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstddef>
#include <sys/time.h>

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
	Sphere* sp = NULL; //= new Sphere();
	int maxV;
	vector<Vec3> vertices;
	Triangle* tr = NULL;
	string line;
	material* mat = new material(); // default material
	// TODO: multiple materials
	int md = 5; 					// default max depth
	
	// Camera
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

	// Lighting
	// Directional light
		float dl_r, dl_g, dl_b, dl_x, dl_y, dl_z;
		vector<Light*> dl_list;
	// Point light
		float pl_r, pl_g, pl_b, pl_x, pl_y, pl_z;
		vector<Light*> pl_list;
	// Spot light
		float sl_r, sl_g, sl_b, sl_px, sl_py, sl_pz, sl_dx, sl_dy, sl_dz, sl_a1, sl_a2;
		vector<Light*> sl_list;
	// Ambient light
		float al_r = 0, al_g = 0, al_b = 0;
	
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
				sp = new Sphere(x, y, z, r, mat);
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
		else if (command == "max_vertices")
		{
		//If the command is a max_vertices command
			int maxV_i;
			input >> maxV_i;
			printf("Max vertices: %d\n",maxV_i);
			maxV = maxV_i;
		}
		else if (command == "vertex")
		{
		//If the command is a max_vertices command
			if (maxV <= vertices.size())
			{
				printf("Cannot add another vertex! At max capacity :(\n");
			}
			else
			{
				float x,y,z;
				input >> x >> y >> z;
				printf("Vertex %lu (%f, %f, %f)\n", vertices.size()+1, x, y, z);
				Vec3 vertex(x,y,z);
				vertices.push_back(vertex);
			}
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
			input >> ar >> ag >> ab >> dr >> dg >> db >> sr >> sg >> sb >> ns >> tr >> tg >> tb >> ior;
			delete mat;
			mat = new material(ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior);
			printf("Material as (%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f)\n", ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior);
		}
		else if (command == "directional_light")
		{
		//If the command is a directional light command
			input >> dl_r >> dl_g >> dl_b >> dl_x >> dl_y >> dl_z;
			printf("Directional light directed at (%f,%f,%f) with color of (%f,%f,%f)\n", dl_x, dl_y, dl_z, dl_r, dl_g, dl_b);
			Light* dl = new Light(dl_r, dl_g, dl_b, dl_x, dl_y, dl_z);
			dl_list.push_back(dl);
		}
		else if (command == "point_light")
		{
		//If the command is a point light command
			input >> pl_r >> pl_g >> pl_b >> pl_x >> pl_y >> pl_z;
			printf("Point light at position (%f,%f,%f) with color of (%f,%f,%f)\n", pl_x, pl_y, pl_z, pl_r, pl_g, pl_b);
			Light* pl = new Light(pl_r, pl_g, pl_b, pl_x, pl_y, pl_z);
			pl_list.push_back(pl);
		}
		else if (command == "spot_light")
		{
		//If the command is a spot light command
			input >> sl_r >> sl_g >> sl_b >> sl_px >> sl_py >> sl_pz >> sl_dx >> sl_dy >> sl_dz >> sl_a1 >> sl_a2;
			printf("Spot light at position (%f,%f,%f) with color of (%f,%f,%f), direction (%f,%f,%f), and angles %f, and %f\n", sl_px, sl_py, sl_pz, sl_r, sl_g, sl_b, sl_dx, sl_dy, sl_dz, sl_a1, sl_a2);
			Light* sl = new Light(sl_r, sl_g, sl_b, sl_px, sl_py, sl_pz);
			sl_list.push_back(sl);
		}
		else if (command == "ambient_light")
		{
		//If the command is an ambient light command
			input >> al_r >> al_g >> al_b;
			printf("Ambient light color of (%f,%f,%f)\n", al_r, al_g, al_b);
		}
		else if(command == "max_depth")
		{
			int mdi;
			input >> mdi;
			printf("Max depth is %d\n", mdi);
			md = mdi;
		}
		else
		{
			getline(input, line); //skip rest of line
			printf("\tWARNING. Do not know command: %s \n", command.c_str());
			//cout << "WARNING. Do not know command: " << command << endl;
		}
	}

	char* name = (char*)c; // name of the image (convert const char* to char*)
	Image *img = new Image(w, h);
	img->Fill(rgb[0], rgb[1], rgb[2]);

	if (cam == NULL)
		cam = new Camera;

	Vec3 *V, pos;	// V is the directional vector of the current ray, pos is the current pixel position
	Vec3 D; 		// D is the direction vector from the camera augmented to fit the trigonometry for the calculation of V
	Vec3* d_u = cam->D.UnitVector();	// *_u denote unit vector
	Vec3* u_u = cam->U.UnitVector();
	Vec3* S = crossProd(*d_u, *u_u);	// vector in direction of positive width
	Vec3* s_u = S->UnitVector();
	float xpos, ypos;
	Ray* P0 = new Ray();

	D = h / (2 * tanf(cam->ha * M_PI/180.0f)) * *d_u;

	//printf("Unit d: %f,%f,%f\n", D.x, D.y, D.z);

/*************************************************
 *	FOR SPHERES!!!!                              *
 *************************************************/
/*	#pragma omp parallel for
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{	
			// find V
			xpos = w/2.0 - i;
			ypos = h/2.0 - j;
			pos = cam->O + D + (ypos * *u_u) + (xpos * *s_u);
			V = (pos - cam->O).UnitVector();

			// Use V to find hits
			P0->o = cam->O;
			P0->d = *V;
			intersect* surf = NULL, *hit = NULL;
			Sphere* now = sp;
			if (sp != NULL)
			{
				hit = NULL;
				hit = now->hit(P0);
			}

			Pixel p = img->GetPixel(i, j);
			while (hit != NULL) // HIT
			{
				// printf("HIT (%f,%f,%f)\n", hit->hit.x, hit->hit.y, hit->hit.z);
				if (surf == NULL)
					surf = hit;
				else
				{
					Vec3 measure1 = hit->hit - cam->O;
					Vec3 measure2 = surf->hit - cam->O;
					if(measure1.Magnitude() < measure2.Magnitude())
						surf = hit;
				}
				now = hit->obj->next;
				if (now == NULL)
					break;
				
				hit = now->hit(P0);
			}

			if (surf != NULL)
			{
				Vec3 vhit(surf->hit.x, surf->hit.y, surf->hit.z); // Position that you hit
				now = surf->obj; // Object that you hit
				material* color = now->mat;

				// pixel values, if no lighting info, same as background
				float r = p.r;
				float g = p.g;
				float b = p.b;
				// ambient
					float ar = 0.0;
					float ag = 0.0;
					float ab = 0.0; // ambient
				// diffuse (Lambertian)
					float dr = 0.0;
					float dg = 0.0;
					float db = 0.0;
				// specular (Phong)
					float sr = 0.0;
					float sg = 0.0;
					float sb = 0.0;

				// ambient light
				ar = color->ar*al_r;
				ag = color->ag*al_g;
				ab = color->ab*al_b;

				// Labmertian and Phong
				// RIGHT NOW, assuming one point light
				// TODO: Multiple point lights!
				if (pl_list.begin() != pl_list.end())
				{
					vector<Light*>:: iterator i;
					for (i = pl_list.begin(); i != pl_list.end(); i++)
					{
						// LAMBERTIAN
						Light* pl = (*i); // look at first point light

						Vec3 n = vhit - now->O; // normal
						Vec3* n_n = n.UnitVector(); // unit vector version of the normal vector
						
						Vec3 l = pl->position - vhit;
						Vec3* l_n = l.UnitVector();

						/////////////
						// Shadows //
						/////////////

						struct timeval tp;
						gettimeofday(&tp, NULL);
						long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;

						Ray* light = new Ray();	// ray for shadow checking
						light->o = vhit + (0.00001 * *l_n);	// move the ray's origin very slightly towards the light source to avoid shadow acne
						light->d = *l_n;
						intersect* shadow = sp->hit(light);	// check if there is an object between object and light source.
						delete light;
						if (shadow != NULL)
						{
							delete shadow;
							// cout << "Shadow" << endl;
							continue;
						}

						float nl = dotProd(*n_n, *l_n); // dotProd(normal unit vector, light unit vector)
						if ( nl < 0)
						{
							nl = 0;
						}

						float d2 = dotProd(l, l); // attenuation: distance from light squared
						//printf("d2: %f\n", d2); 

						dr += (color->dr)/d2 * (pl->r) * nl;
						dg += (color->dg)/d2 * (pl->g) * nl;
						db += (color->db)/d2 * (pl->b) * nl;

						// PHONG
						// we will use the Vec3* l_n n_n from before (light and normal vectors)
						// Is = ks *( pow(dotProd(V,R),n) * Il)
						Vec3 r = (2 * dotProd(*n_n, *l_n)) * *n_n - *l_n;
						//float rmag = r.Magnitude();
						//printf("rmag: %f\n", rmag);

						// -1.0 * *V becau se want FROM intersectino TO eye (not other way around)
						float n_dot_h = dotProd(-1.0 * *V, r);
						if (n_dot_h < 0)
						{
							n_dot_h = 0;
						}

						sr += (color->sr)/d2 * pow( n_dot_h ,color->ns) * pl->r;
						sg += (color->sg)/d2 * pow( n_dot_h ,color->ns) * pl->g;
						sb += (color->sb)/d2 * pow( n_dot_h ,color->ns) * pl->b;
						delete n_n;
						delete l_n;
					}
				}

				////////////////
				// Reflection //
				////////////////
				float rr = 0, rg = 0, rb = 0;
				float tr = 0, tg = 0, tb = 0;

				


				// Do ambient, diffuse, and specular all at once!
				r = ar + dr + sr;
				g = ag + dg + sg;
				b = ab + db + sb;

				r = (r > 1) ? 1 : r;
				g = (g > 1) ? 1 : g;
				b = (b > 1) ? 1 : b;
	
				p.r = 255 * r;
				p.g = 255 * g;
				p.b = 255 * b;
				p.a = 255;
				delete surf;
			}
			else 			 // MISS
			{
				// Miss, don't change anything because it's the background
			}
			
			delete V;
			// Image processing
			img->GetPixel(i, j) = p;
		}
	}
*/





/*************************************************
 *	FOR TRIANGLES!!!!                             *
 *************************************************/
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{	



			/*
			// find V
			xpos = w/2.0 - i;
			ypos = h/2.0 - j;
			pos = cam->O + D + (ypos * *u_u) + (xpos * *s_u);
			V = (pos - cam->O).UnitVector();

			// Use V to find hits
			P0->o = cam->O;
			P0->d = *V;
			intersect* surf = NULL, *hit = NULL;
			Sphere* now = sp;
			if (sp != NULL)
			{
				hit = NULL;
				hit = now->hit(P0);
			}

			Pixel p = img->GetPixel(i, j);
			while (hit != NULL) // HIT
			{
				// printf("HIT (%f,%f,%f)\n", hit->hit.x, hit->hit.y, hit->hit.z);
				if (surf == NULL)
					surf = hit;
				else
				{
					Vec3 measure1 = hit->hit - cam->O;
					Vec3 measure2 = surf->hit - cam->O;
					if(measure1.Magnitude() < measure2.Magnitude())
						surf = hit;
				}
				now = hit->obj->next;
				if (now == NULL)
					break;
				
				hit = now->hit(P0);
			}

			if (surf != NULL)
			{
				Vec3 vhit(surf->hit.x, surf->hit.y, surf->hit.z); // Position that you hit
				now = surf->obj; // Object that you hit
				material* color = now->mat;

				// pixel values, if no lighting info, same as background
				float r = p.r;
				float g = p.g;
				float b = p.b;
				// ambient
					float ar = 0.0;
					float ag = 0.0;
					float ab = 0.0; // ambient
				// diffuse (Lambertian)
					float dr = 0.0;
					float dg = 0.0;
					float db = 0.0;
				// specular (Phong)
					float sr = 0.0;
					float sg = 0.0;
					float sb = 0.0;

				// ambient light
				ar = color->ar*al_r;
				ag = color->ag*al_g;
				ab = color->ab*al_b;

				// Labmertian and Phong
				// RIGHT NOW, assuming one point light
				// TODO: Multiple point lights!
				if (pl_list.begin() != pl_list.end())
				{
					vector<Light*>:: iterator i;
					for (i = pl_list.begin(); i != pl_list.end(); i++)
					{
						// LAMBERTIAN
						Light* pl = (*i); // look at first point light

						Vec3 n = vhit - now->O; // normal
						Vec3* n_n = n.UnitVector(); // unit vector version of the normal vector
						
						Vec3 l = pl->position - vhit;
						Vec3* l_n = l.UnitVector();

						/////////////
						// Shadows //
						/////////////

						struct timeval tp;
						gettimeofday(&tp, NULL);
						long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;

						Ray* light = new Ray();	// ray for shadow checking
						light->o = vhit + (0.00001 * *l_n);	// move the ray's origin very slightly towards the light source to avoid shadow acne
						light->d = *l_n;
						intersect* shadow = sp->hit(light);	// check if there is an object between object and light source.
						delete light;
						if (shadow != NULL)
						{
							delete shadow;
							// cout << "Shadow" << endl;
							continue;
						}

						float nl = dotProd(*n_n, *l_n); // dotProd(normal unit vector, light unit vector)
						if ( nl < 0)
						{
							nl = 0;
						}

						float d2 = dotProd(l, l); // attenuation: distance from light squared
						//printf("d2: %f\n", d2); 

						dr += (color->dr)/d2 * (pl->r) * nl;
						dg += (color->dg)/d2 * (pl->g) * nl;
						db += (color->db)/d2 * (pl->b) * nl;

						// PHONG
						// we will use the Vec3* l_n n_n from before (light and normal vectors)
						// Is = ks *( pow(dotProd(V,R),n) * Il)
						Vec3 r = (2 * dotProd(*n_n, *l_n)) * *n_n - *l_n;
						//float rmag = r.Magnitude();
						//printf("rmag: %f\n", rmag);

						// -1.0 * *V becau se want FROM intersectino TO eye (not other way around)
						float n_dot_h = dotProd(-1.0 * *V, r);
						if (n_dot_h < 0)
						{
							n_dot_h = 0;
						}

						sr += (color->sr)/d2 * pow( n_dot_h ,color->ns) * pl->r;
						sg += (color->sg)/d2 * pow( n_dot_h ,color->ns) * pl->g;
						sb += (color->sb)/d2 * pow( n_dot_h ,color->ns) * pl->b;
						delete n_n;
						delete l_n;
					}
				}

				////////////////
				// Reflection //
				////////////////
				float rr = 0, rg = 0, rb = 0;
				float tr = 0, tg = 0, tb = 0;

				


				// Do ambient, diffuse, and specular all at once!
				r = ar + dr + sr;
				g = ag + dg + sg;
				b = ab + db + sb;

				r = (r > 1) ? 1 : r;
				g = (g > 1) ? 1 : g;
				b = (b > 1) ? 1 : b;
	
				p.r = 255 * r;
				p.g = 255 * g;
				p.b = 255 * b;
				p.a = 255;
				delete surf;
			}
			else 			 // MISS
			{
				// Miss, don't change anything because it's the background
			}
			
			delete V;
			// Image processing
			img->GetPixel(i, j) = p;
			*/
		}
	}
  
  



	//Image* ret = img->Resample();
	/*
	Image *im2 = new Image(w, h);

	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			float gaus[] = {0.058549832, 0.096532353, 0.058549832, 0.096532353, 0.159154943, 0.096532353, 0.058549832, 0.096532353, 0.058549832};
			float corner = 0.058549832;
			float side = 0.096532353;
			float center = 0.159154943;
			Pixel p;
			p.r = 0;
			p.g = 0;
			p.b = 0;
			p.a = 255;
			for (int k = -1; k <= 1; k++){
				for (int l = -1; l <= 1; l++){
					Pixel pix;
					if(i+k > 0 && i+k < w && j+l > 0 && j+1 < h)
					{
						pix = img->GetPixel(i + k, j + l);
					}
					else{
						pix = img->GetPixel(i, j);
					}
					p.r += pix.r * gaus[(k+1)*3 + l + 1];
					p.g += pix.g * gaus[(k+1)*3 + l + 1];
					p.b += pix.b* gaus[(k+1)*3 + l + 1];
				}
			}
			im2->GetPixel(i, j) = p;
		}
	}
	*/

	//im2->Write(name);

	delete d_u;
	delete u_u;
	delete S;
	delete s_u;

// TODO: go through pl_list and delete the vectors in the list and then delete the list

	// cout << "name: " << name << endl;
	img->Write(name);
	//ret->Write(name);
	return 0;
}
