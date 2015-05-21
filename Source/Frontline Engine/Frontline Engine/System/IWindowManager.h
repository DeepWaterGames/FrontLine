#pragma once
#include "../FrontlineCommon.h"
#include "../IGame.h"
#include <GLEW/glew.h>
#include <memory>

struct WindowStyle
{
	bool m_hasBorder;
	bool m_hasCaption;
	bool m_isChild;
	bool m_clipChildren;
	bool m_clipSiblings;
	bool m_isDisabled;
	bool m_dlgFrame;
	bool m_isGroup;
	bool m_hScroll;
	bool m_vScroll;
	bool m_iconic;
	bool m_maximize;
	bool m_maximizeButton;
	bool m_minimize;
	bool m_minimizeButton;
	bool m_isPopup;
	bool m_hasResizeBox;
	bool m_hasSystemMenu;
	bool m_tabStop;
	bool m_visible;
	WindowStyle()
	{
		m_hasBorder = m_hasCaption = m_isChild = m_clipChildren = m_clipSiblings = m_isDisabled = m_dlgFrame = m_isGroup
			= m_hScroll = m_vScroll = m_iconic = m_maximize = m_maximizeButton = m_minimize = m_minimizeButton
			= m_isPopup = m_hasResizeBox = m_hasSystemMenu = m_tabStop = m_visible = false;
	}
	WindowStyle(bool p_hasBorder, bool p_hasCaption, bool p_maximizeButton, bool p_minimizeButton, bool p_dlgFrame, bool p_hasSystemMenu) : m_hasBorder(p_hasBorder), m_hasCaption(p_hasCaption), m_minimizeButton(p_minimizeButton), m_maximizeButton(p_maximizeButton), m_dlgFrame(p_dlgFrame), m_hasSystemMenu(p_hasSystemMenu)
	{

	}
};

const WindowStyle OVERLAPPEDWINDOW = WindowStyle(true, true, true, true, true, true);

class IWindowManager
{
public:
	IWindowManager() {}
	virtual void open(int p_sizeX, int p_sizeY, std::string p_windowTitle, bool p_singleInstance, bool p_fullscreen, WindowStyle p_style = OVERLAPPEDWINDOW, int p_posX = 0, int p_posY = 0) = 0;
	virtual bool resize(int p_sizeX_n, int p_sizeY_n, int p_posX_n = 0, int p_posY_n = 0) = 0;
	virtual void close() = 0;
	virtual void createGLContext() = 0;
	virtual void destroyGLContext() = 0;
	virtual void makeGLContextCurrent() = 0;
	virtual void swapGLBuffers() = 0;
	virtual void updateWindow() = 0;
	virtual bool setFullscreen(bool p_Fullscreen) = 0;
	virtual bool isOpen() = 0;
	~IWindowManager() {}
protected:
	IWindowManager(IWindowManager& manager);
	int m_sizeX, m_sizeY;
	int m_desktopSizeX, m_desktopSizeY;
	bool m_fullscreen;
	WindowStyle m_style;
	std::string m_windowTitle;
};