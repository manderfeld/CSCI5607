//File parsing example

#include "image.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "stb_image_write.h"


using namespace std;

int main(int argc, char* argv[]){

	const char* c;
  string line;
cout << argv[1] << endl;
  //string fileName = "../Scenes/spheres1.scn";

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
  while(input >> command) { //Read first word in the line (i.e., the command type)
    
    if (command[0] == '#'){
      getline(input, line); //skip rest of line
      cout << "Skipping comment: " << command  << line <<  endl;
      continue;
    }
    
    
    if (command == "sphere"){ //If the command is a sphere command
       float x,y,z,r;
       input >> x >> y >> z >> r;
       printf("Sphere as position (%f,%f,%f) with radius %f\n",x,y,z,r);
    }
    else if (command == "background"){ //If the command is a background command
       float r,g,b;
       input >> r >> g >> b;
       printf("Background color of (%f,%f,%f)\n",r,g,b);
    }
    else if (command == "output_image"){ //If the command is an output_image command
       string outFile;
       c = outFile.c_str();
       input >> outFile;
       printf("Render to file named: %s\n", c);
    }
    else {
      getline(input, line); //skip rest of line
      cout << "WARNING. Do not know command: " << command << endl;
    }
  }
/*
int n = argv[1].length();
char* w = new char[n+1];
copy(fileName.begin(), fileName.end(), w);
w[n] = '\0';
*/
//char* c = fileName.c_str();
/*char ar[n+1];
strcpy(ar, fileName.c_str());
for (int i=0; i<n; i++)
{
	cout << ar[i];
}*/

char* w = (char*)c;
Image *img = NULL;
img = new Image(w);


//delete[] w;
  
  return 0;
}
