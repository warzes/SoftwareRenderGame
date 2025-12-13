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

// Helper prototypes
void RenderSkybox(int w, int h);
void RenderFloor(int w, int h);
void RenderCeiling(int w, int h);
void RenderWalls(int w, int h);

struct Sprite
{
	double x;
	double y;
	int texture;
};

struct Door
{
	int x, y;
	int counter;
	DoorState state;
};

struct PushWall
{
	int x, y;
	int counter;
	DoorState state;
	Direction direction;
};

// Ray casting helpers
struct RayContext
{
	double rayDirX, rayDirY;
	int mapX, mapY;
	double sideDistX, sideDistY;
	double deltaDistX, deltaDistY;
	int stepX, stepY;
};

struct Strip
{
	int x;
	int drawStart, drawEnd;
	double perpWallDist;
	std::vector<unsigned>& texture;
	int texX;
	double fog;
	int side;
	bool seeThrough;
};

struct intersect
{
	double tr, tw;
};