#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// OpenGL header files
// post compilation link to these libraries
//   they will be utilized by the Linker
#include <mmsystem.h>

#include <gl/GL.h>
#include <gl/GLU.h>
#include "OpenGLApp.h"

#pragma comment(lib, "openGL32.lib")
#pragma comment(lib, "GLU32.lib")
#pragma comment(lib, "winmm.lib")

bool exiting = false; // is the program exiting?
long windowWidth = 800;
long windowHeight = 600;
long windowBits = 32; // color depth
bool fullscreen = false;

HDC hDC; // handle to device context

OpenGLApp *OGLApp = NULL;


void SetupPixelFormat(HDC hDC)
{
    int pixelFormat;

    PIXELFORMATDESCRIPTOR pfd =  // contains our wishlist of settings
    {   
        sizeof(PIXELFORMATDESCRIPTOR),  // size
            1,                          // version
            PFD_SUPPORT_OPENGL |        // OpenGL window
            PFD_DRAW_TO_WINDOW |        // render to window
            PFD_DOUBLEBUFFER,           // support double-buffering
            PFD_TYPE_RGBA,              // color type
            32,                         // prefered color depth
            0, 0, 0, 0, 0, 0,           // color bits (ignored)
            0,                          // no alpha buffer
            0,                          // alpha bits (ignored)
            0,                          // no accumulation buffer
            0, 0, 0, 0,                 // accum bits (ignored)
            16,                         // depth buffer
            0,                          // no stencil buffer
            0,                          // no auxiliary buffers
            PFD_MAIN_PLANE,             // main layer
            0,                          // reserved
            0, 0, 0,                    // no layer, visible, damage masks
    };

	// find the closest match to our desired pixel format
    pixelFormat = ChoosePixelFormat(hDC, &pfd);
	// apply that match as our current pixel format
    SetPixelFormat(hDC, pixelFormat, &pfd);
}


LRESULT CALLBACK MainWindowProc( HWND hWnd, // handle to window
	                             UINT uMsg, // the message
								 WPARAM wParam, // extra message info
								 LPARAM lParam) // extra message info
{
	static HGLRC hRC;
	int h, w;
	switch(uMsg)
	{
		// process the messages here
	case WM_CREATE: // sent when the window is created
		// create the area where opengl can draw
		
		hDC = GetDC(hWnd);  // get a handle to window properties
		SetupPixelFormat(hDC); // assign our pixel format to window

		hRC = wglCreateContext(hDC);	//sets the window for opengl drawing
		wglMakeCurrent(hDC, hRC);		//tell opengl to draw to the window
		break;

	case WM_CLOSE: // sent when window is closed
	case WM_DESTROY: // when window is destoryed
	case WM_QUIT:  // when user wants to end the program

		wglMakeCurrent(hDC, NULL);	//opengl stops drawing to window
		wglDeleteContext(hRC);		//delete the opengl rendering scheme
	
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_SIZE: // called when window is resized
		h = HIWORD(lParam);
		w = LOWORD(lParam);
		OGLApp->SetupProjection(w, h);
		break;

	case WM_KEYUP:
		if (wParam == VK_SPACE) // did they release space bar
			
		break;
	case WM_KEYDOWN:
		if (wParam == VK_SPACE)
			OGLApp->ToggleFlashlight();
		break;
	default:
		break;
	}

	// send the message back to the OS
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



int WINAPI WinMain(HINSTANCE hInstance,
	               HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nShowCmd)
{
	/* 5 steps to creating a window: 
	   1.  Create and fill in a WNDCLASSEX structure (window class)
	   2.  Register the window with windows (via RegisterWindowEX())
	   3.  Create the window (via CreateWindowEX())
	   4.  Create an event loop (to grab messages from queue and send to 5)
	   5.  Create a message processing function (WinProc/Message procedure)
	*/
    WNDCLASSEX windowClass;     // window class
    HWND       hwnd;            // window handle
    MSG        msg;             // message
    DWORD      dwExStyle;       // Window Extended Style
    DWORD      dwStyle;         // Window Style
    RECT       windowRect;


    windowRect.left=(long)0;                        // Set Left Value To 0
    windowRect.right=(long)windowWidth; // Set Right Value To Requested Width
    windowRect.top=(long)0;                         // Set Top Value To 0
    windowRect.bottom=(long)windowHeight;   // Set Bottom Value To Requested Height

    // fill out the window class structure
    windowClass.cbSize          = sizeof(WNDCLASSEX);
    windowClass.style           = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc     = MainWindowProc;
    windowClass.cbClsExtra      = 0;  // reserved
    windowClass.cbWndExtra      = 0;  // reserved
    windowClass.hInstance       = hInstance;
    windowClass.hIcon           = LoadIcon(NULL, IDI_APPLICATION);  // default icon
    windowClass.hCursor         = LoadCursor(NULL, IDC_ARROW);      // default arrow
    windowClass.hbrBackground   = (HBRUSH)GetStockObject(WHITE_BRUSH);                             // don't need background
    windowClass.lpszMenuName    = NULL;                             // no menu
    windowClass.lpszClassName   = "GLClass";
    windowClass.hIconSm         = LoadIcon(NULL, IDI_WINLOGO);      // windows logo small icon

	RegisterClassEx(&windowClass);

	//TODO: Add code to allow full screen start


	hwnd = CreateWindowEx( NULL, // extended styles
		                   "GLClass", // class name
						   "OpenGL program ", // app name
						   WS_OVERLAPPEDWINDOW, // style
						   0, 0, // starting x, y
						   windowWidth,
						   windowHeight,
						   NULL, // parent window
						   NULL, // menu
						   hInstance,
						   NULL); // extra parameters

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	OGLApp = new OpenGLApp;	//create the application
	OGLApp->Initialize();


	// the event loop - grabs messages from the local queue and
	//   sends them to the winproc
	DWORD lastTime = timeGetTime();
	while (!exiting)
	{
		OGLApp->Prepare(static_cast<float>(timeGetTime() - lastTime)/1000.0f);
		lastTime = timeGetTime();
		OGLApp->Render();
		SwapBuffers(hDC);

		// to grab the message use PeekMessage (non blocking)
		//                      or GetMessage (blocking)
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				exiting = true;
			// send message to winproc
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	OGLApp->Shutdown();
	delete OGLApp;

}