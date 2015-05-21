#include "Win32WindowManager.h"
#include <iostream>
#if _MSC_VER >= 1300 // for VC 7.0
// from ATL 7.0 sources
#ifndef _delayimp_h
extern "C" IMAGE_DOS_HEADER __ImageBase;
#endif
#endif

std::map<HWND, Win32WindowManager*> Win32WindowManager::g_managerLookup = std::map<HWND, Win32WindowManager*>();

HMODULE GetCurrentModule()
{
#if _MSC_VER < 1300 // earlier than .NET compiler (VC 6.0)
	// Here's a trick that will get you the handle of the module
	// you're running in without any a-priori knowledge:
	// http://www.dotnet247.com/247reference/msgs/13/65259.aspx

	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery(&dummy, &mbi, sizeof(mbi));

	return reinterpret_cast<HMODULE>(mbi.AllocationBase);

#else // VC 7.0
	// from ATL 7.0 sources

	return reinterpret_cast<HMODULE>(&__ImageBase);
#endif
}

Win32WindowManager::Win32WindowManager(void)
{
}

void Win32WindowManager::makeGLContextCurrent()
{
	m_oldDeviceContextHandle = wglGetCurrentDC();
	m_oldGLContextHandle = wglGetCurrentContext();
	wglMakeCurrent(m_deviceContextHandle, m_glContextHandle);
}

void Win32WindowManager::swapGLBuffers()
{
	SwapBuffers(m_deviceContextHandle);
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		Win32WindowManager::g_managerLookup[hWnd]->close();
		break;
	case WM_KEYDOWN:
		Win32WindowManager::g_managerLookup[hWnd]->processKeyboardEvent(wParam, true);
		break;
	case WM_KEYUP:
		Win32WindowManager::g_managerLookup[hWnd]->processKeyboardEvent(wParam, false);
		break;
	default:
		// Do not handle this message; make windows call it instead in the default way
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

void Win32WindowManager::processKeyboardEvent(WPARAM p_code, bool p_down)
{
	KeyboardEventData* le_event = FL_NEW KeyboardEventData();
	le_event->code = p_code;
	le_event->m_down = p_down;
	IGame::gp_game->mp_eventManager->VQueueEvent(std::shared_ptr<KeyboardEventData>(le_event));
}

bool Win32WindowManager::setFullscreen(bool p_fullscreen)
{
	m_fullscreen = p_fullscreen;
	// Turn the WindowStyle struct into something that win32 can understand
	DWORD l_windowStyle = ((m_style.m_clipChildren) ? WS_CLIPCHILDREN : 0) | ((m_style.m_clipSiblings) ? WS_CLIPSIBLINGS : 0) | ((m_style.m_dlgFrame) ? WS_DLGFRAME : 0)
		| ((m_style.m_hasBorder) ? WS_BORDER : 0) | ((m_style.m_hasCaption) ? WS_CAPTION : 0) | ((m_style.m_hasResizeBox) ? WS_SIZEBOX : 0)
		| ((m_style.m_hasSystemMenu) ? WS_SYSMENU : 0) | ((m_style.m_hScroll) ? WS_HSCROLL : 0) | ((m_style.m_iconic) ? WS_ICONIC : 0)
		| ((m_style.m_isChild) ? WS_SYSMENU : 0) | ((m_style.m_isDisabled) ? WS_DISABLED : 0) | ((m_style.m_isGroup) ? WS_GROUP : 0)
		| ((m_style.m_isPopup) ? WS_POPUP : 0) | ((m_style.m_maximize) ? WS_MAXIMIZE : 0) | ((m_style.m_maximizeButton) ? WS_MAXIMIZEBOX : 0)
		| ((m_style.m_minimize) ? WS_MINIMIZE : 0) | ((m_style.m_minimizeButton) ? WS_MINIMIZEBOX : 0) | ((m_style.m_tabStop) ? WS_TABSTOP : 0)
		| ((m_style.m_visible) ? WS_VISIBLE : 0) | ((m_style.m_vScroll) ? WS_VSCROLL : 0);
	if (p_fullscreen)
	{
		l_windowStyle = l_windowStyle | WS_POPUP | WS_EX_TOPMOST;
		l_windowStyle = l_windowStyle & ~WS_OVERLAPPEDWINDOW;
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = m_sizeX;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight = m_sizeY;				// Selected Screen Height
		dmScreenSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			setFullscreen(false);
			return false;
		}
	}
	else
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = m_desktopSizeX;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight = m_desktopSizeY + 62;				// Selected Screen Height
		dmScreenSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			return false;
		}
	}
	// Modify the window values
	SetWindowLongPtr(m_windowHandle, GWL_STYLE, l_windowStyle);
	SetWindowPos(m_windowHandle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	ShowWindow(m_windowHandle, SW_NORMAL);
	return true;
}

void Win32WindowManager::close()
{
	setFullscreen(false);
	destroyGLContext();
	DestroyWindow(m_windowHandle);
	g_managerLookup.erase(m_windowHandle);
	m_open = false;
}

void Win32WindowManager::createGLContext()
{

	// Get the device context handle for the window
	m_deviceContextHandle = GetDC(m_windowHandle);

	// Set the pixel format descriptor
	PIXELFORMATDESCRIPTOR l_pfd;
	ZeroMemory(&l_pfd, sizeof(l_pfd));
	l_pfd.nSize = sizeof(l_pfd);
	l_pfd.nVersion = 1;
	l_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER; // Set the pfd flags
	l_pfd.iPixelType = PFD_TYPE_RGBA; // The pixel type
	l_pfd.cColorBits = 24; // Color bits per pixel
	l_pfd.cDepthBits = 16; // Bits in the depth buffer per pixel
	l_pfd.iLayerType = PFD_MAIN_PLANE;
	int l_iFormat = ChoosePixelFormat(m_deviceContextHandle, &l_pfd);
	SetPixelFormat(m_deviceContextHandle, l_iFormat, &l_pfd); // Finally set the created pfd

	// Create the OpenGL Context
	m_glContextHandle = wglCreateContext(m_deviceContextHandle);

	wglMakeCurrent(m_deviceContextHandle, m_glContextHandle);

	// Initialize GLEW (OpenGL Extension Wrangler)
	glewInit();
}

void Win32WindowManager::destroyGLContext()
{
	if (m_glContextHandle)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_glContextHandle);
	}
	if (m_windowHandle && m_deviceContextHandle)
	{
		ReleaseDC(m_windowHandle, m_deviceContextHandle);
	}
}

void Win32WindowManager::open(int p_sizeX, int p_sizeY, std::string p_windowTitle, bool p_singleInstance, bool p_fullscreen, WindowStyle p_style, int p_posX, int p_posY)
{
	m_desktopSizeX = GetSystemMetrics(SM_CXFULLSCREEN);
	m_desktopSizeY = GetSystemMetrics(SM_CYFULLSCREEN);
	m_windowTitle = p_windowTitle;
	m_style = p_style;

	ZeroMemory(&m_windowClass, sizeof(WNDCLASSEX));

	// Set attributes for the window class
	m_windowClass.cbClsExtra = NULL;
	m_windowClass.cbSize = sizeof(WNDCLASSEX);
	m_windowClass.cbWndExtra = NULL;
	// If the window is fullscreen, the hbrBackground field should be left alone
	if (!p_fullscreen)
		m_windowClass.hbrBackground = (HBRUSH) COLOR_WINDOW;
	m_windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	m_windowClass.hIcon = NULL;
	m_windowClass.hIconSm = NULL;
	m_windowClass.hInstance = GetCurrentModule();
	m_windowClass.lpszClassName = "Window Class";
	m_windowClass.lpszMenuName = NULL;
	m_windowClass.style = CS_HREDRAW | CS_VREDRAW;
	m_windowClass.lpfnWndProc = (WNDPROC) WindowProcedure;

	// Register the window class and check for errors
	if (!RegisterClassEx(&m_windowClass))
	{
		// Errors occured
		//int nResult=GetLastError();
		//MessageBox(NULL,
		//"Window class creation failed",
		//"Window Class Failed",
		//MB_ICONERROR);
	}

	// Turn the WindowStyle struct into a DWORD that windows explicitly understands
	DWORD l_windowStyle = ((p_style.m_clipChildren) ? WS_CLIPCHILDREN : 0) | ((p_style.m_clipSiblings) ? WS_CLIPSIBLINGS : 0) | ((p_style.m_dlgFrame) ? WS_DLGFRAME : 0)
		| ((p_style.m_hasBorder) ? WS_BORDER : 0) | ((p_style.m_hasCaption) ? WS_CAPTION : 0) | ((p_style.m_hasResizeBox) ? WS_SIZEBOX : 0)
		| ((p_style.m_hasSystemMenu) ? WS_SYSMENU : 0) | ((p_style.m_hScroll) ? WS_HSCROLL : 0) | ((p_style.m_iconic) ? WS_ICONIC : 0)
		| ((p_style.m_isChild) ? WS_SYSMENU : 0) | ((p_style.m_isDisabled) ? WS_DISABLED : 0) | ((p_style.m_isGroup) ? WS_GROUP : 0)
		| ((p_style.m_isPopup) ? WS_POPUP : 0) | ((p_style.m_maximize) ? WS_MAXIMIZE : 0) | ((p_style.m_maximizeButton) ? WS_MAXIMIZEBOX : 0)
		| ((p_style.m_minimize) ? WS_MINIMIZE : 0) | ((p_style.m_minimizeButton) ? WS_MINIMIZEBOX : 0) | ((p_style.m_tabStop) ? WS_TABSTOP : 0)
		| ((p_style.m_visible) ? WS_VISIBLE : 0) | ((p_style.m_vScroll) ? WS_VSCROLL : 0);

	m_sizeX = p_sizeX;
	m_sizeY = p_sizeY;

	// If the window should be fullscreen, add the popup and topmost styles
	if (p_fullscreen)
	{
		l_windowStyle = l_windowStyle | WS_POPUP | WS_EX_TOPMOST;
		l_windowStyle = l_windowStyle & ~WS_OVERLAPPEDWINDOW;
		if (!setFullscreen(true)) {
			ERRLOG("Window", "Fullscreen failed");
		}
	}

	// Create the window with the specified parameters
	m_windowHandle = CreateWindowEx(NULL,
		"Window Class",
		p_windowTitle.c_str(),
		l_windowStyle,
		0,
		0,
		m_sizeX,
		m_sizeY,
		NULL,
		NULL,
		m_windowClass.hInstance,
		NULL);

	ShowWindow(m_windowHandle, SW_SHOWNORMAL);
	g_managerLookup[m_windowHandle] = this;
	m_open = true;
}

Win32WindowManager::~Win32WindowManager(void)
{
}

bool Win32WindowManager::resize(int p_sizeX_n, int p_sizeY_n, int p_posX_n, int p_posY_n)
{
	// Turn the WindowStyle struct into something that win32 can understand
	DWORD l_windowStyle = ((m_style.m_clipChildren) ? WS_CLIPCHILDREN : 0) | ((m_style.m_clipSiblings) ? WS_CLIPSIBLINGS : 0) | ((m_style.m_dlgFrame) ? WS_DLGFRAME : 0)
		| ((m_style.m_hasBorder) ? WS_BORDER : 0) | ((m_style.m_hasCaption) ? WS_CAPTION : 0) | ((m_style.m_hasResizeBox) ? WS_SIZEBOX : 0)
		| ((m_style.m_hasSystemMenu) ? WS_SYSMENU : 0) | ((m_style.m_hScroll) ? WS_HSCROLL : 0) | ((m_style.m_iconic) ? WS_ICONIC : 0)
		| ((m_style.m_isChild) ? WS_SYSMENU : 0) | ((m_style.m_isDisabled) ? WS_DISABLED : 0) | ((m_style.m_isGroup) ? WS_GROUP : 0)
		| ((m_style.m_isPopup) ? WS_POPUP : 0) | ((m_style.m_maximize) ? WS_MAXIMIZE : 0) | ((m_style.m_maximizeButton) ? WS_MAXIMIZEBOX : 0)
		| ((m_style.m_minimize) ? WS_MINIMIZE : 0) | ((m_style.m_minimizeButton) ? WS_MINIMIZEBOX : 0) | ((m_style.m_tabStop) ? WS_TABSTOP : 0)
		| ((m_style.m_visible) ? WS_VISIBLE : 0) | ((m_style.m_vScroll) ? WS_VSCROLL : 0);

	m_sizeX = p_sizeX_n;
	m_sizeY = p_sizeY_n;

	glViewport(0, 0, m_sizeX, m_sizeY);

	SetWindowPos(m_windowHandle, HWND_TOP, p_posX_n, p_posY_n, m_sizeX, m_sizeY, SWP_SHOWWINDOW);
	return true;
}

void Win32WindowManager::updateWindow()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

bool Win32WindowManager::isOpen()
{
	return m_open;
}