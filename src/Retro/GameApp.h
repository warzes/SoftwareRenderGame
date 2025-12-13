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

// Wall height and type definitions
enum WallType
{
	WALL_SOLID = 0,        // Solid wall from floor to ceiling
	WALL_SHORT,            // Short wall (e.g., half-height)
	WALL_TALL,             // Tall wall (e.g., double-height)
	WALL_SLOPE_BOTTOM,     // Wall with sloped bottom (ramp up)
	WALL_SLOPE_TOP,        // Wall with sloped top (ramp down)
	WALL_WINDOW,           // Wall with window section (transparent middle)
	WALL_TRANSITION_UP,    // Vertical transition going up
	WALL_TRANSITION_DOWN,  // Vertical transition going down
	WALL_COUNT
};

// Wall height and type arrays
extern double wallHeights[mapWidth][mapHeight]; // Height of each wall tile
extern int wallTypes[mapWidth][mapHeight]; // Type of each wall tile (values correspond to WallType enum)

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
	double adjacentHeight; // Height of adjacent wall for transition effects
};

struct intersect
{
	double tr, tw;
};