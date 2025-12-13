#include "stdafx.h"
#include "GameApp.h"
#include "Engine.h"
//=============================================================================
extern int g_frameWidth;
extern int g_frameHeight;
extern unsigned* g_frameBuffer;
extern float* g_depthBuffer;
extern bool keys[256];
//=============================================================================
bool InitGame()
{
	return true;
}
//=============================================================================
void CloseGame()
{
}
//=============================================================================
void FrameGame()
{
	// Очищаем буферы
	for (int i = 0; i < g_frameWidth * g_frameHeight; i++)
	{
		g_frameBuffer[i] = ColorToUInt(50, 50, 255); // Темно-синий фон
		g_depthBuffer[i] = 10000.0f; // Бесконечность
	}
}

//=============================================================================