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
#define SUN_SIZE 5.0f
#define EARTH_SIZE 3.0f
#define SUN_BMP_FILE "sun.bmp"
#define EARTH_BMP_FILE "earth.bmp"


GLUquadric *quad;
GLuint sunTexture;
GLuint earthTexture;

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
float x = 0.0f, z = 40.0f, y = 0.0f;// XZ position of the camera

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
		, float4 position = float4(0.0, 0.0, 1.0, 0.0)
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

	float fraction = 0.2f;

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

	quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluQuadricTexture(quad, GL_TRUE);

	// Specify a global ambient
	GLfloat globalAmbient[] = { 0.2, 0.2, 0.2, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	glEnable(GL_LIGHTING);

	// Material properties
	Material g_SunMaterial(color4(0, 0, 0, 1), color4(1, 1, 1, 1), color4(1, 1, 1, 1));
	
}
void setTexture(GLuint texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
}

void moveSpring() {
	//if (spring_current_length < SPRING_LENGTH_MIN || spring_current_length > SPRING_LENGTH_MAX)
	//	spring_speed_increase = !spring_speed_increase;

	//if (spring_speed_increase)
	//	spring_current_length += spring_speed;
	//else spring_current_length -= spring_speed;
}
void drawSun() {

	setTexture(sunTexture);
	Light g_SunLight(GL_LIGHT0, color4(0, 0, 0, 1), color4(1, 1, 1, 1), color4(1, 1, 1, 1), float4(0, 0, 0, 1));

	glPushMatrix();
	g_SunLight.Activate();
	glDisable(GL_LIGHTING);
	//glRotatef(180.0f, 0.0f, 0.0f, 1.0f); // rotate around OZ
	//glTranslatef(x_spring, y_spring, z_spring);
	gluSphere(quad, SUN_SIZE, 40, 40);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	return;

}
void drawEarth() {

	setTexture(earthTexture);
	Material g_EarthMaterial(color4(0.2, 0.2, 0.2, 1.0), color4(1, 1, 1, 1), color4(1, 1, 1, 1), color4(0, 0, 0, 1), 50);

	glPushMatrix();
	//glRotatef(180.0f, 0.0f, 0.0f, 1.0f); // rotate around OZ
	glTranslatef(-15.0f, 0, 0);
	g_EarthMaterial.Apply();
	gluSphere(quad, EARTH_SIZE, 360, 180);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	return;

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



	// Reset transformations
	glLoadIdentity();

	// Set the camera
	gluLookAt(x, y, z,
		x + lx, y + ly, z + lz,
		0.0f, 1.0f, 0.0f);

	//moveSpring();
	drawSun();
	drawEarth();


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

