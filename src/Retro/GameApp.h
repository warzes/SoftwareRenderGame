#pragma once

extern int g_frameWidth;
extern int g_frameHeight;
extern unsigned* g_frameBuffer;
extern float* g_depthBuffer;
extern double* g_depthBufferX;
extern bool keys[256];

unsigned GetPixel(int x, int y);
void SetPixel(int x, int y, unsigned color);

bool InitGame();
void CloseGame();
void FrameGame();