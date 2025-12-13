#include "stdafx.h"
#include "Engine.h"
//=============================================================================
extern bool IsExitApp;
//=============================================================================
void Print(const std::string& msg)
{
	puts(msg.c_str());
}
//=============================================================================
void Info(const std::string& msg)
{
	Print("\033[32m[INFO]:\033[0m " + msg);
}
//=============================================================================
void Warning(const std::string& msg)
{
	Print("\033[33m[WARNING]:\033[0m " + msg);
}
//=============================================================================
void Debug(const std::string& msg)
{
#if defined(_DEBUG)
	Print("\033[36m[DEBUG]:\033[0m " + msg);
#endif
}
//=============================================================================
void Error(const std::string& msg)
{
	Print("\033[31m[ERROR]:\033[0m " + msg);
}
//=============================================================================
void Fatal(const std::string& msg)
{
	Print("\033[35m[FATAL]:\033[0m " + msg);
	IsExitApp = true;
}
//=============================================================================
unsigned ColorToUInt(const Color& color)
{
	return ColorToUInt(color.r, color.g, color.b, color.a);
}
//=============================================================================
unsigned ColorToUInt(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	//unsigned rgba = unsigned(r | (g << 8) | (b << 16) | (a << 24));
	unsigned rgba = unsigned(b | (g << 8) | (r << 16) | (a << 24));

	return rgba;
}
//=============================================================================
bool RayIntersect(const Vector2D& rayStart, const Vector2D& rayDir, const Vector2D& wallStart, const Vector2D& wallEnd, float& distance, Vector2D& intersection)
{
	// Рассчитываем пересечение луча с отрезком стены
	Vector2D wallVec = wallEnd - wallStart;
	Vector2D startToRay = rayStart - wallStart;

	float crossProduct = rayDir.x * wallVec.y - rayDir.y * wallVec.x;
	if (abs(crossProduct) < 0.0001f) return false; // Параллельные линии

	float t = (startToRay.x * wallVec.y - startToRay.y * wallVec.x) / crossProduct;
	float u = (startToRay.x * rayDir.y - startToRay.y * rayDir.x) / crossProduct;

	if (t >= 0 && u >= 0 && u <= 1)
	{
		distance = t;
		intersection = rayStart + rayDir * t;
		return true;
	}
	return false;
}
//=============================================================================
void MoveCamera(Camera& camera, float dx, float dy, float dz)
{
	float rad = DegToRad(camera.angle);
	camera.position.x += cosf(rad) * dx - sinf(rad) * dy;
	camera.position.y += sinf(rad) * dx + cosf(rad) * dy;
	camera.position.z += dz;
}
//=============================================================================
void RotateCamera(Camera& camera, float angleChange)
{
	camera.angle += DegToRad(angleChange);
}
//=============================================================================
COLORREF ApplyLighting(COLORREF color, float distance)
{
	float factor = std::max(0.1f, 1.0f - distance * 0.01f);

	BYTE r = GetRValue(color) * factor;
	BYTE g = GetGValue(color) * factor;
	BYTE b = GetBValue(color) * factor;

	return RGB(r, g, b);
}
//=============================================================================