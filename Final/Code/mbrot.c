#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include "util.h"

void draw(void);
void idle_handler(void);
void key_handler(unsigned char key, int x, int y);
void bn_handler(int bn, int state, int x, int y);
void mouse_handler(int x, int y);

unsigned int prog;
double cx = 0.7, cy = 0.0;
double scale = 2.2;
int iter = 70;
//const double zoom_factor = 0.001;
const double zoom_factor = 0.008;

int main(int argc, char **argv) {
	void *img;
	
	/* initialize glut */
	glutInitWindowSize(800, 600);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Mandelbrot Set");

	glutDisplayFunc(draw);
	glutIdleFunc(idle_handler);
	glutKeyboardFunc(key_handler);
	glutMouseFunc(bn_handler);
	glutMotionFunc(mouse_handler);

	/* load the 1D palette texture */
	glBindTexture(GL_TEXTURE_1D, 1);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	
	if(!(img = load_image("pal.ppm", 0, 0))) {
		return EXIT_FAILURE;
	}
	glTexImage1D(GL_TEXTURE_1D, 0, 4, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, img);
	free(img);

	glEnable(GL_TEXTURE_1D);

	/* load and set the mandelbrot shader */
	if(!(prog = setup_shader("mbrot.glsl"))) {
		return EXIT_FAILURE;
	}
	set_uniform1i(prog, "iter", iter);

	glutMainLoop();
	return 0;
}

void draw(void) {
	set_uniform2f(prog, "center", cx, cy);
	set_uniform1f(prog, "scale", scale);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glEnd();

	glutSwapBuffers();
}

void idle_handler(void) {
	glutPostRedisplay();
}

void key_handler(unsigned char key, int x, int y) {
	switch(key) {
	case 27:
	case 'q':
	case 'Q':
		exit(0);

	case '=':
		if(1) {
			iter += 10;
		} else {
	case '-':
			iter -= 10;
			if(iter < 0) iter = 0;
		}
		printf("iterations: %d\n", iter);
		set_uniform1i(prog, "iter", iter);
		break;
	case 'f':
		scale *= (double)(1 - zoom_factor * 2.0);
		printf("scale: %20.18f\n", scale);
		break;
	case 'd':
		scale *= (double)(1 + zoom_factor * 2.0);
	default:
		break;
	}
}

int which_bn;
//int which_bn2;
double px, py;

void bn_handler(int bn, int state, int x, int y) {
	
	int xres = glutGet(GLUT_WINDOW_WIDTH);
	int yres = glutGet(GLUT_WINDOW_HEIGHT);
	px = 2.0 * ((double)x / (double)xres - 0.5);
	py = 2.0 * ((double)y / (double)yres - 0.5);
	//which_bn = 1;
	which_bn = bn;


	if(which_bn == 3)
	{
		printf("UP\n");
		scale *= 1 - zoom_factor * 2.0;
	}
	else if(which_bn == 4)
	{
		printf("DOWN\n");
		scale *= 1 + zoom_factor * 2.0;;
	}
	else if(which_bn == 1)
	{
		//which_bn = 1;
	}
	
}

void mouse_handler(int x, int y) {
	int xres = glutGet(GLUT_WINDOW_WIDTH);
	int yres = glutGet(GLUT_WINDOW_HEIGHT);
	double fx = 2.0 * ((double)x / (double)xres - 0.5);
	double fy = 2.0 * ((double)y / (double)yres - 0.5);

	cx += (fx - px) * scale / 2.0;
	cy -= (fy - py) * scale / 2.0;

/*
	if(which_bn == 1) {
		cx += (fx - px) * scale / 2.0;
		cy -= (fy - py) * scale / 2.0;
	} else if(which_bn == 0) {
		//scale *= (fy - py < 0.0) ? 1 - zoom_factor : 1 + zoom_factor;
	}
*/


	px = fx;
	py = fy;
}
