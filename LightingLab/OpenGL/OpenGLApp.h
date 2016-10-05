#pragma once
class OpenGLApp
{
private:
	int m_windowWidth;
	int m_windowHeight;

	float m_cubeAngle; //use this to rotate the cube
	float m_lightAngle; //move/position the green light

	GLboolean m_flashlightOn; //help us toggle the flashlight

	GLUquadricObj *m_pSphere; //object to simplify making a sphere

public:
	OpenGLApp();
	~OpenGLApp();

	bool Initialize();	//initialize your opengl scene
	bool Shutdown();	//shut down your scene

	//used to setup your opengl camera and drawing area
	void SetupProjection(int width, int height);

	void Prepare(float dt);
	void Render();

	void DrawGrid();

	void ToggleFlashlight() { m_flashlightOn = !m_flashlightOn; }

};

