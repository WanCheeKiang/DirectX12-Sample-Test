#include "GameKeyInput.h"
#include<DirectXMath.h>
#include"Application.h"
#include"Window.h"
GameKeyInput::GameKeyInput(const std::wstring& name, int width, int height, bool vSync)
{
	m_Name = name;
	m_Width = width;
	m_Height = height;
	m_vSync = vSync;
}
GameKeyInput::~GameKeyInput()
{
}
bool GameKeyInput::Initialize()
{
	// Check for DirectX Math library support.
	if (!DirectX::XMVerifyCPUSupport())
	{
		MessageBoxA(NULL, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_pWindow = Application::Get().CreateRenderWindow(m_Name, m_Width, m_Height, m_vSync);
	m_pWindow->RegisterCallbacks(shared_from_this());
	m_pWindow->Show();

	return true;
}

void GameKeyInput::Destroy()
{
	Application::Get().DestroyWindow(m_pWindow);
	m_pWindow.reset();
}

void GameKeyInput::OnUpdate(UpdateEventArgs& e)
{

}

void GameKeyInput::OnRender(RenderEventArgs& e)
{
}

void GameKeyInput::OnKeyPressed(KeyEventArgs& e)
{
}

void GameKeyInput::OnKeyReleased(KeyEventArgs& e)
{
}

void GameKeyInput::OnMouseMoved(MouseMotionEventArgs& e)
{
}

void GameKeyInput::OnMouseButtonPressed(MouseButtonEventArgs& e)
{
}

void GameKeyInput::OnMouseButtonReleased(MouseButtonEventArgs& e)
{
}

void GameKeyInput::OnMouseWheel(MouseWheelEventArgs& e)
{
}

void GameKeyInput::OnResize(ResizeEventArgs& e)
{
}

void GameKeyInput::OnWindowDestroy()
{
}
