#include "stdafx.h"
#include "EngineCore.h"
#if ENABLE_WINAPI
//=============================================================================
constexpr const wchar_t* szWindowClass = L"RetroGame_WindowClass";
//=============================================================================
bool IsExitApp{ false };
HWND g_hwnd{ nullptr };
HINSTANCE g_hInstance{ nullptr };
HDC g_hdc{ nullptr };
HDC g_memDC{ nullptr };
HBITMAP g_hBitmap{ nullptr };
BITMAPINFO g_bmi;

int g_frameWidth = 1600;
int g_frameHeight = 900;
unsigned* g_frameBuffer{ nullptr };
float* g_depthBuffer{ nullptr };
uint8_t* g_stencilBuffer{ nullptr };

WPARAM g_lastKey = 0;
LPARAM g_lastMousePos = 0;
WPARAM g_lastMouseButtons = 0;
BOOL g_keyPressed = FALSE;
BOOL g_mouseMoved = FALSE;
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (message)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	//case WM_PAINT:
	//{
	//	PAINTSTRUCT ps;
	//	HDC hdc = BeginPaint(hWnd, &ps);
	//	EndPaint(hWnd, &ps);
	//}
	//break;
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			g_frameWidth = LOWORD(lParam);
			g_frameHeight = HIWORD(lParam);

			g_bmi.bmiHeader.biWidth = g_frameWidth;
			g_bmi.bmiHeader.biHeight = -g_frameHeight;
			DeleteObject(g_hBitmap);
			void* bits = nullptr;
			g_hBitmap = CreateDIBSection(g_memDC, &g_bmi, DIB_RGB_COLORS, &bits, NULL, 0);
			if (g_hBitmap)
			{
				SelectObject(g_memDC, g_hBitmap);

				// Пересоздаем буферы
				if (g_frameBuffer) free(g_frameBuffer);
				if (g_depthBuffer) free(g_depthBuffer);
				if (g_stencilBuffer) free(g_stencilBuffer);

				g_frameBuffer = (unsigned*)malloc(g_frameWidth * g_frameHeight * sizeof(unsigned));
				g_depthBuffer = (float*)malloc(g_frameWidth * g_frameHeight * sizeof(float));
				g_stencilBuffer = (uint8_t*)malloc(g_frameWidth * g_frameHeight * sizeof(uint8_t));
			}
		}
		return 0;
	case WM_KEYDOWN:
	case WM_KEYUP:
		g_lastKey = wParam;
		g_keyPressed = (message == WM_KEYDOWN);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		g_lastMousePos = lParam;
		g_lastMouseButtons = wParam;
		if (message == WM_MOUSEMOVE)
		{
			g_mouseMoved = TRUE;
		}
		break;
	case WM_MOUSEWHEEL:
		g_lastMouseButtons = wParam;
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//=============================================================================
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	g_hInstance = hInstance;

	WNDCLASSEX wcex    = { 0 };
	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc   = WndProc;
	wcex.hInstance     = hInstance;
	wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = szWindowClass;

	return RegisterClassEx(&wcex);
}
//=============================================================================
bool CreateMainWindow(HINSTANCE hInstance)
{
	MyRegisterClass(hInstance);

	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	RECT rect = { 0, 0, g_frameWidth, g_frameHeight };
	AdjustWindowRect(&rect, style, FALSE);

	g_hwnd = CreateWindowEx(
		0, szWindowClass, L"RetroRPG", style, 
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, 
		nullptr, nullptr, hInstance, nullptr);
	if (!g_hwnd)
	{
		return false;
	}

	g_hdc = GetDC(g_hwnd);
	g_memDC = CreateCompatibleDC(g_hdc);

	ZeroMemory(&g_bmi, sizeof(BITMAPINFO));
	g_bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	g_bmi.bmiHeader.biWidth       = g_frameWidth;
	g_bmi.bmiHeader.biHeight      = -g_frameHeight; // Отрицательное значение для верхнего левого угла
	g_bmi.bmiHeader.biPlanes      = 1;
	g_bmi.bmiHeader.biBitCount    = 32;
	g_bmi.bmiHeader.biCompression = BI_RGB;

	void* bits = nullptr;
	g_hBitmap = CreateDIBSection(g_memDC, &g_bmi, DIB_RGB_COLORS, &bits, NULL, 0);
	if (!g_hBitmap)
	{
		return false;
	}
	SelectObject(g_memDC, g_hBitmap);

	ShowWindow(g_hwnd, SW_SHOW);
	UpdateWindow(g_hwnd);

	return true;
}
//=============================================================================
void winAPIPresent()
{
	SetDIBitsToDevice(
		g_memDC,
		0, 0, (DWORD)g_frameWidth, (DWORD)g_frameHeight,
		0, 0, 0, (UINT)g_frameHeight,
		g_frameBuffer,
		&g_bmi,
		DIB_RGB_COLORS
	);

	BitBlt(g_hdc, 0, 0, g_frameWidth, g_frameHeight, g_memDC, 0, 0, SRCCOPY);
}
//=============================================================================
WPARAM winAPIGetLastKey()
{
	return g_lastKey;
}
//=============================================================================
LPARAM winAPIGetLastMousePos()
{
	return g_lastMousePos;
}
//=============================================================================
WPARAM winAPIGetLastMouseButtons()
{
	return g_lastMouseButtons;
}
//=============================================================================
BOOL winAPIWasKeyPressed()
{
	return g_keyPressed;
}
//=============================================================================
BOOL winAPIWasMouseMoved()
{
	return g_mouseMoved;
}
//=============================================================================
void winAPIClearInputState()
{
	g_keyPressed = FALSE;
	g_mouseMoved = FALSE;
}
//=============================================================================
uint16_t winAPIGetNextKey()
{
	if (g_keyPressed) {
		g_keyPressed = FALSE; // сброс после получения
		return (uint16_t)g_lastKey;
	}
	return 0; // нет нажатий
}
//=============================================================================
void winAPINextMouseDelta(double* deltaX, double* deltaY)
{
	static int lastX = 0, lastY = 0;
	int currentX = LOWORD(g_lastMousePos);
	int currentY = HIWORD(g_lastMousePos);

	if (g_mouseMoved) {
		*deltaX = (double)(currentX - lastX);
		*deltaY = (double)(currentY - lastY);
		lastX = currentX;
		lastY = currentY;
		g_mouseMoved = FALSE;
	}
	else {
		*deltaX = 0.0;
		*deltaY = 0.0;
	}
}
//=============================================================================
uint16_t winAPIMouseButtons()
{
	// Преобразование WinAPI кодов кнопок мыши в формат, совместимый с оригинальной библиотекой
	uint16_t result = 0;
	if (g_lastMouseButtons & MK_LBUTTON) result |= MOUSE_BUTTON_LEFT;
	if (g_lastMouseButtons & MK_RBUTTON) result |= MOUSE_BUTTON_RIGHT;
	if (g_lastMouseButtons & MK_MBUTTON) result |= MOUSE_BUTTON_MIDDLE;
	return result;
}
//=============================================================================
bool InitGame();
void CloseGame();
void FrameGame();
//=============================================================================
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	IsExitApp = false;
	if (!CreateMainWindow(hInstance))
		return 0;

	g_frameBuffer = (unsigned*)malloc(g_frameWidth * g_frameHeight * sizeof(unsigned));
	g_depthBuffer = (float*)malloc(g_frameWidth * g_frameHeight * sizeof(float));
	g_stencilBuffer = (uint8_t*)malloc(g_frameWidth * g_frameHeight * sizeof(uint8_t));

	if (!InitGame())
		return 0;

	MSG msg;
	while (!IsExitApp)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				IsExitApp = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			FrameGame();

			int r = 128;
			int g = 78;
			int b = 200;
			int a = 255;
			unsigned rgba = r | (g << 8) | (b << 16) | (a << 24);
			for (int y = 0; y < g_frameHeight; y++)
			{
				const int k = y * g_frameWidth;
				for (int x = 0; x < g_frameWidth; x++)
					g_frameBuffer[x + k] = rgba;
			}

			winAPIPresent();

			if (winAPIGetNextKey() == 27)
				IsExitApp = true;

			//Sleep(16); // ~60 FPS
		}
	}

	CloseGame();

	if (g_frameBuffer) free(g_frameBuffer);
	if (g_depthBuffer) free(g_depthBuffer);
	if (g_stencilBuffer) free(g_stencilBuffer);

	if (g_hBitmap) DeleteObject(g_hBitmap);
	if (g_memDC)   DeleteDC(g_memDC);
	if (g_hdc)     ReleaseDC(g_hwnd, g_hdc);
	if (g_hwnd)    DestroyWindow(g_hwnd);

	return 0;
}
//=============================================================================
#endif // ENABLE_WINAPI