#include "stdafx.h"
#include "GameApp.h"
#include "Engine.h"
//=============================================================================
double posX = 22.0, posY = 11.5; //x and y start position
double dirX = -1.0, dirY = 0.0; //initial direction vector
double planeX = 0.0, planeY = 0.66; //the 2d raycaster version of camera plane
double lookVert = 0;
double eyePos = 0;

double doorTime = 0;

std::vector<unsigned> texture[23];
#if SKYBOX
std::vector<unsigned> skybox{ 320 * 240 };
#endif

// Floor tile mapping - maps floor tile types to texture indices
int floorTileTextures[FLOOR_TILE_COUNT] = { 3, 3, 4, 5, 6, 7 }; // Default mappings

// Ceiling type mapping - maps ceiling types to texture indices or behavior
int ceilingTypeTextures[CEILING_TYPE_COUNT] = { 0, 6, 6 }; // Sky doesn't need texture, solid and textured use texture 6 by default

// Floor and ceiling map - separate from wall map to allow different properties
int floorMap[mapWidth][mapHeight] =
{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 2},
	{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2},
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 2, 1, 2},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 2},
	{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 2, 0, 2},
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2},
	{1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1},
	{1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 2},
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 2, 0, 1, 0, 2, 1, 2, 0, 2, 2, 2},
	{1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 2, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1},
	{2, 2, 2, 0, 2, 0, 0, 1, 2, 1, 0, 0, 2, 0, 2, 1, 0, 0, 0, 0, 0, 1},
	{2, 2, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
	{2, 0, 0, 0, 0, 2, 2, 0, 2, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 2, 0, 2, 1, 0, 0, 0, 1, 1},
	{2, 1, 0, 0, 2, 0, 0, 2, 2, 2, 1, 2, 2, 2, 0, 2, 0, 0, 1, 0, 1, 0, 1},
	{2, 1, 0, 0, 0, 2, 1, 2, 2, 1, 0, 0, 1, 2, 0, 0, 0, 1, 0, 0, 1, 1},
	{2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 1, 0, 1, 0, 1, 0, 1, 0, 1},
	{1, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{2, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 1, 0, 1, 0, 1, 0, 1, 0, 1},
	{2, 2, 1, 0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 1, 2, 0, 1, 0, 1, 0, 0, 0, 1, 1},
	{2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

int ceilingMap[mapWidth][mapHeight] =
{
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
	{1, 1, 2, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2},
	{1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 2},
	{1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 1, 2, 2, 1, 1, 1, 2},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 2},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2},
	{2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2},
	{2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2},
	{2, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2},
	{2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2},
	{2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 2, 1, 2, 2, 2},
	{2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 2, 2, 1, 2, 2, 2},
	{2, 2, 2, 2, 1, 2, 1, 1, 1, 2, 2, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2},
	{2, 2, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 2},
	{2, 1, 1, 1, 2, 2, 1, 2, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2},
	{2, 1, 1, 1, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 1, 2, 2},
	{2, 2, 1, 1, 1, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2},
	{2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 1, 2, 2},
	{2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2},
	{2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
	{2, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2},
	{2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 1, 2, 1, 2, 1, 1, 1, 2, 2},
	{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
};

int worldMap[mapWidth][mapHeight] =
{
	{8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 6, 4, 4, 6, 4, 6, 4, 4, 4, 6, 4},
	{8, 0, 0, 0, 0, 0, 0, 0,12, 0, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
	{8, 0, 3, 3, 0, 0, 0, 0, 8, 8, 8, 4, 0, 0, 0, 1, 1,14, 1, 1, 0, 0, 0, 6},
	{8, 0, 0, 3, 0, 0, 0, 0,10, 0,10, 0, 0, 0, 0,14, 0, 0, 0,14, 0, 0, 0, 6},
	{8, 0, 3, 3, 0, 0, 0, 0, 8, 8, 8, 4, 0, 0, 0, 1, 1, 0,14, 1, 0, 0, 0, 4},
	{8, 0, 0, 0, 0, 0, 0, 0,11, 0, 8, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 4, 6},
	{8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 6, 0, 0, 0, 0, 0, 6},
	{7, 7, 7, 7, 9, 7, 7, 7, 7, 0, 8, 0, 8, 0, 8, 0, 8, 4, 0, 4, 0, 6, 0, 6},
	{7, 0, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 0, 0, 0, 0, 0, 6},
	{7, 0,16, 7,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 0, 0, 0, 0, 4},
	{7, 0,15, 8,16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 6, 0, 6, 0, 6},
	{7, 0, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 4, 6,10, 6, 6, 6},
	{7, 7, 7, 7, 9, 7, 7, 7, 7, 8, 8, 4,12, 6, 8, 4, 8, 3, 3, 3, 0, 3, 3, 3},
	{2, 2, 2, 2, 0, 2, 0, 0, 0, 4, 6, 4, 0, 0, 6, 0, 6, 3, 0, 0, 0, 0, 0, 3},
	{2, 2, 0, 0, 0, 1, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
	{2, 0, 0, 0, 0, 2, 2, 0, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
	{1, 0, 0, 0, 0, 1, 0, 0, 1, 4, 4, 4, 4, 4, 6, 0, 6, 3, 3, 0, 0, 0, 3, 3},
	{2,15, 0, 0, 0, 2, 0, 0, 2, 2, 2, 1, 2, 2, 2,12, 6, 0, 0, 5, 0, 5, 0, 5},
	{2,16, 0, 0, 0, 2,14, 2, 2,16, 0, 0, 0,15, 2, 0, 0, 0, 0, 5, 0, 0, 5, 5},
	{2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5,12, 5,12, 5, 0, 5, 0, 5},
	{1,12, 2, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0,10, 0, 0, 0, 0, 0, 0, 0, 0, 5},
	{2, 0,11, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5,12, 5,12, 5, 0, 5, 0, 5},
	{2, 2, 1, 0, 0, 0, 0, 2, 2,15, 0, 0, 0,16, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
	{2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5}
};

Sprite sprite[numSprites] =
{
	{20.5, 10.5, 22},
	{20.5, 11.5, 22},
	{18.5,4.5, 22},
	{10.0,4.5, 22},
	{10.0,12.5,22},
	{3.5, 6.5, 22},
	{3.5, 20.5,22},
	{3.5, 14.5,22},
	{14.5,20.5,22},
	{17.5,18.5,22},

	{18.5, 10.5, 21},
	{18.5, 11.5, 21},
	{18.5, 12.5, 21},
	{ 1.5,  9.5, 21},
	{ 5.5,  9.5, 21},

	{21.5, 1.5, 20},
	{15.5, 1.5, 20},
	{16.0, 1.8, 20},
	{16.2, 1.2, 20},
	{3.5,  2.5, 20},
	{9.5, 15.5, 20},
	{10.0, 15.1,20},
	{10.5, 15.8,20},
};

//arrays used to sort the sprites
int spriteOrder[numSprites];
double spriteDistance[numSprites];

//function used to sort the sprites
//sort the sprites based on distance
void sortSprites(int* order, double* dist, int amount)
{
	std::vector<std::pair<double, int>> sprites(amount);
	for (int i = 0; i < amount; i++)
	{
		sprites[i].first = dist[i];
		sprites[i].second = order[i];
	}
	std::sort(sprites.begin(), sprites.end());
	// restore in reverse order to go from farthest to nearest
	for (int i = 0; i < amount; i++)
	{
		dist[i] = sprites[amount - i - 1].first;
		order[i] = sprites[amount - i - 1].second;
	}
}

Door doors[MAX_DOORS];
int numDoors = 0;

PushWall pushWalls[MAX_PUSH_WALLS];
int numPushWalls = 0;

void preProcessMap()
{
	numDoors = 0;
	for (int x = 0; x < mapWidth; x++) {
		for (int y = 0; y < mapHeight; y++) {
			if (worldMap[x][y] == 9) {
				if (numDoors == MAX_DOORS) {
					Error("Too many doors!");
					return;
				}
				Door* door = &doors[numDoors++];
				door->x = x;
				door->y = y;
				door->state = closed;
				door->counter = texWidth;
			}
			else if (worldMap[x][y] == 14) {
				if (numPushWalls == MAX_PUSH_WALLS) {
					Error("Too many push walls!");
					return;
				}
				PushWall* pw = &pushWalls[numPushWalls++];
				pw->x = x;
				pw->y = y;
				pw->state = closed;
				pw->counter = texWidth;
			}
		}
	}
}

Door* findDoor(int x, int y)
{
	for (int i = 0; i < numDoors; i++) {
		Door* door = &doors[i];
		if (door->x == x && door->y == y) {
			return door;
		}
	}
	return NULL;
}

PushWall* findPushWall(int x, int y)
{
	for (int i = 0; i < numPushWalls; i++) {
		PushWall* pw = &pushWalls[i];
		if (pw->x == x && pw->y == y) {
			return pw;
		}
	}
	return NULL;
}

bool wallCanMove(PushWall* pw, Direction dir)
{
	int nx=0, ny=0;
	switch (dir) {
	case dir_N: nx = pw->x; ny = pw->y - 1; break;
	case dir_S: nx = pw->x; ny = pw->y + 1; break;
	case dir_E: nx = pw->x + 1; ny = pw->y; break;
	case dir_W: nx = pw->x - 1; ny = pw->y; break;
	}
	return worldMap[nx][ny] == 0;
}

void updateMap()
{
	for (int i = 0; i < numDoors; i++) {
		Door* door = &doors[i];
		switch (door->state) {
		case opening:
			if (--door->counter == texWidth / 4) {
				door->state = open;
			}
			break;
		case closing:
			if (++door->counter == texWidth) {
				door->state = closed;
			}
			break;
		default: break;
		}
	}
	for (int i = 0; i < numPushWalls; i++) {
		PushWall* pw = &pushWalls[i];
		if (pw->state == opening) {
			int mx, my;
			if (--pw->counter == 0) {
				switch (pw->direction) {
				case dir_N: mx = pw->x; my = pw->y - 1; break;
				case dir_S: mx = pw->x; my = pw->y + 1; break;
				case dir_E: mx = pw->x + 1; my = pw->y; break;
				case dir_W: mx = pw->x - 1; my = pw->y; break;
				}
				pw->counter = texWidth;
				worldMap[pw->x][pw->y] = 0;
				pw->x = mx;
				pw->y = my;
				worldMap[mx][my] = 14;

				if (!wallCanMove(pw, pw->direction))
					pw->state = open;
			}
		}
	}
}

bool canPass(int x, int y)
{
	if (worldMap[x][y] == 9) {
		Door* door = findDoor(x, y);
		return (door->state == open);
	}
	else if (worldMap[x][y] == 10 || worldMap[x][y] == 13)
		return true;
	return worldMap[x][y] == 0;
}

void drawStrip(Strip& strip)
{
	// Draw the vertical strip
	int texY = 0, texCounter = 0;
	int dy = strip.drawEnd - strip.drawStart;

	if (strip.drawStart < 0) {
		texCounter = -strip.drawStart * texHeight;
		if (texCounter > dy) {
			div_t res = div(texCounter, dy);
			texY += res.quot;
			texCounter = res.rem;
		}
		strip.drawStart = 0;
	}
	if (strip.drawEnd >= g_frameHeight)
		strip.drawEnd = (g_frameHeight - 1);

	for (int y = strip.drawStart; y <= strip.drawEnd; y++) {

		unsigned color = strip.texture[texHeight * texY + strip.texX];

		if ((color & 0xFF000000) != 0) {
			//make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
			if (strip.side > 1) color = (color >> 1) & 8355711;
			if (strip.side == 1) color = (color >> 2) & 0x3F3F3F;

#if FOG_LEVEL
			color = ColorLerp(color, FOG_COLOR, strip.fog);
#endif
			if (strip.seeThrough)
				color = ((color & 0xFEFEFE) >> 1) + ((GetPixel(strip.x,y) & 0xFEFEFE) >> 1);

			SetPixel(strip.x, y, color);
		}

		texCounter += texHeight;
		while (texCounter > dy) {
			texY++;
			texCounter -= dy;
		}
	}
}

struct SpritePrepare
{
	int drawStartX, drawEndX;
	int dX, cX, texX;
	int drawStartY, drawEndY;
	int dY, texY0, cY0;
	int texNum;
	double transformY;
	double fog;
};

bool prepsSort(SpritePrepare& i, SpritePrepare& j) { return (i.transformY < j.transformY); }

std::vector<SpritePrepare> spritePrep;

//SPRITE CASTING
void prepareSprites()
{
	spritePrep.clear();

	SpritePrepare prep;

	for (int i = 0; i < numSprites; i++)
	{
		//translate sprite position to relative to camera
		double spriteX = sprite[i].x - posX;
		double spriteY = sprite[i].y - posY;

		//transform sprite with the inverse camera matrix
		// [ planeX   dirX ] -1                                       [ dirY      -dirX ]
		// [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
		// [ planeY   dirY ]                                          [ -planeY  planeX ]

		double invDet = 1.0 / (planeX * dirY - dirX * planeY); //required for correct matrix multiplication

		double transformX = invDet * (dirY * spriteX - dirX * spriteY);
		prep.transformY = invDet * (-planeY * spriteX + planeX * spriteY); //this is actually the depth inside the screen, that what Z is in 3D, the distance of sprite to player, matching sqrt(spriteDistance[i])

		if (prep.transformY < 0) continue;

		int spriteScreenX = int((g_frameWidth / 2) * (1 + transformX / prep.transformY));

		//parameters for scaling and moving the sprites
#define uDiv 2
#define vDiv 2
// Note that vMove is 128 rather than 64 to get the sprites on the ground.
// It's because the textures are 32x32, rather than 64x64 as in the original.
#define vMove 128.0
		int vMoveScreen = int(vMove / prep.transformY);

		//calculate height of the sprite on screen
		int spriteHeight = abs(int(g_frameHeight / (prep.transformY))) / vDiv; //using "transformY" instead of the real distance prevents fisheye
		//calculate lowest and highest pixel to fill in current stripe
		prep.drawStartY = -spriteHeight / 2 + g_frameHeight / 2 + vMoveScreen + lookVert + eyePos / prep.transformY;
		prep.drawEndY = spriteHeight / 2 + g_frameHeight / 2 + vMoveScreen + lookVert + eyePos / prep.transformY;

		//calculate width of the sprite
		int spriteWidth = abs(int(g_frameHeight / (prep.transformY))) / uDiv; // same as height of sprite, given that it's square
		prep.drawStartX = -spriteWidth / 2 + spriteScreenX;
		prep.drawEndX = spriteWidth / 2 + spriteScreenX;

		if (prep.drawStartX >= g_frameWidth || prep.drawEndX < 0) continue;

		// Precompute some variables for the vertical strips
		prep.dY = prep.drawEndY - prep.drawStartY;
		prep.cY0 = 0;
		prep.texY0 = 0;
		if (prep.drawStartY < 0) {
			prep.cY0 = -prep.drawStartY * texHeight;
			if (prep.cY0 > prep.dY) {
				div_t res = div(prep.cY0, prep.dY);
				prep.texY0 += res.quot;
				prep.cY0 = res.rem;
			}
			prep.drawStartY = 0;
		}
		if (prep.drawEndY >= g_frameHeight)
			prep.drawEndY = (g_frameHeight - 1);

		prep.texX = 0;
		prep.dX = prep.drawEndX - prep.drawStartX;
		prep.cX = 0;

		if (prep.drawStartX < 0) {
			prep.cX = -prep.drawStartX * texWidth;
			if (prep.cX > prep.dX) {
				div_t res = div(prep.cX, prep.dX);
				prep.texX += res.quot;
				prep.cX = res.rem;
			}
			prep.drawStartX = 0;
		}
		if (prep.drawEndX > g_frameWidth) prep.drawEndX = g_frameWidth;

#if FOG_LEVEL
		prep.fog = prep.transformY / FOG_CONSTANT * FOG_LEVEL;
#endif

		prep.texNum = sprite[i].texture;
		spritePrep.push_back(prep);
	}

	//sort sprites from far to close
	std::sort(spritePrep.begin(), spritePrep.end(), prepsSort);
}

void drawSpriteStrip(SpritePrepare& prep, int stripe)
{
	if (stripe < prep.drawStartX || stripe >= prep.drawEndX)
		return;

	// If the left side of the sprite is concealed by a wall
	// then we need to adjust texX accordingly.
	int delta = stripe - prep.drawStartX;
	if (delta) {
		prep.drawStartX += delta;
		div_t res = div(prep.cX + delta * texWidth, prep.dX);
		prep.texX += res.quot;
		prep.cX = res.rem;
	}

	int texY = prep.texY0, cY = prep.cY0;
	for (int y = prep.drawStartY; y <= prep.drawEndY; y++) {

		unsigned color = texture[prep.texNum][texWidth * texY + prep.texX]; //get current color from the texture
		if ((color & 0x00FFFFFF) != 0) {
#if FOG_LEVEL
			color = ColorLerp(color, FOG_COLOR, prep.fog);
#endif
			SetPixel(stripe, y, color); //paint pixel if it isn't black, black is the invisible color
		}

		cY = cY + texHeight;
		while (cY > prep.dY) {
			texY++;
			cY -= prep.dY;
		}
	}

	prep.drawStartX++;
	prep.cX += texWidth;
	while (prep.cX > prep.dX) {
		prep.texX++;
		prep.cX -= prep.dX;
	}
}

intersect wallIntersect(double W0x, double W0y, double W1x, double W1y, double Px, double Py, double Dx, double Dy)
{
	intersect i = { -1, -1 };
	double M = (W1y - W0y) / (W1x - W0x); // Note to self: M = -1 or 1 in our case
	if (Dy == M * Dx) return i; // parallel ray
	i.tr = (W0y + M * (Px - W0x) - Py) / (Dy - M * Dx);
	i.tw = (Px + Dx * i.tr - W0x) / (W1x - W0x); // Note to self: W1x - W0x == 1 in our case
	return i;
}

//=============================================================================
bool InitGame()
{
	for (int i = 0; i < 23; i++) texture[i].resize(texWidth * texHeight);

	//load some textures
	unsigned long tw, th, error = 0;
	error |= loadImage(texture[0], tw, th, "data/pics/wall1.png");
	error |= loadImage(texture[1], tw, th, "data/pics/wall2.png");
	error |= loadImage(texture[2], tw, th, "data/pics/wall3.png");
	error |= loadImage(texture[3], tw, th, "data/pics/tile1.png");
	error |= loadImage(texture[4], tw, th, "data/pics/tile2.png");
	error |= loadImage(texture[5], tw, th, "data/pics/tile3.png");
	error |= loadImage(texture[6], tw, th, "data/pics/tile4.png");
	error |= loadImage(texture[7], tw, th, "data/pics/tile5.png");
	error |= loadImage(texture[8], tw, th, "data/pics/door.png");
	error |= loadImage(texture[9], tw, th, "data/pics/entry.png");
	error |= loadImage(texture[10], tw, th, "data/pics/gate.png");
	error |= loadImage(texture[11], tw, th, "data/pics/glass.png");
	error |= loadImage(texture[12], tw, th, "data/pics/glass-break.png");
	error |= loadImage(texture[13], tw, th, "data/pics/secret.png");
	error |= loadImage(texture[14], tw, th, "data/pics/wall5.png");
	error |= loadImage(texture[15], tw, th, "data/pics/wall6.png");
	if (error) { Error("error loading images"); return false; }

	//load some sprite textures
	error |= loadImage(texture[20], tw, th, "data/pics/altar.png");
	error |= loadImage(texture[21], tw, th, "data/pics/fountain.png");
	error |= loadImage(texture[22], tw, th, "data/pics/statue.png");
	if (error) { Error("error loading images"); return false; }

#if SKYBOX
	error |= loadImage(skybox, tw, th, "data/pics/skybox.png");
	if (error) { Error("error loading skybox"); return false; }
#endif

	// Initialize floor tile texture mappings
	// FLOOR_TILE_DEFAULT -> texture 3 (tile1.png)
	// FLOOR_TILE_1 -> texture 3 (tile1.png)
	// FLOOR_TILE_2 -> texture 4 (tile2.png)
	// FLOOR_TILE_3 -> texture 5 (tile3.png)
	// FLOOR_TILE_4 -> texture 6 (tile4.png)
	// FLOOR_TILE_5 -> texture 7 (tile5.png)
	floorTileTextures[FLOOR_TILE_DEFAULT] = 3;
	floorTileTextures[FLOOR_TILE_1] = 3;
	floorTileTextures[FLOOR_TILE_2] = 4;
	floorTileTextures[FLOOR_TILE_3] = 5;
	floorTileTextures[FLOOR_TILE_4] = 6;
	floorTileTextures[FLOOR_TILE_5] = 7;

	// Initialize ceiling type texture mappings
	// CEILING_SKY -> no texture needed (render sky or background)
	// CEILING_SOLID -> texture 6 (tile4.png)
	// CEILING_TEXTURED -> texture 6 (tile4.png)
	ceilingTypeTextures[CEILING_SKY] = 0; // Not used when rendering sky
	ceilingTypeTextures[CEILING_SOLID] = 6;
	ceilingTypeTextures[CEILING_TEXTURED] = 6;

	preProcessMap();

	return true;
}
//=============================================================================
void CloseGame()
{
}
//=============================================================================
void FrameGame()
{
	// ������� ������
	for (int i = 0; i < g_frameWidth * g_frameHeight; i++)
	{
		g_frameBuffer[i] = ColorToUInt(50, 50, 255); // �����-����� ���
		g_depthBuffer[i] = 10000.0f; // �������������
	}

	// Draw skybox and ceiling/floor via helper functions
#if SKYBOX
	RenderSkybox(g_frameWidth, g_frameHeight);
#endif

	// Render floor and ceiling by helper functions
	RenderFloor(g_frameWidth, g_frameHeight);

#if !SKYBOX
	RenderCeiling(g_frameWidth, g_frameHeight);
#endif

	prepareSprites();

	Direction playerDirection;
	if (abs(dirX) > abs(dirY)) {
		if (dirX > 0)
			playerDirection = dir_E;
		else
			playerDirection = dir_W;
	}
	else {
		if (dirY > 0)
			playerDirection = dir_S;
		else
			playerDirection = dir_N;
	}

	// Wall casting and sprite blending
	RenderWalls(g_frameWidth, g_frameHeight);
		
	doorTime += 0.016;
	if (doorTime > 1.0 / texWidth) {
		updateMap();
		doorTime -= 1.0 / texWidth;
	}

	//speed modifiers
	double moveSpeed = 0.016 * 5.0; //the constant value is in squares/second
	double rotSpeed = 0.016 * 3.0; //the constant value is in radians/second

	//move forward if no wall in front of you
	if (keys['W'])
	{
		if (canPass(int(posX + dirX * moveSpeed), int(posY))) posX += dirX * moveSpeed;
		if (canPass(int(posX), int(posY + dirY * moveSpeed))) posY += dirY * moveSpeed;
	}
	//move backwards if no wall behind you
	if (keys['S'])
	{
		if (canPass(int(posX - dirX * moveSpeed), int(posY))) posX -= dirX * moveSpeed;
		if (canPass(int(posX), int(posY - dirY * moveSpeed))) posY -= dirY * moveSpeed;
	}
	//rotate to the right
	if (keys['D'])
	{
		//both camera direction and camera plane must be rotated
		double oldDirX = dirX;
		dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
		dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
		double oldPlaneX = planeX;
		planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
		planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
	}
	//rotate to the left
	if (keys['A'])
	{
		//both camera direction and camera plane must be rotated
		double oldDirX = dirX;
		dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
		dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
		double oldPlaneX = planeX;
		planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
		planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
	}
	if (keys[VK_SPACE])
	{
		int faceX = int(posX + dirX);
		int faceY = int(posY + dirY);
		Door* door = findDoor(faceX, faceY);
		if (door) {
			switch (door->state) {
			case closed: door->state = opening; break;
			case open: door->state = closing; break;
			default: break;
			}
		}
		else {
			PushWall* pw = findPushWall(faceX, faceY);
			if (pw && pw->state == closed && wallCanMove(pw, playerDirection)) {
				pw->direction = playerDirection;
				pw->state = opening;
			}
			else if (worldMap[faceX][faceY] == 12) {
				worldMap[faceX][faceY] = 13;
			}
		}
	}
	if (keys['Q'])
	{
		if (lookVert < LOOK_UP_MAX) {
			lookVert += 0.016 * 600;
			if (lookVert > LOOK_UP_MAX)
				lookVert = LOOK_UP_MAX;
		}
	}
	else if (keys['E'])
	{
		if (lookVert > -LOOK_UP_MAX) {
			lookVert -= 0.016 * 600;
			if (lookVert < -LOOK_UP_MAX)
				lookVert = -LOOK_UP_MAX;
		}
	}
	else
	{
		if (lookVert > 0) {
			lookVert -= 0.016 * 400;
			if (lookVert < 0)
				lookVert = 0;
		}
		else if (lookVert < 0) {
			lookVert += 0.016 * 400;
			if (lookVert > 0)
				lookVert = 0;
		}
	}
	if (keys['X'])
	{
		if (eyePos == 0)
			eyePos = 128;
	}
	else if (keys['C'])
	{
		if (eyePos <= 0 && eyePos > -128)
		{
			eyePos -= 0.016 * 400;
			if (eyePos < -128) {
				eyePos = -128;
			}
		}
	}
	else {
		if (eyePos < 0) {
			eyePos += 0.016 * 400;
			if (eyePos > 0) {
				eyePos = 0;
			}
		}
	}
	if (eyePos > 0) {
		eyePos -= 0.016 * 400;
		if (eyePos < 0) {
			eyePos = 0;
		}
	}
}

//=============================================================================

//=============================================================================
// Helper rendering functions
//=============================================================================
#if SKYBOX
void RenderSkybox(int w, int h)
{
	int texX;
	double rayDirX0 = dirX - planeX;
	double rayDirY0 = dirY - planeY;
	double rayDirX1 = dirX + planeX;
	double rayDirY1 = dirY + planeY;

	int texX0 = (int)(-atan2(rayDirY0, rayDirX0) * (double)SKYBOX_WIDTH / (2 * M_PI) * SKYBOX_REPEATS);
	int texX1 = (int)(-atan2(rayDirY1, rayDirX1) * (double)SKYBOX_WIDTH / (2 * M_PI) * SKYBOX_REPEATS);
	while (texX1 < texX0)
		texX1 += SKYBOX_WIDTH;
	while (texX0 < 0) {
		texX0 += SKYBOX_WIDTH;
		texX1 += SKYBOX_WIDTH;
	}

	int dtexX = texX1 - texX0;
	int dy = h / 2 + lookVert;
	int dtexY = SKYBOX_HEIGHT * (h / 2 + lookVert) / (h / 2 + LOOK_UP_MAX) - 1;
	int texY0 = SKYBOX_HEIGHT - 1 - dtexY;

	for (int x = 0, cX = 0; x < w; x++)
	{
		if (texX0 >= SKYBOX_WIDTH)
		{
			texX = texX0 - SKYBOX_WIDTH;
		}
		else
			texX = texX0;

		for (int y = 0, texY = texY0, cY = 0; y < dy; y++) {

			unsigned color = skybox[SKYBOX_WIDTH * texY + texX];
			SetPixel(x, y, color);

			cY = cY + dtexY;
			while (cY > dy)
			{
				texY = texY + 1;
				cY = cY - dy;
			}
		}

		cX = cX + dtexX;
		while (cX > w)
		{
			texX0 = texX0 + 1;
			cX = cX - w;
		}
	}
}
#endif

// Renders the floor texture using floor casting
void RenderFloor(int w, int h)
{
	for (int y = g_frameHeight / 2 + lookVert + 1, p = 1; y < g_frameHeight; ++y, ++p)
	{
		float rayDirX0 = dirX - planeX;
		float rayDirY0 = dirY - planeY;
		float rayDirX1 = dirX + planeX;
		float rayDirY1 = dirY + planeY;

		float posZ = 0.5 * g_frameHeight;
		float rowDistance = (posZ + eyePos) / p;

		float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / g_frameWidth;
		float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / g_frameWidth;

		float floorX = posX + rowDistance * rayDirX0;
		float floorY = posY + rowDistance * rayDirY0;

#if FOG_LEVEL
		double fog = rowDistance / FOG_CONSTANT * FOG_LEVEL;
#endif

		for (int x = 0; x < g_frameWidth; ++x)
		{
			int cellX = (int)(floorX);
			int cellY = (int)(floorY);

			int tx = (int)(texWidth * (floorX - cellX)) & (texWidth - 1);
			int ty = (int)(texHeight * (floorY - cellY)) & (texHeight - 1);

			floorX += floorStepX;
			floorY += floorStepY;

			// Use floorMap to determine which floor tile texture to use
			if (cellX >= 0 && cellX < mapWidth && cellY >= 0 && cellY < mapHeight) {
				int floorTileType = floorMap[cellX][cellY];
				int floorTexture = floorTileTextures[floorTileType];
				unsigned color = texture[floorTexture][texWidth * ty + tx];
				color = (color >> 1) & 8355711;
#if FOG_LEVEL
				color = ColorLerp(color, FOG_COLOR, fog);
#endif
				SetPixel(x, y, color);
			}
		}
	}
}

// Renders the ceiling texture when skybox is disabled
void RenderCeiling(int w, int h)
{
	for (int y = g_frameHeight / 2 + lookVert + 1, p = 1; y >= 0; y--, ++p)
	{
		float rayDirX0 = dirX - planeX;
		float rayDirY0 = dirY - planeY;
		float rayDirX1 = dirX + planeX;
		float rayDirY1 = dirY + planeY;

		float posZ = 0.5 * g_frameHeight;
		float rowDistance = (posZ - eyePos) / p;

		float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / g_frameWidth;
		float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / g_frameWidth;

		float floorX = posX + rowDistance * rayDirX0;
		float floorY = posY + rowDistance * rayDirY0;

#if FOG_LEVEL
		double fog = rowDistance / FOG_CONSTANT * FOG_LEVEL;
#endif
		for (int x = 0; x < g_frameWidth; ++x)
		{
			int cellX = (int)(floorX);
			int cellY = (int)(floorY);

			int tx = (int)(texWidth * (floorX - cellX)) & (texWidth - 1);
			int ty = (int)(texHeight * (floorY - cellY)) & (texHeight - 1);

			floorX += floorStepX;
			floorY += floorStepY;

			// Use ceilingMap to determine which ceiling type to use
			if (cellX >= 0 && cellX < mapWidth && cellY >= 0 && cellY < mapHeight) {
				int ceilingType = ceilingMap[cellX][cellY];
				
				// Only render ceiling if it's not sky type (CEILING_SKY)
				if (ceilingType != CEILING_SKY) {
					int ceilingTexture = ceilingTypeTextures[ceilingType];
					unsigned color = texture[ceilingTexture][texWidth * ty + tx];
					color = (color >> 1) & 8355711;
#if FOG_LEVEL
					color = ColorLerp(color, FOG_COLOR, fog);
#endif
					SetPixel(x, y, color);
				}
			}
		}
	}
}

// Renders walls and blended sprites per column
void RenderWalls(int w, int h)
{
	const double invW = 2.0 / double(w);
	const double dx = dirX, dy = dirY;
	const double pX = planeX, pY = planeY;
	const int spritePrepCount = (int)spritePrep.size();

	std::stack<Strip> stack;
	for (int x = 0; x < w; x++)
	{
		//calculate ray position and direction
		double cameraX = x * invW - 1.0; //x-coordinate in camera space
		double rayDirX = dx + pX * cameraX;
		double rayDirY = dy + pY * cameraX;

		//which box of the map we're in
		int mapX = int(posX);
		int mapY = int(posY);

		//length of ray from one x or y-side to next x or y-side
		double deltaDistX = (rayDirX == 0.0) ? 1e30 : fabs(1.0 / rayDirX);
		double deltaDistY = (rayDirY == 0.0) ? 1e30 : fabs(1.0 / rayDirY);

		//what direction to step in x or y-direction (either +1 or -1)
		int stepX = (rayDirX < 0.0) ? -1 : 1;
		int stepY = (rayDirY < 0.0) ? -1 : 1;

		//length of ray from current position to next x or y-side
		double sideDistX = (rayDirX < 0.0) 
			? (posX - mapX) * deltaDistX 
			: (mapX + 1.0 - posX) * deltaDistX;
		double sideDistY = (rayDirY < 0.0) 
			? (posY - mapY) * deltaDistY
			: (mapY + 1.0 - posY) * deltaDistY;

		int hit = 0; //was there a wall hit?
		int side = 0; //was a NS or a EW wall hit?

		double perpWallDist = 0.0;
		int tile = 0;

		// Scan ray until we find a wall that is not translucent/transparent
		while (true)
		{
			//perform DDA
			while (hit == 0)
			{
				//jump to next map square, either in x-direction, or in y-direction
				if (sideDistX < sideDistY)
				{
					sideDistX += deltaDistX;
					mapX += stepX;
					side = 0;
				}
				else
				{
					sideDistY += deltaDistY;
					mapY += stepY;
					side = 1;
				}
				//Check if ray has hit a wall
				if (worldMap[mapX][mapY] > 0) hit = 1;
			}

			//texturing calculations
			int texNum = worldMap[mapX][mapY] - 1; //1 subtracted from it so that texture 0 can be used!

			double wallX = 0.0; //where exactly the wall was hit
			bool diag = false;
			Door* door = NULL;
			PushWall* pw = NULL;
			// Sunken wall (door/entry/gate/glass) -> special distance logic
			if (texNum == 8 || texNum == 9 || texNum == 10 || texNum == 11 || texNum == 12) {
				/* Sunken wall encountered */
				if (texNum == 8)
					door = findDoor(mapX, mapY); /* Door encountered */
				if (side == 0)
				{
					double dist = sideDistX - deltaDistX * 0.5;
					if (sideDistY < dist)
					{
						hit = 0;
						continue;
					}
					perpWallDist = dist;
				}
				else
				{
					double dist = sideDistY - deltaDistY * 0.5;
					if (sideDistX < dist)
					{
						hit = 0;
						continue;
					}
					perpWallDist = dist;
				}
			}
			// Secret push wall similar handling
			else if (texNum == 13 && (pw = findPushWall(mapX, mapY)))
			{
				/* Secret push wall encountered */
				if (side == 0)
				{
					double dist = sideDistX - deltaDistX * (double)pw->counter / texWidth;
					if (sideDistY < dist)
					{
						hit = 0;
						continue;
					}
					perpWallDist = dist;
				}
				else
				{
					double dist = sideDistY - deltaDistY * (double)pw->counter / texWidth;
					if (sideDistX < dist)
					{
						hit = 0;
						continue;
					}
					perpWallDist = dist;
				}
			}
			// Diagonal wall calculation
			else if (texNum == 14 || texNum == 15)
			{
				/* Diagonal wall */
				intersect i;
				double d;
				if (texNum == 14) {
					i = wallIntersect(mapX, mapY, mapX + 1, mapY + 1, posX, posY, rayDirX, rayDirY);
					d = posX - mapX - posY + mapY;
				}
				else {
					i = wallIntersect(mapX, mapY + 1, mapX + 1, mapY, posX, posY, rayDirX, rayDirY);
					d = mapX - posX - posY + mapY + 1;
				}
				if (i.tw < 0.0 || i.tw >= 1.0) {
					hit = 0;
					continue;
				}
				perpWallDist = i.tr;
				wallX = i.tw;
				if (d < 0) wallX = 1.0 - wallX;
				diag = true;
				side = 3;
			}
			else
			{
				// Regular wall
				perpWallDist = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);
			}

			int lineHeight = (int)(h / perpWallDist);

			int drawStart = -lineHeight / 2 + h / 2 + lookVert + eyePos / perpWallDist;
			int drawEnd = lineHeight / 2 + h / 2 + lookVert + eyePos / perpWallDist;

			if (!diag) {
				if (side == 0) wallX = posY + perpWallDist * rayDirY;
				else           wallX = posX + perpWallDist * rayDirX;
				wallX -= floor((wallX));
			}

			int texX = int(wallX * double(texWidth));

			if (door)
			{
				texX -= texWidth - door->counter;
				if (texX < 0)
				{
					hit = 0;
					continue;
				}
			}

			if (side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
			if (side == 1 && rayDirY < 0) texX = texWidth - texX - 1;

#if FOG_LEVEL
			double fog = perpWallDist / FOG_CONSTANT * FOG_LEVEL;
#else
			double fog = 0.0;
#endif

			Strip strip = { x, drawStart, drawEnd, perpWallDist, texture[texNum], texX, fog, side,
			  texNum == 11 || texNum == 12 };
			stack.push(strip);

			// Transparent walls (glass/entries/gates) should allow rays to continue
			if (texNum == 9 || texNum == 10 || texNum == 11 || texNum == 12) {
				hit = 0;
				continue;
			}

			// If we arrived here we hit a solid wall and should stop scanning
			break;
		}
		
		// Render sprites and solid walls from back to front
		int farSprite = spritePrepCount - 1;

		while (farSprite >= 0 && spritePrep[farSprite].transformY > perpWallDist)
			farSprite--;

		while (!stack.empty()) {
			Strip& strip = stack.top();

			while (farSprite >= 0 && spritePrep[farSprite].transformY > strip.perpWallDist) {
				drawSpriteStrip(spritePrep[farSprite--], x);
			}

			drawStrip(strip);
			stack.pop();
		}

		while (farSprite >= 0)
			drawSpriteStrip(spritePrep[farSprite--], x);
	}
}