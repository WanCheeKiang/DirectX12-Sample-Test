#pragma once
#include<stdint.h>
#include<Windows.h>

enum class KeyCode
{
	Space = 0x20, Control = 0x11,
	A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	Zero = 0x30, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
	Num0 = 0x60, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
	Plus = 0x6B, Minus = 0x6D,
	Left = 0x25, Up, Right, Down
};

enum KeyState
{
	NotDown, DownFirst = 1, Release, Down
};

enum EAxis
{
	WS, AD, UpDown, LeftRight, SpaceM
};

class KeyInput
{
public:
	static void Init();
	static void ResetAxes();
	static void WinProc(UINT message, WPARAM wParam, LPARAM lParam);
	static void Update();
	static int32_t GetMouseX() { return mouseX; };
	static int32_t GetMouseY() { return mouseY; };

	static float GetAxis(EAxis axis);

	static KeyState GetKeyState(KeyCode target);
private:
	static int32_t mouseX;
	static int32_t mouseY;
	static uint8_t keyStates[256];
};

