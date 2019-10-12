#include "KeyInput.h"


int32_t KeyInput::mouseX = 0;
int32_t KeyInput::mouseY = 0;
uint8_t KeyInput::keyStates[256]{};


void KeyInput::Init()
{
	RAWINPUTDEVICE Rinput[2];

	Rinput[0].usUsagePage = 0x01;
	Rinput[0].usUsage = 0x02;
	Rinput[0].dwFlags = 0;   // adds HID mouse and also ignores legacy mouse messages
	Rinput[0].hwndTarget = 0;

	Rinput[1].usUsagePage = 0x01;
	Rinput[1].usUsage = 0x06;
	Rinput[1].dwFlags = 0;   // adds HID keyboard and also ignores legacy keyboard messages
	Rinput[1].hwndTarget = 0;



	if (RegisterRawInputDevices(Rinput, 2, sizeof(Rinput[0])) == FALSE)
	{
		DWORD error = GetLastError();
		DebugBreak();
		exit(EXIT_FAILURE);
	}

	ZeroMemory(keyStates, sizeof(uint8_t) * 256);
}

void KeyInput::ResetAxes()
{
	mouseX = 0;
	mouseY = 0;
}

void KeyInput::WinProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	UINT dwSize = sizeof(RAWINPUT);
	RAWINPUT raw{};

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
		&raw, &dwSize, sizeof(RAWINPUTHEADER));


	if (raw.header.dwType == RIM_TYPEMOUSE)
	{
		KeyInput::mouseX += raw.data.mouse.lLastX;
		KeyInput::mouseY += raw.data.mouse.lLastY;
	}

	if (raw.header.dwType == RIM_TYPEKEYBOARD)
	{
		keyStates[raw.data.keyboard.VKey] &= 0xfe;
		keyStates[raw.data.keyboard.VKey] |= 1 - (RI_KEY_BREAK & raw.data.keyboard.Flags);
	}

}

void KeyInput::Update()
{
	for (int i = 0; i < 256; i++)
	{
		const uint8_t last = 1 & keyStates[i];
		keyStates[i] = (keyStates[i] << 1) | last;
	}

	ResetAxes();
}

float KeyInput::GetAxis(EAxis axis)
{
	float output = 0;

	switch (axis)
	{
	case EAxis::WS:
	{
		output += GetKeyState(KeyCode::W);
		output -= GetKeyState(KeyCode::S);
		return output;
	}
	case EAxis::AD:
	{
		output += GetKeyState(KeyCode::D);
		output -= GetKeyState(KeyCode::A);
		return output;
	}
	case EAxis::UpDown:
	{
		output += GetKeyState(KeyCode::Up);
		output -= GetKeyState(KeyCode::Down);
		return output;
	}
	case EAxis::LeftRight:
	{
		output += GetKeyState(KeyCode::Right);
		output -= GetKeyState(KeyCode::Left);
		return output;
	}
	case EAxis::SpaceM:
	{
		output += GetKeyState(KeyCode::M);
		output -= GetKeyState(KeyCode::Space);
		return output;
	}
	}

	return 0.0f;
}

KeyState KeyInput::GetKeyState(KeyCode target)
{
	KeyState output = static_cast<KeyState>(0x3 & keyStates[static_cast<int>(target)]);
	return output;
}
