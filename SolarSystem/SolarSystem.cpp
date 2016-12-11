// Spring.cpp : Defines the entry point for the console application.
//

#include <windows.h> 
#include <GL/gl.h> 
#include <GL/glut.h> 
#include <stdlib.h>
#include "imageloader.h"
#include <math.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4

#define BLACK_BACKGROUND glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
#define SUN_SIZE 60.0f
#define MERCURY_SIZE 0.24f
#define VENUS_SIZE 0.6f
#define EARTH_SIZE 0.64f
#define MARS_SIZE 0.33f
#define JUPITER_SIZE 7.0f
#define SATURN_SIZE 6.0f
#define URAN_SIZE 2.5f
#define NEPTUN_SIZE 2.4f

#define SUN_BMP_FILE "sun.bmp"
#define MERCURY_BMP_FILE "mercury.bmp"
#define VENUS_BMP_FILE "venus.bmp"
#define EARTH_BMP_FILE "earth.bmp"
#define MARS_BMP_FILE "mars.bmp"
#define JUPITER_BMP_FILE "jupiter.bmp"
#define SATURN_BMP_FILE "saturn.bmp"
#define URAN_BMP_FILE "uran.bmp"
#define NEPTUN_BMP_FILE "neptun.bmp"

#define MERCURY_POSITION glTranslatef(163.0f, 0.0f, 0.0f);//distance from sun
#define VENUS_POSITION glTranslatef(-166.0f, 0.0f, 30.0f);//distance from sun
#define EARTH_POSITION glTranslatef(-169.0f, 0.0f, 120.0f);//distance from sun
#define MARS_POSITION glTranslatef(-174.0f, 0.0f, 150.0f);//distance from sun
#define JUPITER_POSITION glTranslatef(-208.0f, 0.0f, -300.0f);//distance from sun
#define SATURN_POSITION glTranslatef(248.0f, 0.0f, 280.0f);//distance from sun
#define URAN_POSITION glTranslatef(338.0f, 0.0f, 300.0f);//distance from sun
#define NEPTUN_POSITION glTranslatef(-440.0f, 0.0f, 390.0f);//distance from sun


GLUquadric *quad;
GLuint sunTexture;
GLuint mercuryTexture;
GLuint venusTexture;
GLuint earthTexture;
GLuint marsTexture;
GLuint jupiterTexture;
GLuint saturnTexture;
GLuint uranTexture;
GLuint neptunTexture;

//planets vars
float mercuryAngle = 0.0f;
float mercuryAngleItself = 0.0f;
float venusAngle = 0.0f;
float venusAngleItself = 0.0f;
float earthAngle = 0.0f;
float earthAngleItself = 0.0f;
float marsAngle = 0.0f;
float marsAngleItself = 0.0f;
float jupiterAngle = 0.0f;
float jupiterAngleItself = 0.0f;
float saturnAngle = 0.0f;
float saturnAngleItself = 0.0f;
float uranAngle = 0.0f;
float uranAngleItself = 0.0f;
float neptunAngle = 0.0f;
float neptunAngleItself = 0.0f;
float sunAngle = 0.0f;

float speed = 1.0f;
float speed_min = 0.1f;
float speed_max = 10.0f;

//spring vars
GLfloat PI = 3.14159265359f;
GLfloat t_spring = 0.0f;
GLfloat x_spring, y_spring, z_spring;
float spring_speed = 0.01f;
float last_bottom_x_pos, last_bottom_y_pos, last_bottom_z_pos;
float last_top_x_pos, last_top_y_pos, last_top_z_pos;

bool spring_speed_increase = true;

//camera vars
float angleX = 0.0;// angle of rotation for the camera direction
float angleY = 0.0;

float lx = 0.0f, lz = -1.0f, ly = 0.0f;// actual vector representing the camera's direction
float x = 0.0f, z = 100.0f, y = 0.0f;// XZ position of the camera

float deltaAngleX = 0.0f;
float deltaAngleY = 0.0f;

int xOrigin = -1;
int yOrigin = -1;


struct float3
{
	float3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}

	float x;
	float y;
	float z;
};
struct float4
{
	float4(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _v = 0.0f) : x(_x), y(_y), z(_z), v(_v){}

	float x;
	float y;
	float z;
	float v;
};
struct color4
{
	color4(float _r = 0.0f, float _g = 0.0f, float _b = 0.0f, float _t = 0.0f) : r(_r), g(_g), b(_b), t(_t) {}

	float r;
	float g;
	float b;
	float t;
};
struct Light
{
	Light(GLenum lightID = GL_LIGHT0
		, color4 ambient = color4(0.0, 0.0, 0.0, 1.0)
		, color4 diffuse = color4(1.0, 1.0, 1.0, 1.0)
		, color4 specular = color4(1.0, 1.0, 1.0, 1.0)
		, float4 position = float4(0.0, 0.0, 0.0, 0.0)
		, float3 spotDirection = float3(0.0, 0.0, 1.0)
		, float  spotExponent = 0.0
		, float  spotCutoff = 180.0f
		, float  constantAttenuation = 1.0
		, float  linearAttenuation = 0.0
		, float  quadraticAttenuation = 0.0)
		: m_LightID(lightID)
		, m_Ambient(ambient)
		, m_Diffuse(diffuse)
		, m_Specular(specular)
		, m_Position(position)
		, m_SpotDirection(spotDirection)
		, m_SpotExponent(spotExponent)
		, m_SpotCutoff(spotCutoff)
		, m_ConstantAttenuation(constantAttenuation)
		, m_LinearAttenuation(linearAttenuation)
		, m_QuadraticAttenuation(quadraticAttenuation)
	{}

	void Activate()
	{
		glEnable(m_LightID);
		glLightfv(m_LightID, GL_AMBIENT, &(m_Ambient.r));
		glLightfv(m_LightID, GL_DIFFUSE, &(m_Diffuse.r));
		glLightfv(m_LightID, GL_SPECULAR, &(m_Specular.r));
		glLightfv(m_LightID, GL_POSITION, &(m_Position.x));
		glLightfv(m_LightID, GL_SPOT_DIRECTION, &(m_SpotDirection.x));
		glLightf(m_LightID, GL_SPOT_EXPONENT, m_SpotExponent);
		glLightf(m_LightID, GL_SPOT_CUTOFF, m_SpotCutoff);
		glLightf(m_LightID, GL_CONSTANT_ATTENUATION, m_ConstantAttenuation);
		glLightf(m_LightID, GL_LINEAR_ATTENUATION, m_LinearAttenuation);
		glLightf(m_LightID, GL_QUADRATIC_ATTENUATION, m_QuadraticAttenuation);
	}

	void Deactivate()
	{
		glDisable(m_LightID);
	}

	GLenum m_LightID;
	color4 m_Ambient;
	color4 m_Diffuse;
	color4 m_Specular;

	float4 m_Position;
	float3 m_SpotDirection;
	float  m_SpotExponent;
	float  m_SpotCutoff;
	float  m_ConstantAttenuation;
	float  m_LinearAttenuation;
	float  m_QuadraticAttenuation;
};
struct Material
{
	Material(color4 ambient = color4(0.2, 0.2, 0.2, 1.0)
		, color4 diffuse = color4(0.8, 0.8, 0.8, 1.0)
		, color4 specular = color4(0.0, 0.0, 0.0, 1.0)
		, color4 emission = color4(0.0, 0.0, 0.0, 1.0)
		, float shininess = 0)
		: m_Ambient(ambient)
		, m_Diffuse(diffuse)
		, m_Specular(specular)
		, m_Emission(emission)
		, m_Shininess(shininess)
	{}

	void Apply()
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &(m_Ambient.r));
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &(m_Diffuse.r));
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &(m_Specular.r));
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &(m_Emission.r));
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_Shininess);
	}

	color4 m_Ambient;
	color4 m_Diffuse;
	color4 m_Specular;
	color4 m_Emission;
	float  m_Shininess;

};
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

	float fraction = 2.0f;

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
		y += ly * fraction;
		break;
	case GLUT_KEY_DOWN:
		x -= lx * fraction;
		z -= lz * fraction;
		y -= ly * fraction;
		break;
	case GLUT_KEY_PAGE_UP:
		y += fraction;
		break;
	case GLUT_KEY_PAGE_DOWN:
		y -= fraction;
		break;
	}
}
void processKeyboardKeys(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':
		if (speed < speed_max)
			speed += 0.1f;
		break;
	case 's':
		if (speed > speed_min)
			speed -= 0.1f;
		break;
	default:
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

	Image *image = loadBMP(SUN_BMP_FILE);
	sunTexture = loadTexture(image);

	Image *image2 = loadBMP(EARTH_BMP_FILE);
	earthTexture = loadTexture(image2);

	Image *image3 = loadBMP(MERCURY_BMP_FILE);
	mercuryTexture = loadTexture(image3);

	Image *image4 = loadBMP(VENUS_BMP_FILE);
	venusTexture = loadTexture(image4);

	Image *image5 = loadBMP(MARS_BMP_FILE);
	marsTexture = loadTexture(image5);

	Image *image6 = loadBMP(JUPITER_BMP_FILE);
	jupiterTexture = loadTexture(image6);

	Image *image7 = loadBMP(SATURN_BMP_FILE);
	saturnTexture = loadTexture(image7);

	Image *image8 = loadBMP(URAN_BMP_FILE);
	uranTexture = loadTexture(image8);

	Image *image9 = loadBMP(NEPTUN_BMP_FILE);
	neptunTexture = loadTexture(image9);

	quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluQuadricTexture(quad, GL_TRUE);

	// Specify a global ambient
	GLfloat globalAmbient[] = { 0.2, 0.2, 0.2, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	glEnable(GL_LIGHTING);


	
}
void setTexture(GLuint texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
}

void moves() {
	mercuryAngle += (0.05 * speed);
	mercuryAngleItself += (1.0f * speed);
	venusAngle += (0.05 * speed);
	venusAngleItself += (1.0f * speed);
	earthAngle += (0.05 * speed);
	earthAngleItself += (1.0f * speed);
	marsAngle += (0.05 * speed);
	marsAngleItself += (1.0f * speed);
	jupiterAngle += (0.05 * speed);
	jupiterAngleItself += (1.0f * speed);
	saturnAngle += (0.05 * speed);
	saturnAngleItself += (1.0f * speed);
	uranAngle += (0.05 * speed);
	uranAngleItself += (1.0f * speed);
	neptunAngle += (0.05 * speed);
	neptunAngleItself += (1.0f * speed);
	sunAngle += (0.2 * speed);
}
void drawSun() {

	setTexture(sunTexture);
	Light g_SunLight(GL_LIGHT0, color4(0, 0, 0, 1), color4(1, 1, 1, 1), color4(1, 1, 1, 1), float4(0, 0, 0, 1));
	// Material properties
	Material g_SunMaterial(color4(0, 0, 0, 1), color4(1, 1, 1, 1), color4(1, 1, 1, 1));

	glPushMatrix();
		g_SunLight.Activate();
		glDisable(GL_LIGHTING);
		glRotatef(sunAngle, 0, 1, 0);//rotate around itself
		g_SunMaterial.Apply();
		gluSphere(quad, SUN_SIZE, 40, 40);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	return;

}
void drawMercury() {

	setTexture(mercuryTexture);
	Material g_EarthMaterial(color4(0.2, 0.2, 0.2, 1.0), color4(1, 1, 1, 1), color4(1, 1, 1, 1), color4(0, 0, 0, 1), 50);

	glPushMatrix();
		glRotatef(mercuryAngle, 0, 1, 0);//rotate around sun	
		MERCURY_POSITION
		glPushMatrix();
			glRotatef(mercuryAngleItself, 0, 1, 0);//rotate around itself
			g_EarthMaterial.Apply();
			gluSphere(quad, MERCURY_SIZE, 360, 180);
		glPopMatrix();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	return;

}
void drawVenus() {

	setTexture(venusTexture);
	Material g_EarthMaterial(color4(0.2, 0.2, 0.2, 1.0), color4(1, 1, 1, 1), color4(1, 1, 1, 1), color4(0, 0, 0, 1), 50);

	glPushMatrix();
		glRotatef(venusAngle, 0, 1, 0);//rotate around sun	
		VENUS_POSITION
		glPushMatrix();
			glRotatef(venusAngleItself, 0, 1, 0);//rotate around itself
			glRotatef(-90, 1, 0, 0);//rotate 90 deg!
			g_EarthMaterial.Apply();
			gluSphere(quad, VENUS_SIZE, 360, 180);
		glPopMatrix();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	return;

}
void drawEarth() {

	setTexture(earthTexture);
	Material g_EarthMaterial(color4(0.2, 0.2, 0.2, 1.0), color4(1, 1, 1, 1), color4(1, 1, 1, 1), color4(0, 0, 0, 1), 50);

	glPushMatrix();
		glRotatef(earthAngle, 0, 1, 0);//rotate around sun	
		EARTH_POSITION
		glPushMatrix();
			glRotatef(earthAngleItself, 0, 1, 0);//rotate around itself
			glRotatef(-90, 1, 0, 0);//rotate 90 deg!
			g_EarthMaterial.Apply();
			gluSphere(quad, EARTH_SIZE, 360, 180);
		glPopMatrix();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	return;

}
void drawMars() {

	setTexture(marsTexture);
	Material g_EarthMaterial(color4(0.2, 0.2, 0.2, 1.0), color4(1, 1, 1, 1), color4(1, 1, 1, 1), color4(0, 0, 0, 1), 50);

	glPushMatrix();
		glRotatef(marsAngle, 0, 1, 0);//rotate around sun	
		MARS_POSITION
		glPushMatrix();
			glRotatef(marsAngleItself, 0, 1, 0);//rotate around itself
			glRotatef(-90, 1, 0, 0);//rotate 90 deg!
			g_EarthMaterial.Apply();
			gluSphere(quad, MARS_SIZE, 360, 180);
		glPopMatrix();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	return;

}
void drawJupiter() {

	setTexture(jupiterTexture);
	Material g_EarthMaterial(color4(0.2, 0.2, 0.2, 1.0), color4(1, 1, 1, 1), color4(1, 1, 1, 1), color4(0, 0, 0, 1), 50);

	glPushMatrix();
		glRotatef(jupiterAngle, 0, 1, 0);//rotate around sun	
		JUPITER_POSITION
		glPushMatrix();
			glRotatef(jupiterAngleItself, 0, 1, 0);//rotate around itself
			glRotatef(-90, 1, 0, 0);//rotate 90 deg!
			g_EarthMaterial.Apply();
			gluSphere(quad, JUPITER_SIZE, 360, 180);
		glPopMatrix();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	return;

}
void drawSaturn() {
	
	setTexture(saturnTexture);
	Material g_EarthMaterial(color4(0.2, 0.2, 0.2, 1.0), color4(1, 1, 1, 1), color4(1, 1, 1, 1), color4(0, 0, 0, 1), 50);

	glPushMatrix();
		glRotatef(saturnAngle, 0, 1, 0);//rotate around sun	
		SATURN_POSITION
		glPushMatrix();
			glRotatef(saturnAngleItself, 0, 1, 0);//rotate around itself
			glRotatef(-90, 1, 0, 0);//rotate 90 deg!
			g_EarthMaterial.Apply();
			gluSphere(quad, SATURN_SIZE, 360, 180);
		glPopMatrix();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	return;

}
void drawUran() {

	setTexture(uranTexture);
	Material g_EarthMaterial(color4(0.2, 0.2, 0.2, 1.0), color4(1, 1, 1, 1), color4(1, 1, 1, 1), color4(0, 0, 0, 1), 50);

	glPushMatrix();
		glRotatef(uranAngle, 0, 1, 0);//rotate around sun	
		URAN_POSITION
		glPushMatrix();
			glRotatef(uranAngleItself, 0, 1, 0);//rotate around itself
			glRotatef(-90, 1, 0, 0);//rotate 90 deg!
			g_EarthMaterial.Apply();
			gluSphere(quad, URAN_SIZE, 360, 180);
		glPopMatrix();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	return;

}
void drawNeptun() {

	setTexture(neptunTexture);
	Material g_EarthMaterial(color4(0.2, 0.2, 0.2, 1.0), color4(1, 1, 1, 1), color4(1, 1, 1, 1), color4(0, 0, 0, 1), 50);

	glPushMatrix();
		glRotatef(neptunAngle, 0, 1, 0);//rotate around sun	
		NEPTUN_POSITION
		glPushMatrix();
			glRotatef(neptunAngleItself, 0, 1, 0);//rotate around itself
			glRotatef(-90, 1, 0, 0);//rotate 90 deg!
			g_EarthMaterial.Apply();
			gluSphere(quad, NEPTUN_SIZE, 360, 180);
		glPopMatrix();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	return;

}



void reshape(int x, int y)
{
	if (y == 0 || x == 0) return;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (GLdouble)x / (GLdouble)y, 0.01, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, x, y);  //Use the whole window for rendering
}
void display(void)
{
	glEnable(GL_TEXTURE_2D);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // Clear the color buffer with current clearing color
	glMatrixMode(GL_MODELVIEW);      // To operate on Model-View matrix
	glLoadIdentity();                // Reset the model-view matrix

	// Reset transformations
	glLoadIdentity();

	// Set the camera
	gluLookAt(x, y, z,
		x + lx, y + ly, z + lz,
		0.0f, 1.0f, 0.0f);

	moves();
	drawSun();
	drawMercury();
	drawVenus();
	drawEarth();
	drawMars();
	drawJupiter();
	drawSaturn();
	drawUran();
	drawNeptun();

	glutPostRedisplay(); // Redraw screen with new mouse data.
						 /* this redraws the scene without
						 waiting for the display callback so that any changes appear
						 instantly */
	glutSwapBuffers();

	glFlush();// Flush buffers to screen

}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Spring");

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_SINGLE);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(processKeyboardKeys);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	initGL();                       // Our own OpenGL initialization

	glutMainLoop();
	return 0;
}

