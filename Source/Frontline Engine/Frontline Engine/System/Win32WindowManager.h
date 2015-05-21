#pragma once
#include "../FrontlineCommon.h"
#include "../IGame.h"
#include <Windows.h>
#include "IWindowManager.h"
#include "..\KeyboardEventData.h"

class Win32WindowManager :
	public IWindowManager
{
public:
	static std::map<HWND, Win32WindowManager*> g_managerLookup;
	virtual void createGLContext() override;
	virtual void open(int p_sizeX, int p_sizeY, std::string p_windowTitle, bool p_singleInstance, bool p_fullscreen, WindowStyle p_style = OVERLAPPEDWINDOW, int p_posX = 0, int p_posY = 0) override;
	virtual bool resize(int p_sizeX_n, int p_sizeY_n, int p_posX_n = 0, int p_posY_n = 0) override;
	virtual void destroyGLContext() override;
	virtual void makeGLContextCurrent() override;
	virtual void swapGLBuffers() override;
	virtual void close() override;
	virtual bool setFullscreen(bool p_Fullscreen) override;
	virtual void updateWindow() override;
	virtual bool isOpen() override;
	Win32WindowManager(void);
	~Win32WindowManager(void);
	void Win32WindowManager::processKeyboardEvent(WPARAM p_code, bool p_down);
protected:
	WNDCLASSEX m_windowClass;
	HWND m_windowHandle;
	HDC m_deviceContextHandle;
	HGLRC m_glContextHandle;
	HDC m_oldDeviceContextHandle;
	HGLRC m_oldGLContextHandle;
	bool m_open;
};