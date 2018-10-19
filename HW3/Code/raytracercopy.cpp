//File parsing example 

#include "image.h"
#include "vector.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "stb_image_write.h"

// ray class make separate files later (TODO)
struct Ray
{
	Vec3 o; // origin
	Vec3 d; // direction
	Ray(const Vec3& o, const Vec3& d) : o(o), d(d) {}
};

struct Sphere
{
	// fill this with information such as xyz position, radius, material, etc
};

using namespace std;

int main(int argc, char* argv[]){

	const char* c = "raytraced.bmp"; // char pointer for image name. default is raytraced.bmp
	int w = 640; // default width
	int h = 480; // default height
	string line;
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
			cout << "Skipping comment: " << command  << line <<  endl;
			continue;
		}
		if (command == "sphere")
		{
		//If the command is a sphere command
			float x,y,z,r;
			input >> x >> y >> z >> r;
			printf("Sphere as position (%f,%f,%f) with radius %f\n",x,y,z,r);
		}
		else if (command == "background")
		{
		//If the command is a background command
			float r,g,b;
			input >> r >> g >> b;
			printf("Background color of (%f,%f,%f)\n",r,g,b);
		}
		else if (command == "film_resolution")
		{
			input >> w >> h;
			printf("Image with width %d and height %d\n", w, h);
		}
		else if (command == "output_image")
		{
		//If the command is an output_image command
			string outFile;
			c = outFile.c_str();
			input >> outFile;
			printf("Render to file named: %s\n", c);
		}
		else
		{
			getline(input, line); //skip rest of line
			cout << "WARNING. Do not know command: " << command << endl;
		}
	}

	char* name = (char*)c; // name of the image (convert const char* to char*)
	Image *img = new Image(w, h);
	img->Fill(0,0,0);
	img->Write(name);

	return 0;
}
