#include "stdafx.h"
#include "Engine.h"
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

int g_windowWidth = 1600;
int g_windowHeight = 900;
int g_frameWidth = 0;
int g_frameHeight = 400;
unsigned* g_frameBuffer{ nullptr };
float* g_depthBuffer{ nullptr };
double* g_depthBufferX{ nullptr };

LPARAM g_lastMousePos = 0;
WPARAM g_lastMouseButtons = 0;
BOOL g_mouseMoved = FALSE;

bool keys[256] = { false };
//=============================================================================
unsigned GetPixel(int x, int y)
{
	if (x < 0 || x >= g_frameWidth || y < 0 || y >= g_frameHeight) return 0;
	return g_frameBuffer[y * g_frameWidth + x];
}
//=============================================================================
void SetPixel(int x, int y, unsigned color)
{
	if (x < 0 || x >= g_frameWidth || y < 0 || y >= g_frameHeight) return;
	g_frameBuffer[y * g_frameWidth + x] = color;
}
//=============================================================================
void ResizeBuffer(int width, int height)
{
	float aspectRatio = (float)width / (float)height;
	g_frameWidth = g_frameHeight * aspectRatio;

	// Пересоздаем буферы
	if (g_frameBuffer) delete[] g_frameBuffer;
	if (g_depthBuffer) delete[] g_depthBuffer;
	if (g_depthBufferX) delete[] g_depthBufferX;

	g_frameBuffer = new unsigned[g_frameWidth * g_frameHeight];
	g_depthBuffer = new float[g_frameWidth * g_frameHeight];
	g_depthBufferX = new double[g_frameWidth];
}
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
			g_windowWidth = LOWORD(lParam);
			g_windowHeight = HIWORD(lParam);

			float aspectRatio = (float)g_windowWidth / (float)g_windowHeight;
			g_frameWidth = g_frameHeight * aspectRatio;

			g_bmi.bmiHeader.biWidth = g_frameWidth;
			g_bmi.bmiHeader.biHeight = -g_frameHeight;
			DeleteObject(g_hBitmap);
			void* bits = nullptr;
			g_hBitmap = CreateDIBSection(g_memDC, &g_bmi, DIB_RGB_COLORS, &bits, NULL, 0);
			if (g_hBitmap)
			{
				SelectObject(g_memDC, g_hBitmap);

				// Пересоздаем буферы
				if (g_frameBuffer) delete[] g_frameBuffer;
				if (g_depthBuffer) delete[] g_depthBuffer;
				if (g_depthBufferX) delete[] g_depthBufferX;

				g_frameBuffer = new unsigned[g_frameWidth * g_frameHeight];
				g_depthBuffer = new float[g_frameWidth * g_frameHeight];
				g_depthBufferX = new double[g_frameWidth];
			}
		}
		return 0;
	case WM_KEYDOWN:
		keys[wParam] = true;
		return 0;
	case WM_KEYUP:
		keys[wParam] = false;
		return 0;
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

	DWORD style = WS_OVERLAPPEDWINDOW;
	RECT rect = { 0, 0, g_windowWidth, g_windowHeight };
	AdjustWindowRect(&rect, style, FALSE);

	g_hwnd = CreateWindowEx(
		0, szWindowClass, L"RetroRPG", style, 
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, 
		nullptr, nullptr, hInstance, nullptr);
	if (!g_hwnd)
	{
		return false;
	}

	float aspectRatio = (float)g_windowWidth / (float)g_windowHeight;
	g_frameWidth = g_frameHeight * aspectRatio;

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
	//SetDIBitsToDevice(
	//	g_memDC,
	//	0, 0, (DWORD)g_windowWidth, (DWORD)g_windowHeight,
	//	0, 0, 0, (UINT)g_windowHeight,
	//	g_frameBuffer,
	//	&g_bmi,
	//	DIB_RGB_COLORS
	//);

	//BitBlt(g_hdc, 0, 0, g_windowWidth, g_windowHeight, g_memDC, 0, 0, SRCCOPY);

	StretchDIBits(g_hdc,
		0, 0, g_windowWidth, g_windowHeight, 
		0, 0, g_frameWidth, g_frameHeight, 
		g_frameBuffer, &g_bmi, DIB_RGB_COLORS, SRCCOPY);
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
BOOL winAPIWasMouseMoved()
{
	return g_mouseMoved;
}
//=============================================================================
void winAPIClearInputState()
{
	g_mouseMoved = FALSE;
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

	ResizeBuffer(g_windowWidth, g_windowHeight);

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
			winAPIPresent();
			Sleep(16); // ~60 FPS
		}
	}

	CloseGame();

	if (g_frameBuffer) delete[] g_frameBuffer;
	if (g_depthBuffer) delete[] g_depthBuffer;

	if (g_hBitmap) DeleteObject(g_hBitmap);
	if (g_memDC)   DeleteDC(g_memDC);
	if (g_hdc)     ReleaseDC(g_hwnd, g_hdc);
	if (g_hwnd)    DestroyWindow(g_hwnd);

	return 0;
}
//=============================================================================
#endif // ENABLE_WINAPI