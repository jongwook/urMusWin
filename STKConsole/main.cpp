// GLRect.cpp
// OpenGL SuperBible, 
// Program by Richard S. Wright Jr.
// This program shows a very simple OpenGL program using
// the standard Win32 API function calls.

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
//#include <gl/glext.h>

#include "opengl.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "urAPI.h"
#include "AudioLayer.h"

extern lua_State *lua;

static LPCTSTR lpszAppName = L"urMus Simulator";


// Declaration for Window procedure
LRESULT CALLBACK WndProc(	HWND 	hWnd,
							UINT	message,
							WPARAM	wParam,
							LPARAM	lParam);

// Set Pixel Format function - forward declaration
void SetDCPixelFormat(HDC hDC);

///////////////////////////////////////////
// Window has changed size
void ChangeSize(GLsizei w, GLsizei h) {
	float windowWidth, windowHeight;

	if(h == 0) h = 1;
	
    glViewport(0, 0, w, h);
    glLoadIdentity();

	if (w <= h) {
		windowHeight = 480.0f*h/w;
		windowWidth = 320.0f;
	} else {
		windowWidth = 480.0f*w/h;
		windowHeight = 320.0f;
	}

	gluOrtho2D(0.0f, 320.0f, 0.0f, 480.0f);
}


/////////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void) {
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}


/////////////////////////////////////////////////////////////////
// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC) {
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,								// Version of this structure	
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,				// Double buffered mode
		PFD_TYPE_RGBA,					// RGBA Color mode
		32,								// Want 32 bit color 
		0,0,0,0,0,0,					// Not used to select mode
		0,0,							// Not used to select mode
		0,0,0,0,0,						// Not used to select mode
		16,								// Size of depth buffer
		0,								// Not used 
		0,								// Not used 
		0,	            				// Not used 
		0,								// Not used 
		0,0,0 };						// Not used 

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}


#if (defined _CONSOLE) || (!defined _WIN32)

int main(int argc, char ** argv) {
	return WinMain((HINSTANCE)GetModuleHandle(NULL), NULL, "", 0);
}

#endif

//////////////////////////////////////////////////////////////////
// Entry point of all Windows programs
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG			msg;		// Windows message structure
	WNDCLASS	wc;			// Windows class structure
	HWND		hWnd;		// Storeage for window handle
	RECT		rcClient, rcWindow;
	POINT		ptDiff;

	char *temp=(char *)glGetString(GL_EXTENSIONS);

	// Register Window style
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC |  CS_DBLCLKS;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance 		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	
	// No need for background brush for OpenGL window
	wc.hbrBackground	= NULL;		
	
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= lpszAppName;

	// Register the window class
	if(RegisterClass(&wc) == 0)
		return FALSE;


	// Create the main application window
	hWnd = CreateWindow(
				lpszAppName,
				lpszAppName,
				
				// OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS
				WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
	
				// Window position and size
				100, 100,
				250, 250,
				NULL,
				NULL,
				hInstance,
				NULL);

	// If window was not created, quit
	if(hWnd == NULL)
		return FALSE;


	// Display the window
	ShowWindow(hWnd,SW_SHOW);

	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWindow);
	ptDiff.x = (rcWindow.right-rcWindow.left)-rcClient.right;
	ptDiff.y = (rcWindow.bottom-rcWindow.top)-rcClient.bottom;
	MoveWindow(hWnd, rcWindow.left, rcWindow.top, 320+ptDiff.x, 480+ptDiff.y, TRUE);

	UpdateWindow(hWnd);

	// Process application messages until the application closes
	while( GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


char * toMultibyte(const wchar_t * source) {
	static char result[260];
	wcstombs(result,source,sizeof(result)/sizeof(char));
	return result;
}

wchar_t * toWidechar(const char * source) {
	static wchar_t result[260];
	mbstowcs(result,source,sizeof(result)/sizeof(wchar_t));
	return result;
}


//////////////////////////////////////////////////////////////
// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND 	hWnd,UINT	message, WPARAM	wParam, LPARAM	lParam) {
	static HGLRC hRC = NULL;		// Permenant Rendering context
	static HDC hDC = NULL;			// Private GDI Device context
	static OPENFILENAME ofn;
	static wchar_t szFile[260];
	static bool bMouseDown=false;
	int ret;
	TRACKMOUSEEVENT tme;

	switch (message) {
		case WM_CREATE:
			// Store the device context
			hDC = GetDC(hWnd);		

			// Select the pixel format
			SetDCPixelFormat(hDC);		

			// Create the rendering context and make it current
			hRC = wglCreateContext(hDC);
			ret=wglMakeCurrent(hDC, hRC);
			initExtensions();

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize=sizeof(ofn);
			ofn.hwndOwner=hWnd;
			ofn.lpstrFile=szFile;
			ofn.lpstrFile[0]='\0';
			ofn.nMaxFile=sizeof(szFile);
			ofn.lpstrFilter=L"Lua File(*.lua)\0*.lua\0All(*.*)\0*.*\0";
			ofn.nFilterIndex=0;
			ofn.nMaxFileTitle=0;
			ofn.lpstrInitialDir=L".";
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if(!GetOpenFileName(&ofn)) {
				PostQuitMessage(0);
				break;
			}

			lua=lua_open();
			luaL_openlibs(lua);
			//luaopen_lfs(lua);
			l_setupAPI(lua);

			if(luaL_dofile(lua, toMultibyte(ofn.lpstrFile))!=0) {
				const char* error=lua_tostring(lua, -1);
				fputs(error, stderr);
				lua_close(lua);
				PostQuitMessage(0);
				break;
			}
			

			SetTimer(hWnd,33,1,NULL);
			break;

		case WM_DESTROY:
			wglMakeCurrent(hDC,NULL);
			wglDeleteContext(hRC);
			lua_close(lua);
			cleanupAudioLayer();
			PostQuitMessage(0);
			break;

		case WM_SIZE:
			ChangeSize(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_TIMER:
			
			InvalidateRect(hWnd,NULL,FALSE);
			
			break;
		case WM_LBUTTONDOWN:
			touchesBegan(LOWORD(lParam), HIWORD(lParam));
			bMouseDown=true;
			break;

		case WM_LBUTTONUP:
			touchesEnded(LOWORD(lParam), HIWORD(lParam));
			bMouseDown=false;
			break;
		case WM_MOUSEMOVE:
			if(wParam & MK_LBUTTON) {
				touchesMoved(LOWORD(lParam), HIWORD(lParam));
			}
			
			tme.cbSize=sizeof(tme);
			tme.dwFlags=TME_LEAVE;
			tme.hwndTrack=hWnd;
			tme.dwHoverTime=HOVER_DEFAULT;
			TrackMouseEvent(&tme);

			break;

		case WM_MOUSELEAVE:
			if(bMouseDown) {
				touchesEnded(LOWORD(lParam), HIWORD(lParam));
				bMouseDown=false;
			}
			break;

		case WM_LBUTTONDBLCLK:
			doubleClick(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_PAINT:
			initExtensions();
			layoutSubviews();
		
			SwapBuffers(hDC);
			ValidateRect(hWnd,NULL);
			break;

        default:   
            return (DefWindowProc(hWnd, message, wParam, lParam));

        }
    return (0L);
}


