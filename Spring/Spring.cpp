// Spring.cpp : Defines the entry point for the console application.
//

#include <windows.h> 
#include <GL/gl.h> 
#include <GL/glut.h> 
#include <stdlib.h>
#include "imageloader.h"
#include <math.h>

#define BLACK_BACKGROUND glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
#define BALL_SPRING_SIZE 0.5f
#define BALL_SIZE 5.0f
#define CAMERA_START_POSITION glTranslatef(0, 0, -20.0f);
#define SPRING_LENGTH_MAX 0.7f
#define SPRING_LENGTH_MIN 0.3f
#define NUM_OF_BALLS_IN_SPHERE 50.0f
#define NUM_OF_BALLS_IN_ROD_1 30.0F
#define NUM_OF_BALLS_IN_ROD_2 40.0F
#define STEEL_BMP_FILE "steel2.bmp"
#define WOOD_BMP_FILE "wood2.bmp"
#define QUBOID_WIDTH 5.0f


GLUquadric *quad;
GLuint steelTexture;
GLuint woodTexture;


//spring vars
GLfloat PI = 3.14159265359f;
GLfloat t_spring = 0.0f;
GLfloat x_spring, y_spring, z_spring;
float spring_current_length = SPRING_LENGTH_MAX;
float spring_speed = 0.01f;
float last_bottom_x_pos, last_bottom_y_pos, last_bottom_z_pos;
float last_top_x_pos, last_top_y_pos, last_top_z_pos;

bool spring_speed_increase = true;

//camera vars
float angle = 0.0;// angle of rotation for the camera direction
float lx = 0.0f, lz = -1.0f, ly = 0.0f;// actual vector representing the camera's direction
float x = 0.0f, z = 20.0f, y = 0.0f;// XZ position of the camera


GLuint loadTexture(Image* image) {

	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
											 //Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0,                            //The border of the image
		GL_RGB, GL_UNSIGNED_BYTE, image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture
}
void processSpecialKeys(int key, int xx, int yy) {

	float fraction = 0.5f;

	switch (key) {
	case GLUT_KEY_LEFT:
		angle -= 0.1f;
		lx = sin(angle);
		lz = -cos(angle);
		break;
	case GLUT_KEY_RIGHT:
		angle += 0.1f;
		lx = sin(angle);
		lz = -cos(angle);
		break;
	case GLUT_KEY_UP:
		x += lx * fraction;
		z += lz * fraction;
		break;
	case GLUT_KEY_DOWN:
		x -= lx * fraction;
		z -= lz * fraction;
		break;
	case GLUT_KEY_PAGE_UP:
		y += fraction;
		break;
	case GLUT_KEY_PAGE_DOWN:
		y -= fraction;
		break;
	}
}

void initGL(void) {

	Image *image = loadBMP(STEEL_BMP_FILE);
	steelTexture = loadTexture(image);
	Image *image2 = loadBMP(WOOD_BMP_FILE);
	woodTexture = loadTexture(image2);


	BLACK_BACKGROUND
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

}
void setTexture(GLUquadric* quad, GLuint texture) {
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluQuadricTexture(quad, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void moveSpring() {
	if (spring_current_length < SPRING_LENGTH_MIN || spring_current_length > SPRING_LENGTH_MAX)
		spring_speed_increase = !spring_speed_increase;

	if (spring_speed_increase)
		spring_current_length += spring_speed;
	else spring_current_length -= spring_speed;
}
void drawSpringWithRods() {

	int i;

	//draw spring
	for (t_spring = 0.0f; t_spring <= 8 * PI; t_spring += (PI / NUM_OF_BALLS_IN_SPHERE)){

		x_spring = cos(t_spring) * (3 + cos(0));
		z_spring = sin(t_spring) * (3 + cos(0));
		y_spring = spring_current_length * t_spring + sin(0);

		glPushMatrix();
			glRotatef(180.0f, 0.0f, 0.0f, 1.0f); // rotate around OZ
			glTranslatef(x_spring, y_spring, z_spring);
			gluSphere(quad, BALL_SPRING_SIZE, 20, 20);
		glPopMatrix();
	}

	last_bottom_x_pos = x_spring;
	last_bottom_y_pos = y_spring;
	last_bottom_z_pos = z_spring;

	//draw bottom rods
	for (i = 0; i < NUM_OF_BALLS_IN_ROD_1; i++) {
		glPushMatrix();
			glRotatef(180.0f, 0.0f, 0.0f, 1.0f); // rotate around OZ
			glTranslatef(last_bottom_x_pos, last_bottom_y_pos, last_bottom_z_pos);
			gluSphere(quad, BALL_SPRING_SIZE, 20, 20);
		glPopMatrix();
		last_bottom_x_pos -= (BALL_SPRING_SIZE / 4);
	}
	for (i = 0; i < NUM_OF_BALLS_IN_ROD_2; i++) {
		glPushMatrix();
			glRotatef(180.0f, 0.0f, 0.0f, 1.0f); // rotate around OZ
			glTranslatef(last_bottom_x_pos, last_bottom_y_pos, last_bottom_z_pos);
			gluSphere(quad, BALL_SPRING_SIZE, 20, 20);
		glPopMatrix();
		last_bottom_y_pos += (BALL_SPRING_SIZE / 4);
	}

	last_top_x_pos = 4.0f;
	last_top_y_pos = 0.0f;
	last_top_z_pos = 0.0f;

	//draw top rods
	for (i = 0; i < NUM_OF_BALLS_IN_ROD_1; i++) {
		glPushMatrix();
		glRotatef(180.0f, 0.0f, 0.0f, 1.0f); // rotate around OZ
		glTranslatef(last_top_x_pos, last_top_y_pos, last_top_z_pos);
		gluSphere(quad, BALL_SPRING_SIZE, 20, 20);
		glPopMatrix();
		last_top_x_pos -= (BALL_SPRING_SIZE / 4);
	}
	for (i = 0; i < NUM_OF_BALLS_IN_ROD_2; i++) {
		glPushMatrix();
		glRotatef(180.0f, 0.0f, 0.0f, 1.0f); // rotate around OZ
		glTranslatef(last_top_x_pos, last_top_y_pos, last_top_z_pos);
		gluSphere(quad, BALL_SPRING_SIZE, 20, 20);
		glPopMatrix();
		last_top_y_pos -= (BALL_SPRING_SIZE / 4);
	}

}
void drawBall() {
	last_bottom_y_pos += BALL_SIZE ;

	setTexture(quad, woodTexture);

	glPushMatrix();
		glRotatef(180.0f, 0.0f, 0.0f, 1.0f); // rotate around OZ
		glTranslatef(last_bottom_x_pos, last_bottom_y_pos, last_bottom_z_pos);
		gluSphere(quad, BALL_SIZE, 20, 20);
	glPopMatrix();
}
void drawCuboid() {

	last_top_x_pos *= 100;

	setTexture(quad, woodTexture);

	glPushMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f); // rotate around OZ

		glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
									  // Top face (y = 1.0f)
									  // Define vertices in counter-clockwise (CCW) order with normal pointing out
		//glColor3f(0.0f, 1.0f, 0.0f);     // Green
		glVertex3f(last_top_x_pos, last_top_y_pos, QUBOID_WIDTH);
		glVertex3f(-last_top_x_pos, last_top_y_pos, QUBOID_WIDTH);
		glVertex3f(-last_top_x_pos, last_top_y_pos, -QUBOID_WIDTH);
		glVertex3f(last_top_x_pos, last_top_y_pos, -QUBOID_WIDTH);

		// Bottom face (y = -1.0f)
		//glColor3f(1.0f, 0.5f, 0.0f);     // Orange
		glVertex3f(last_top_x_pos, 2 * last_top_y_pos, -QUBOID_WIDTH);
		glVertex3f(-last_top_x_pos, 2 * last_top_y_pos, -QUBOID_WIDTH);
		glVertex3f(-last_top_x_pos, 2 * last_top_y_pos, QUBOID_WIDTH);
		glVertex3f(last_top_x_pos, 2 * last_top_y_pos, QUBOID_WIDTH);

		// Front face  (z = 1.0f)
		//glColor3f(1.0f, 0.0f, 0.0f);     // Red
		glVertex3f(last_top_x_pos, 2 * last_top_y_pos, -QUBOID_WIDTH);
		glVertex3f(-last_top_x_pos, 2 * last_top_y_pos, -QUBOID_WIDTH);
		glVertex3f(-last_top_x_pos, last_top_y_pos, -QUBOID_WIDTH);
		glVertex3f(last_top_x_pos, last_top_y_pos, -QUBOID_WIDTH);

		// Back face (z = -1.0f)
		//glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
		glVertex3f(last_top_x_pos, last_top_y_pos, QUBOID_WIDTH);
		glVertex3f(-last_top_x_pos, last_top_y_pos, QUBOID_WIDTH);
		glVertex3f(-last_top_x_pos, 2 * last_top_y_pos, QUBOID_WIDTH);
		glVertex3f(last_top_x_pos, 2 * last_top_y_pos, QUBOID_WIDTH);

		// Left face (x = -1.0f)
		//glColor3f(0.0f, 0.0f, 1.0f);     // Blue
		glVertex3f(-last_top_x_pos, 2 * last_top_y_pos, -QUBOID_WIDTH);
		glVertex3f(-last_top_x_pos, 2 * last_top_y_pos, QUBOID_WIDTH);
		glVertex3f(-last_top_x_pos, last_top_y_pos, QUBOID_WIDTH);
		glVertex3f(-last_top_x_pos, last_top_y_pos, -QUBOID_WIDTH);

		// Right face (x = 1.0f)
		//glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
		glVertex3f(last_top_x_pos, 2 * last_top_y_pos, QUBOID_WIDTH);
		glVertex3f(last_top_x_pos, 2 * last_top_y_pos, -QUBOID_WIDTH);
		glVertex3f(last_top_x_pos, last_top_y_pos, -QUBOID_WIDTH);
		glVertex3f(last_top_x_pos, last_top_y_pos, QUBOID_WIDTH);
		glEnd();  // End of drawing color-cube
	glPopMatrix();



}

void reshape(int x, int y)
{
	if (y == 0 || x == 0) return;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, (GLdouble)x / (GLdouble)y, 0.6, 50.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, x, y);  //Use the whole window for rendering
}
void display(void)
{
	glEnable(GL_TEXTURE_2D);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // Clear the color buffer with current clearing color
	glMatrixMode(GL_MODELVIEW);      // To operate on Model-View matrix
	glLoadIdentity();                // Reset the model-view matrix


	//CAMERA_START_POSITION

	quad = gluNewQuadric();
	setTexture(quad, steelTexture);

	// Reset transformations
	glLoadIdentity();

	// Set the camera
	gluLookAt(x, y, z,
		x + lx, y + ly, z + lz,
		0.0f, 1.0f, 0.0f);

	moveSpring();
	drawSpringWithRods();
	drawBall();
	drawCuboid();

	glutPostRedisplay(); // Redraw screen with new mouse data.
	glutSwapBuffers();

	glFlush();// Flush buffers to screen

}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(50, 50);
	glutCreateWindow("Spring");

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(processSpecialKeys);
	initGL();                       // Our own OpenGL initialization

	glutMainLoop();
	return 0;
}

