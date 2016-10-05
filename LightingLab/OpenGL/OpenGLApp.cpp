#include<Windows.h>
#include<gl\GL.h>
#include<gl\GLU.h>
#include <cmath>
#include "OpenGLApp.h"

//declare some global constants
const float CUBE_DEG_PER_S = 5.0f;
const float LIGHT_DEG_PER_S = 15.0f;

const float CUBE_SIZE = 9.0f;



const GLfloat flashLightPos[] = { 0.0, 0.0, 0.0, 1.0 };
const GLfloat flashLightDir[] = { 0.0, 0.0, -1.0 };
const GLfloat flashLightColor[] = { 0.2, 0.2, 0.2, 1.0 };

const GLfloat redLightColor[] = { 0.5, 0.1, 0.2, 1.0 };
const GLfloat redLightPos[] = { 10.0, 0.0, 5.0, 1.0 };

const GLfloat greenLightColor[] = { 0.1, 0.6, 0.2, 1.0 };
const GLfloat greenLightPos[] = { 0.0, 0.0, 10.0, 1.0 };

// draw one side of the cube
void DrawPlane(float size, float step)
{
	glBegin(GL_QUADS);
	for (float z = 0.0; z < size; z += step)
	{
		for (float x = 0.0; x < size; x += step)
		{
			glVertex3f(x, 0.0, z);
			glVertex3f(x + step, 0.0, z);
			glVertex3f(x + step, 0.0, z + step);
			glVertex3f(x, 0.0, z + step);
		}
	}
	glEnd();
}

// draw the entire cube
void DrawCube(float size, int resolution)
{
	float step = size / resolution;

	glPushMatrix();
	glTranslatef(-size / 2, -size / 2, -size / 2);

	glNormal3f(0.0, -1.0, 0.0);

	// top
	glPushMatrix();
	glTranslatef(0.0, size, 0.0);
	glScalef(1.0, -1.0, 1.0);
	DrawPlane(size, step);
	glPopMatrix();

	// bottom
	DrawPlane(size, step);

	// left
	glPushMatrix();
	glRotatef(90.0, 0.0, 0.0, 1.0);
	glScalef(1.0, -1.0, 1.0);
	DrawPlane(size, step);
	glPopMatrix();

	// right
	glPushMatrix();
	glTranslatef(size, 0.0, 0.0);
	glRotatef(90.0, 0.0, 0.0, 1.0);
	DrawPlane(size, step);
	glPopMatrix();

	// front
	glPushMatrix();
	glTranslatef(0.0, 0.0, size);
	glRotatef(90.0, -1.0, 0.0, 0.0);
	DrawPlane(size, step);
	glPopMatrix();

	// back
	glPushMatrix();
	glRotatef(90.0, -1.0, 0.0, 0.0);
	glScalef(1.0, -1.0, 1.0);
	DrawPlane(size, step);
	glPopMatrix();

	glPopMatrix();
}


OpenGLApp::OpenGLApp()
{
	m_cubeAngle = 0.0f;
	m_lightAngle = 0.0f;
	m_pSphere = NULL; //initialize pointer to null

	m_flashlightOn = GL_FALSE;
}

OpenGLApp::~OpenGLApp()
{
}


bool OpenGLApp::Initialize()	//initialize your opengl scene
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	//first we are going to set up the flashlight
	//GL_LIGHT0 is going to be the flashlight
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, flashLightPos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, flashLightColor);

	//diffuse
	glLightfv(GL_LIGHT0, GL_DIFFUSE, flashLightColor);
	//specular
	glLightfv(GL_LIGHT0, GL_SPECULAR, flashLightColor);
	//set up the spotlight
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, flashLightDir);

	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 128.0f);
	
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 2.0f); //angle of the cone

	//set up the second light, the staic light that
	//just hangs out off the side of the cube
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, redLightColor);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, redLightColor);
	glLightfv(GL_LIGHT1, GL_SPECULAR, redLightColor);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.50f);

	//initialize the moving green light
	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_AMBIENT, greenLightColor);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, greenLightColor);
	glLightfv(GL_LIGHT2, GL_SPECULAR, greenLightColor);

	//create a quadratic object and store its address
	//in the pointer we created for this class m_pSphere
	m_pSphere = gluNewQuadric();

	return true;
}	
bool OpenGLApp::Shutdown()	//shut down your scene
{
	if (m_pSphere) //if the pointer isn't empty (0)
		gluDeleteQuadric(m_pSphere); //delete the object
	m_pSphere = NULL; // clear the address the pointer stored

	return true;
}

//used to setup your opengl camera and drawing area
void OpenGLApp::SetupProjection(int width, int height){
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// you need 3 things to create a frustum
	// 1. Field of view (52.0f here)
	// 2. aspect ratio of screen (width/height of window)
	// 3. near and far plane (1, and 1000)
	gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	m_windowWidth = width;
	m_windowHeight = height;
}

void OpenGLApp::Prepare(float dt)
{
	//dt is the time elapsed between passes of the main loop

	//incremeting cube angle later to be used to rotate
	m_cubeAngle += CUBE_DEG_PER_S * dt;
	if (m_cubeAngle > 360.0f)
		m_cubeAngle = 0.0f;

	//incremeting light angle later to be used to move the light
	m_lightAngle += LIGHT_DEG_PER_S * dt;
	if (m_lightAngle > 360.0f)
		m_lightAngle - 0.0f;

}
void OpenGLApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//make sure we are in modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//set up/move the camera
	gluLookAt(0.0, 0.0, 20.0, //where to move the camera
			  0.0, 0.0, 0.0, //which vertex should it look at?
			  0.0, 1.0, 0.0); //which way is up? (positive y)

	if (m_flashlightOn)
		glEnable(GL_LIGHT0); //turn on flashlight (light0)
	else
		glDisable(GL_LIGHT0); //turn off flashlight (light0)


	//postion the red light
	glLightfv(GL_LIGHT1, GL_POSITION, redLightPos);

	//draw the red sphere that is going to represent the red light
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glTranslatef(redLightPos[0], redLightPos[1], redLightPos[2]);
		glColor4fv(redLightColor);
		gluSphere(m_pSphere, 0.2, 10, 10);
		glEnable(GL_LIGHTING);
	glPopMatrix();

	//position and draw the green light & sphere
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glRotatef(m_lightAngle, 1.0f, 0.0f, 0.0f);
		glRotatef(m_lightAngle, 0.0f, 1.0f, 0.0f);
		glLightfv(GL_LIGHT2, GL_POSITION, greenLightPos);
		glTranslatef(greenLightPos[0], greenLightPos[1], greenLightPos[2]);
		glColor4fv(greenLightColor);
		gluSphere(m_pSphere, 0.2, 10, 10);
		glEnable(GL_LIGHTING);
	glPopMatrix();

	//set up the cube's material
	GLfloat cubeColor[] = { 0.6f, 0.7f, 1.0f, 0.0f };
	GLfloat cubeSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat cubeEmission[] = { 0.0f, 0.0f, 1.0f, 0.7f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, cubeSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, cubeEmission);

	//draw the cube
	glPushMatrix();
		glRotatef(m_cubeAngle, 1.0f, 1.0f, 0.0f);
		glRotatef(m_cubeAngle, 0.0f, 0.0f, 1.0f);
		DrawCube(CUBE_SIZE, 256);
	glPopMatrix();
	
		
		
	//DrawGrid();


}

void OpenGLApp::DrawGrid()
{
	//increase the size of drawn points
	glPointSize(3.0f);

	glBegin(GL_LINES);
	//change the color to blue
	glColor3f(0.0f, 0.0, 1.0f);

	//vertical
	for (float x = 8; x >= -8; x--)
	{
		glVertex3f(x, 5.0f, 0.0f);
		glVertex3f(x, -5.0f, 0.0f);
	}
	//horizontal
	for (float y = 5; y >= -5; y--)
	{
		glVertex3f(8.0f, y, 0.0f);
		glVertex3f(-8.0f, y, 0.0f);
	}

	glEnd();

	//we are going to draw below
	glBegin(GL_POINTS);

	//change color to white
	glColor3f(1.0f, 1.0f, 1.0f);

	//draw the points on the y-axis
	for (float y = 5; y >= -5; y--)
	{
		glVertex3f(0.0f, y, 0.0f);
	}

	//draw the points on the x-axis
	for (float x = 8; x >= -8; x--)
	{
		glVertex3f(x, 0.0, 0.0f);
	}

	//set the color to red
	glColor3f(1.0f, 0.0f, 0.0f);
	//draw the orgin
	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

}