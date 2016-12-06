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
#define SPRING_LENGTH_MAX 0.7f
#define SPRING_LENGTH_MIN 0.3f
#define NUM_OF_BALLS_IN_SPHERE 50.0f
#define NUM_OF_BALLS_IN_ROD_1 30.0F
#define NUM_OF_BALLS_IN_ROD_2 40.0F
#define STEEL_BMP_FILE "steel2.bmp"
#define WOOD_BMP_FILE "wood3.bmp"
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
float angleX = 0.0;// angle of rotation for the camera direction
float angleY = 0.0;

float lx = 0.0f, lz = -1.0f, ly = 0.0f;// actual vector representing the camera's direction
float x = 0.0f, z = 40.0f, y = 0.0f;// XZ position of the camera

float deltaAngleX = 0.0f;
float deltaAngleY = 0.0f;

int xOrigin = -1;
int yOrigin = -1;


GLuint loadTexture(Image* image) {

	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
											 //Map the image to the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0,                            //The border of the image
		GL_RGB, GL_UNSIGNED_BYTE, image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture
}
void processSpecialKeys(int key, int xx, int yy) {

	float fraction = 0.5f;

	switch (key) {
	//case GLUT_KEY_LEFT:
	//	angleX -= 0.1f;
	//	lx = sin(angleX);
	//	lz = -cos(angleX);
	//	break;
	//case GLUT_KEY_RIGHT:
	//	angleX += 0.1f;
	//	lx = sin(angleX);
	//	lz = -cos(angleX);
	//	break;
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
void mouseButton(int button, int state, int x, int y) {

	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			angleX -= deltaAngleX;
			xOrigin = -1;
			angleY -= deltaAngleY;
			yOrigin = -1;
		}
		else {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
		}
	}
}
void mouseMove(int x, int y) {

	// this will only be true when the left button is down
	if (xOrigin >= 0) {
		// update deltaAngle
		deltaAngleX = (x - xOrigin) * 0.01f;

		// update camera's direction
		lx = sin(angleX - deltaAngleX);
		lz = -cos(angleX - deltaAngleX);
	}
	if (yOrigin >= 0) {
		deltaAngleY = (y - yOrigin) * 0.01f;

		ly = tan(angleY - deltaAngleY);

		//lz = -cos(angleY - deltaAngleY);

	}
}
void initGL(void) {

	BLACK_BACKGROUND
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	Image *image = loadBMP(STEEL_BMP_FILE);
	steelTexture = loadTexture(image);
	Image *image2 = loadBMP(WOOD_BMP_FILE);
	woodTexture = loadTexture(image2);

	quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluQuadricTexture(quad, GL_TRUE);

}
void setTexture(GLuint texture) {

	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
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
	glDisable(GL_TEXTURE_2D);
}
void drawBall() {
	last_bottom_y_pos += BALL_SIZE ;

	setTexture(woodTexture);
	glPushMatrix();
		glRotatef(180.0f, 0.0f, 0.0f, 1.0f); // rotate around OZ
		glTranslatef(last_bottom_x_pos, last_bottom_y_pos, last_bottom_z_pos);
		gluSphere(quad, BALL_SIZE, 20, 20);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

}
void drawCuboid() {

	last_top_x_pos *= 100;
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f); // rotate around OZ
	setTexture(woodTexture);
	glBegin(GL_QUADS);               

		//Top face
		glTexCoord2f(0, 0); glVertex3f(last_top_x_pos, last_top_y_pos, QUBOID_WIDTH);
		glTexCoord2f(0, 1); glVertex3f(-last_top_x_pos, last_top_y_pos, QUBOID_WIDTH);
		glTexCoord2f(1, 0); glVertex3f(-last_top_x_pos, last_top_y_pos, -QUBOID_WIDTH);
		glTexCoord2f(1, 1); glVertex3f(last_top_x_pos, last_top_y_pos, -QUBOID_WIDTH);

		// Bottom face
		glTexCoord2f(0, 0); glVertex3f(last_top_x_pos, 2 * last_top_y_pos, -QUBOID_WIDTH);
		glTexCoord2f(0, 1); glVertex3f(-last_top_x_pos, 2 * last_top_y_pos, -QUBOID_WIDTH);
		glTexCoord2f(1, 0); glVertex3f(-last_top_x_pos, 2 * last_top_y_pos, QUBOID_WIDTH);
		glTexCoord2f(1, 1); glVertex3f(last_top_x_pos, 2 * last_top_y_pos, QUBOID_WIDTH);

		// Front face
		glTexCoord2f(0, 0); glVertex3f(last_top_x_pos, 2 * last_top_y_pos, -QUBOID_WIDTH);
		glTexCoord2f(0, 1); glVertex3f(-last_top_x_pos, 2 * last_top_y_pos, -QUBOID_WIDTH);
		glTexCoord2f(1, 0); glVertex3f(-last_top_x_pos, last_top_y_pos, -QUBOID_WIDTH);
		glTexCoord2f(1, 1); glVertex3f(last_top_x_pos, last_top_y_pos, -QUBOID_WIDTH);

		// Back face
		glTexCoord2f(0, 0); glVertex3f(last_top_x_pos, last_top_y_pos, QUBOID_WIDTH);
		glTexCoord2f(0, 1); glVertex3f(-last_top_x_pos, last_top_y_pos, QUBOID_WIDTH);
		glTexCoord2f(1, 0); glVertex3f(-last_top_x_pos, 2 * last_top_y_pos, QUBOID_WIDTH);
		glTexCoord2f(1, 1); glVertex3f(last_top_x_pos, 2 * last_top_y_pos, QUBOID_WIDTH);

		// Left face
		glTexCoord2f(0, 0); glVertex3f(-last_top_x_pos, 2 * last_top_y_pos, -QUBOID_WIDTH);
		glTexCoord2f(0, 1); glVertex3f(-last_top_x_pos, 2 * last_top_y_pos, QUBOID_WIDTH);
		glTexCoord2f(1, 0); glVertex3f(-last_top_x_pos, last_top_y_pos, QUBOID_WIDTH);
		glTexCoord2f(1, 1); glVertex3f(-last_top_x_pos, last_top_y_pos, -QUBOID_WIDTH);

		// Right face
		glTexCoord2f(0, 0); glVertex3f(last_top_x_pos, 2 * last_top_y_pos, QUBOID_WIDTH);
		glTexCoord2f(0, 1); glVertex3f(last_top_x_pos, 2 * last_top_y_pos, -QUBOID_WIDTH);
		glTexCoord2f(1, 0); glVertex3f(last_top_x_pos, last_top_y_pos, -QUBOID_WIDTH);
		glTexCoord2f(1, 1); glVertex3f(last_top_x_pos, last_top_y_pos, QUBOID_WIDTH);

		glEnd();  // End of drawing color-cube
		glFlush();
		glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void reshape(int x, int y)
{
	if (y == 0 || x == 0) return;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (GLdouble)x / (GLdouble)y, 0.01, 50.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, x, y);  //Use the whole window for rendering
}
void display(void)
{
	glEnable(GL_TEXTURE_2D);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // Clear the color buffer with current clearing color
	glMatrixMode(GL_MODELVIEW);      // To operate on Model-View matrix
	glLoadIdentity();                // Reset the model-view matrix


	setTexture(steelTexture);

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
	glutInitWindowSize(100, 100);
	glutCreateWindow("Spring");

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_SINGLE);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	initGL();                       // Our own OpenGL initialization

	glutMainLoop();
	return 0;
}

