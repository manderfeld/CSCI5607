const char* INSTRUCTIONS = 
"***************\n"
"This is 3D Maze\n"
"\n"
"Up/down     -  move forwards and backwards\n"
"Left/right  -  rotate to the left or right\n"
"***************\n"
;

#include "glad/glad.h"  //Include order can matter here
#if defined(__APPLE__) || defined(__linux__)
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
#endif
#include <cstdio>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

int screenWidth = 800; 
int screenHeight = 600;  
float timePast = 0;

bool DEBUG_ON = false;
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
bool fullscreen = false;
void Win2PPM(int width, int height);

//srand(time(NULL));
float rand01(){
	return rand()/(float)RAND_MAX;
}

// Doors and keys
// TODO: make these not globals. Maybe classes?
map<int, bool> doorInfo;
map<int, bool> keyInfo;
	// Colors of doors and keys
	char colors[5] = {'r' , 'g' , 'b', 'c', 'm'};

// Player attributes and related variables
int activeItem = -1;
int itemList[5]; // store list of int representing keys, most recent will be in back
int itemAmt = 0;

float playerRadius = .2;
float pickupRadius = .22;

float c_pos_x, c_pos_y, c_pos_z, c_dir_x, c_dir_y, c_dir_z, c_theta;
float c_pos_xi, c_pos_yi;

void makeMap(vector<string>, map<int, vector< pair<int, char> > > &bigMap, float arr[]);
void makeLevel(int texturedShader, int modelList[], map<int, vector< pair<int, char> > >bigMap, int w, int h);
void makeMapOld(int texturedShader, int modelList[], char** ret, int w, int h);
void parseInput(int texturedShared, int modelList[], vector<string> input, int w, int h, float arr[]);
void addFloor(int shaderProgram, int verts[], int w, int h, float x, float y, float z);
void addModel(int shaderProgram, int verts[], int mod, int tex, int clr, float w, float h, float d, float x, float y, float z);
void drawGeometry(int shaderProgram, int verts[]);

GLuint texture(const char* input)
{
		SDL_Surface* surface = SDL_LoadBMP(input);
		if (surface==NULL)
		{ //If it failed, print the error
	        printf("Error: \"%s\"\n",SDL_GetError());
	    }
	    GLuint temp;
	    glGenTextures(1, &temp);
	    
	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, temp);
	    
	    //What to do outside 0-1 range
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    
	    //Load the texture into memory
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
	    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture
	    
	    SDL_FreeSurface(surface);

	    printf("Loading texture: %s\n", input);
	    return temp;
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
	//Ask SDL to get a recent version of OpenGL (3.2 or greater)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	//Create a window (offsetx, offsety, width, height, flags)
		SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	//Create a context to draw in
		SDL_GLContext context = SDL_GL_CreateContext(window);
	//Load OpenGL extentions with GLAD
		if (gladLoadGLLoader(SDL_GL_GetProcAddress))
		{
			printf("\nOpenGL loaded\n");
			printf("Vendor:   %s\n", glGetString(GL_VENDOR));
			printf("Renderer: %s\n", glGetString(GL_RENDERER));
			printf("Version:  %s\n\n", glGetString(GL_VERSION));
		}
		else
		{
			printf("ERROR: Failed to initialize OpenGL context.\n");
			return -1;
		}
	
// Load model files
	//Load Model 1
		ifstream modelFile;
		modelFile.open("models/teapot.txt");
		int numLines = 0;
		modelFile >> numLines;
		float* model1 = new float[numLines];
		for (int i = 0; i < numLines; i++)
		{
			modelFile >> model1[i];
		}
		printf("%d\n",numLines);
		int numVertsTeapot = numLines/8;
		modelFile.close();
	
	//Load Model 2
		modelFile.open("models/knot.txt");
		numLines = 0;
		modelFile >> numLines;
		float* model2 = new float[numLines];
		for (int i = 0; i < numLines; i++)
		{
			modelFile >> model2[i];
		}
		printf("%d\n",numLines);
		int numVertsKnot = numLines/8;
		modelFile.close();

	//Load Model 3
		modelFile.open("models/cube.txt");
		numLines = 0;
		modelFile >> numLines;
		float* model3 = new float[numLines];
		for (int i = 0; i < numLines; i++)
		{
			modelFile >> model3[i];
		}
		printf("%d\n",numLines);
		int numVertsCube = numLines/8;
		modelFile.close();
	//Load Model 4
		modelFile.open("models/knot.txt");
		numLines = 0;
		modelFile >> numLines;
		float* model4 = new float[numLines];
		for (int i = 0; i < numLines; i++)
		{
			modelFile >> model4[i];
		}
		printf("%d\n",numLines);
		int numVertsSphere = numLines/8;
		modelFile.close();

	//Concatenate model arrays
		int model2Offset = numVertsTeapot*8; // offset used for model2
		int model3Offset = model2Offset+numVertsKnot*8;
		int model4Offset = model3Offset+numVertsCube*8;

		float* modelData = new float[(numVertsTeapot+numVertsKnot+numVertsCube+numVertsSphere)*8];
		copy(model1, model1+numVertsTeapot*8, modelData);
		copy(model2, model2+numVertsKnot*8, modelData+model2Offset);
		copy(model3, model3+numVertsCube*8, modelData+model3Offset);
		copy(model4, model4+numVertsSphere*8, modelData+model4Offset);

		int totalNumVerts = numVertsTeapot+numVertsKnot+numVertsCube+numVertsSphere;

		// Put these into an array for startVerts and numVerts of all the models
		int startVertTeapot = 0;  //The teapot is the first model in the VBO
		int startVertKnot = numVertsTeapot; //The knot starts right after the taepot
		int startVertCube = startVertKnot + numVertsKnot;
		int startVertSphere = startVertCube + numVertsSphere;

		int modelList[4*2]; //make an array the size (number of models)*2 since storing 2 data points for each moel
		// find a better way to do this, for now hard coding
		modelList[0] = startVertTeapot;
		modelList[1] = numVertsTeapot;
		modelList[2] = startVertKnot;
		modelList[3] = numVertsKnot;
		modelList[4] = startVertCube;
		modelList[5] = numVertsCube;
		modelList[6] = startVertSphere;
		modelList[7] = numVertsSphere;

	//// Allocate Textures ///////
		string file = "walls.bmp";
		GLuint tex0 = texture(file.c_str());
		
		file = "ground.bmp";
		GLuint tex1 = texture(file.c_str());

    //// End Allocate Texture ///////
	
	//Build a Vertex Array Object (VAO) to store mapping of shader attributse to VBO
		GLuint vao;
		glGenVertexArrays(1, &vao); //Create a VAO
		glBindVertexArray(vao); //Bind the above created VAO to the current context

	//Allocate memory on the graphics card to store geometry (vertex buffer object)
		GLuint vbo[1];
		glGenBuffers(1, vbo);  //Create 1 buffer called vbo
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
		glBufferData(GL_ARRAY_BUFFER, totalNumVerts*8*sizeof(float), modelData, GL_STATIC_DRAW); //upload vertices to vbo
		//GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
		//GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used
	
	int texturedShader = InitShader("textured-Vertex.glsl", "textured-Fragment.glsl");	
	
	//Tell OpenGL how to set fragment shader input 
	GLint posAttrib = glGetAttribLocation(texturedShader, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	  //Attribute, vals/attrib., type, isNormalized, stride, offset
	glEnableVertexAttribArray(posAttrib);
	
	//GLint colAttrib = glGetAttribLocation(phongShader, "inColor");
	//glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	//glEnableVertexAttribArray(colAttrib);
	
	GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(normAttrib);
	
	GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

	GLint uniView = glGetUniformLocation(texturedShader, "view");
	GLint uniProj = glGetUniformLocation(texturedShader, "proj");

	glBindVertexArray(0); //Unbind the VAO in case we want to create a new one	
                       
	glEnable(GL_DEPTH_TEST);  

	printf("%s\n",INSTRUCTIONS);



/////////////////////////////////////////////////
// TODO: could probably work in a class structure of some sort to store all of these variables in a much better way
// Setting up variables
// Map
// Open the level file
// start by putting each line in a vector of strings (each line is in its own row)
// then go through each row and assign coordinates based on row and col where col is placement of character in line

// TODO: find a more efficient way to do this

	vector<string> lines;

	cout << "* BEGIN FILE INPUT" << endl;
	cout << "\tReading file: " << argv[1] << endl;
	ifstream input(argv[1]);

// Check for errors opening the file
		if(input.fail())
		{
			cout << "\tCannot open file: '" << argv[1] << "'" << endl;
			return 0;
		}
// First off, get width and height
		int map_w, map_h;
		input >> map_w >> map_h;
		cout << "\tMaking map (w,h):  (" << map_w << ", " << map_h << ")" << endl;

// Loop through reading each line and put it in the lines vector
		string line;
		while(input >> line)
		{
			lines.push_back(line);
		}

// Really convoluted way of storing this but I kept getting seg faults with 2d dynamic arrays
		map<int, vector< pair<int, char> > > bigMap;
		float arr[2]; // array for camera position
		makeMap(lines, bigMap, arr);

		// then go through array
		for (map<int, vector< pair<int, char> > >::iterator it = bigMap.begin(); it != bigMap.end(); it++)
		{
			int x = it->first;
			vector< pair<int, char> > val = it->second;
			for (vector< pair<int, char> >::iterator itt = val.begin(); itt != val.end(); itt++)
			{
				int y = itt->first;
				char let = itt->second;
				cout << "|" << let;
				//cout << "At (x,y) there is char ---  " << x << " , " << y << ": " << let << endl;
			}
			cout << endl;
		}

	cout << "* END FILE INPUT" << endl;
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// VARIABLES
bool up = false, down = false, left = false, right = false;
float pos_res = 0.05, // resolution (amount we wish to modify things by)
	th_res = pos_res/2.0;

c_pos_x = arr[0]; c_pos_y = arr[1];
c_theta = (3*M_PI)/2;

/////////////////////////////////////////////////

	//Event Loop (Loop forever processing each event as fast as possible)
	SDL_Event windowEvent;

	bool quit = false;
	int init = -1;
	while (!quit)
	{
		while (SDL_PollEvent(&windowEvent))
		{  //inspect all events in the queue
			if (windowEvent.type == SDL_QUIT) quit = true;
			//List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
			//Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) 
			quit = true; //Exit event loop
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f)
			{
				//If "f" is pressed
				fullscreen = !fullscreen;
				SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Toggle fullscreen 
			}

			//SJG: Use key input to change the state of the object
			//     We can use the ".mod" flag to see if modifiers such as shift are pressed
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP)
			{
				//If "up key" is pressed
				up = true;
			}
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_UP)
			{
				up = false;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN)
			{
				//If "down key" is pressed
				down = true;
			}
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_DOWN)
			{
				down = false;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT)
			{
				//If "left key" is pressed
				left = true;
			}
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_LEFT)
			{
				left = false;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT)
			{
				//If "right key" is pressed
				right = true;
			}
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_RIGHT)
			{
				right = false;
			}
		}

		// if/elses for whether the key is going from  pressed->released  or  not pressed->pressed
		if (up)
		{
			c_pos_xi = c_pos_x;
			c_pos_yi = c_pos_y;
			c_pos_x = c_pos_x + pos_res*cosf(c_theta);
			c_pos_y = c_pos_y + pos_res*sinf(c_theta);
		}
		if (down)
		{
			c_pos_xi = c_pos_x;
			c_pos_yi = c_pos_y;
			c_pos_x = c_pos_x - pos_res*cosf(c_theta);
			c_pos_y = c_pos_y - pos_res*sinf(c_theta);
		}
		if (left)
		{
			c_theta += th_res;
		}
		if (right)
		{
			c_theta -= th_res;
		}

		// Clear the screen to default color
			//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClearColor(.3f, 0.5f, 0.9f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(texturedShader);
		//bool ret = isWalkable(c_pos_x, c_pos_y, c_pos_xi, c_pos_yi);

		if (init>0)
		{
			//cout << "init > 0" << endl;
			bool ret = true;

				// ll    is x-1, y-1
				// left  is x-1, y
				// ul    is x-1, y+1
				// down  is x, y-1
			/////// you are x, y (don't count this...)
				// up    is x, y+1
				// lr    is x+1, y-1
				// right is x+1, y
				// ur    is x+1, y+1
			for (int dx = -1; dx < 2; dx++)
			{
				for (int dy = -1; dy < 2; dy++)
				{
					if ( (dx==0) || (dy==0) )
						{continue;} // ignore
					// If things are weird.. check this (from the lua game seudo code)
					int i = floor(c_pos_x + playerRadius*dx);
					int j = floor(c_pos_y + playerRadius*dy);

					if( (i < 1) || (j < 1) || (i > map_w) || (j > map_h) )
					{
						ret = false;
					}
					if ( (i>0)&&(i<map_w)&&(j>0)&&(j<map_h) ) 
					{
						vector< pair<int, char> > row = bigMap[j];
						pair<int,char> coord = row[i];
						char let = coord.second;
						if(let == 'W')
						{
							ret = false;
						}
						// door
						else if ( (let>='A') && (let<='E') )
						{
							//cout << "it's a door!" << endl;
							// if it's a door, check if we have the key for it
							// if we don't have the key, it's a wall
							if (keyInfo[let-'A'] == true)
							{
								//cout << "   don't have key" << endl;
								//								if (!doorInfo[let-'A'])
								doorInfo[let-'A'] = true; // set bool to true (we've opened the door, so hide it later)
								ret = true;
							}
							else
							{
								ret = false;
							}
						}
						// key
						else if ( (let>='a') && (let<='e') )
						{
							//cout << "I see a key" << endl;
							keyInfo[let-'a'] = true;
							//key[let-'a'] = true; // change to true
							activeItem = let-'a';
							//cout << "active key is:  " << let << endl;
						}
						else if (let == 'G') // if it's the goal we exit the maze! we are free!!
						{
							quit = true;
						}
					}
				}
			}
			if (!ret) // if not walkable, do not allow movement
			{
				c_pos_x = c_pos_xi;
				c_pos_y = c_pos_yi;
			}
		}

		timePast = SDL_GetTicks()/1000.f; 
		// Have camera look at stuff
			glm::mat4 view = glm::lookAt(
			glm::vec3(c_pos_x, c_pos_y, c_pos_z),  //Cam Position
			glm::vec3(c_pos_x + cosf(c_theta), c_pos_y + sinf(c_theta), c_pos_z),
			glm::vec3(0.0f, 0.0f, 1.0f)); //Up
			glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	// may have to change the value of far if the maze is huge
		glm::mat4 proj = glm::perspective(3.14f/4, screenWidth / (float) screenHeight, 0.005f, 100.0f); //FOV, aspect, near, far
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

		// textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex0);
			glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex1);
			glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);

		glBindVertexArray(vao);
		makeLevel(texturedShader, modelList, bigMap, map_w, map_h);

		SDL_GL_SwapWindow(window); //Double buffering
		init++;
	}

	//Clean Up
		glDeleteProgram(texturedShader);
    	glDeleteBuffers(1, vbo);
    	glDeleteVertexArrays(1, &vao);
		SDL_GL_DeleteContext(context);
		SDL_Quit();

	return 0;
}

void makeMap(vector<string> input, map<int, vector< pair<int, char> > > &bigMap, float arr[])
{
	// make a big unordered map...
	/*
	{0, < <0, 'W'> , <1, 'W'> , <2, '0'> , <3, '0'> >}
	{1, <0, 1, 2, 3>} etc...
	*/
	for (int in = 0; in < input.size(); in++) // go through each line that was parsed
	{
		string row = input[in];
		// each line is the row index, will be the key value for the map
		for (int st = 0; st < row.length(); st++) // look at each letter in the line
		{
			char let = row[st]; // each letter is a pair with its y (st) coordinate
			if (let == 'S')
			{
				arr[0] = float(st)+.5;
				arr[1] = float(in)+.5;
			}
			// its x coordinate is the key of the map. the value is the pair <#coord#, 'char'>
			pair<int, char> val = make_pair(st, let);
			bigMap[in].push_back(val);
		}
	}
}

void makeLevel(int texturedShader, int modelList[], map<int, vector< pair<int, char> > >bigMap, int w, int h)
{
	float floorw = (float)w;
	float floorh = (float)h;
	float floorx, floory;

	floorx = floorw/2;
	floory = floorh/2-0.5;

	addModel(texturedShader, modelList, 2, 1, 0, floorw, floorh, 1.0, floorx, floory+0.5, -1.0);

	float z = 0.0;

	for (map<int, vector< pair<int, char> > >::iterator it = bigMap.begin(); it != bigMap.end(); it++)
	{
		int y = it->first;
		//cout << "Row, col: " << x << " , ";
		vector< pair<int, char> > val = it->second;
		for (vector< pair<int, char> >::iterator itt = val.begin(); itt != val.end(); itt++)
		{
			int x = itt->first;
			char let = itt->second;
			//cout << y << "  making  " << let << endl;
				// Wall  is W
				// Door  is A-E
				// Key   is a-e
				// 0     is open (we don't do anything)
			if (let == 'W')
			{
				addModel(texturedShader, modelList, 2, 0, 0, 1.0, 1.0, 1.0, x+0.5, y+0.5, z);
			}
			else if ( (let >= 'A') && (let <= 'E') ) // if it's between inclusive A and E then it's a Door
			{
				if (activeItem < 0) // if no keys have been picked up, just render all door normally
				{
					addModel(texturedShader, modelList, 1, -1, (let-'A'), 1.0, 1.0, 1.0, x+0.5, y+0.5, z);
				}
				else
				{
					if(doorInfo[let-'A']==true)
					{
						// hide the door if it's been opened (under the floor)
						addModel(texturedShader, modelList, 1, -1, (let-'A'), 1.0, 1.0, 1.0, x+0.5, y+0.5, -1.5);
					}
					else
					{
						addModel(texturedShader, modelList, 1, -1, (let-'A'), 1.0, 1.0, 1.0, x+0.5, y+0.5, z);
					}
				}
			}
			else if ( (let >= 'a') && (let <= 'e') ) // if it's between inclusive a and e then it's a key
			{
				// items.back() is most recent item
				// key[] has booleans for whether a key has been picked up or not

				if (activeItem < 0) // if no keys have been picked up, just render all keys normally
				{
					addModel(texturedShader, modelList, 0, -1, (let-'a'), 1.0, 1.0, 1.0, x+0.5, y+0.5, z);
				}
				else
				{
					//cout << "there is an activeitem" << endl;
					if((let-'a') == activeItem)
					{
						//cout << "key is in front of person" << endl;
						addModel(texturedShader, modelList, 0, -1, (let-'a'), 0.1, 0.1, 0.1, c_pos_x+0.2*cosf(c_theta), c_pos_y+0.2*sinf(c_theta), -.04);
						//c_pos_x + cosf(c_theta), c_pos_y + sinf(c_theta)
						//if the key has been the most recent item "hold" it in front of the camera
					}
					else if(keyInfo[let-'a'] == true)
					{
						// hide the other taken keys (under the floor)
						addModel(texturedShader, modelList, 0, -1, (let-'a'), 1.0, 1.0, 1.0, x+0.5, y+0.5, -1.5);
					}
					// key is not active and has not been taken
					else
					{
						addModel(texturedShader, modelList, 0, -1, (let-'a'), 1.0, 1.0, 1.0, x+0.5, y+0.5, z);
					}
				}
			}
			else if (let == 'G')
			{
				addModel(texturedShader, modelList, 2, 1, 5, 1.0, 1.0, 1.0, x+0.5, y+0.5, z);
			}
			else if (let == 'S')
			{
				continue; // should have already taken care of this
			}
			else
			{
				// we don't care
			}
		}
	}
}

void addModel(int shaderProgram, int verts[], int mod, int tex, int clr, float w, float h, float d, float x, float y, float z)
{
	//determine color
	char clrname = colors[clr];
	float colR=1, colG=0, colB=0;
		// red by default
	if (clrname == 'g')
	{
		colR = 0; colG = 1; colB = 0;
	}
	else if (clrname == 'b')
	{
		colR = 0; colG = 0; colB = 1;
	}
	else if (clrname == 'c')
	{
		colR = 0; colG = 1; colB = 1;
	}
	else if (clrname == 'm')
	{
		colR = 1; colG = 0; colB = 1;
	}
	else if (clrname == 'n')
	{
		colR = 0; colG = 0; colB = 0;
	}

	GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
	glm::vec3 colVec(colR,colG,colB);
	glUniform3fv(uniColor, 1, glm::value_ptr(colVec));

	GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
	glm::mat4 model;

	// Translate
		model = glm::translate(model, glm::vec3(x, y, z));
	// Scale
		model = glm::scale(model, glm::vec3(w, h, d));

	GLint uniModel = glGetUniformLocation(shaderProgram, "model");
	//Set which texture to use (1 = brick texture ... bound to GL_TEXTURE1)
		glUniform1i(uniTexID, tex); 
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
	//Draw an instance of the model (at the position & orientation specified by the model matrix above)

	// verts[0] model 1 start teapot
	// verts[2] model 2 start knot
	// verts[4] model 3 start key
    // so mod*2 is start
		glDrawArrays(GL_TRIANGLES, verts[mod*2], verts[mod*2+1]); //(Primitive Type, Start Vertex, Num Verticies)
}

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile){
	FILE *fp;
	long length;
	char *buffer;

	// open the file containing the text of the shader code
	fp = fopen(shaderFile, "r");

	// check for errors in opening the file
	if (fp == NULL) {
		printf("can't open shader source file %s\n", shaderFile);
		return NULL;
	}

	// determine the file size
	fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
	length = ftell(fp);  // return the value of the current position

	// allocate a buffer with the indicated number of bytes, plus one
	buffer = new char[length + 1];

	// read the appropriate number of bytes from the file
	fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
	fread(buffer, 1, length, fp); // read all of the bytes

	// append a NULL character to indicate the end of the string
	buffer[length] = '\0';

	// close the file
	fclose(fp);

	// return the string
	return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName){
	GLuint vertex_shader, fragment_shader;
	GLchar *vs_text, *fs_text;
	GLuint program;

	// check GLSL version
	printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Create shader handlers
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read source code from shader files
	vs_text = readShaderSource(vShaderFileName);
	fs_text = readShaderSource(fShaderFileName);

	// error check
	if (vs_text == NULL) {
		printf("Failed to read from vertex shader file %s\n", vShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("Vertex Shader:\n=====================\n");
		printf("%s\n", vs_text);
		printf("=====================\n\n");
	}
	if (fs_text == NULL) {
		printf("Failed to read from fragent shader file %s\n", fShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("\nFragment Shader:\n=====================\n");
		printf("%s\n", fs_text);
		printf("=====================\n\n");
	}

	// Load Vertex Shader
	const char *vv = vs_text;
	glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
	glCompileShader(vertex_shader); // Compile shaders
	
	// Check for errors
	GLint  compiled;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		printf("Vertex shader failed to compile:\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}
	
	// Load Fragment Shader
	const char *ff = fs_text;
	glShaderSource(fragment_shader, 1, &ff, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
	
	//Check for Errors
	if (!compiled) {
		printf("Fragment shader failed to compile\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}

	// Create the program
	program = glCreateProgram();

	// Attach shaders to program
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	// Link and set program to use
	glLinkProgram(program);

	return program;
}