#include "stdafx.h"
#include "GameApp.h"
#include "Engine.h"
#include "Temp.h"
//=============================================================================
extern int g_frameWidth;
extern int g_frameHeight;
extern unsigned* g_frameBuffer;
extern float* g_depthBuffer;
extern double* g_depthBufferX;
extern bool keys[256];


unsigned GetPixel(int x, int y)
{
	if (x < 0 || x >= g_frameWidth || y < 0 || y >= g_frameHeight) return 0;
	return g_frameBuffer[y * g_frameWidth + x];
}

void SetPixel(int x, int y, unsigned color)
{
	if (x < 0 || x >= g_frameWidth || y < 0 || y >= g_frameHeight) return;
	g_frameBuffer[y * g_frameWidth + x] = color;
}

double posX = 22.0, posY = 11.5; //x and y start position
double dirX = -1.0, dirY = 0.0; //initial direction vector
double planeX = 0.0, planeY = 0.66; //the 2d raycaster version of camera plane
double doorTime = 0;

double lookVert = 0;

double eyePos = 0;

// lookVert should have values between -LOOK_UP_MAX and LOOK_UP_MAX
#define LOOK_UP_MAX 128


#define texWidth 32 // must be power of two
#define texHeight 32 // must be power of two
#define mapWidth 24
#define mapHeight 24

#define SKYBOX 1
#define SKYBOX_WIDTH    320
#define SKYBOX_HEIGHT   200
#define SKYBOX_REPEATS  4

#define FOG_LEVEL 2
#define FOG_COLOR 0xA09EE7
#define FOG_CONSTANT ((mapWidth + mapHeight)/2)

std::vector<unsigned> texture[23];
#if SKYBOX
std::vector<unsigned> skybox{ 320 * 240 };
#endif

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

struct Sprite
{
	double x;
	double y;
	int texture;
};

#define numSprites 23

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
	for (int i = 0; i < amount; i++) {
		sprites[i].first = dist[i];
		sprites[i].second = order[i];
	}
	std::sort(sprites.begin(), sprites.end());
	// restore in reverse order to go from farthest to nearest
	for (int i = 0; i < amount; i++) {
		dist[i] = sprites[amount - i - 1].first;
		order[i] = sprites[amount - i - 1].second;
	}
}

#define MAX_DOORS 20
enum DoorState { closed, opening, open, closing };
enum Direction { dir_N, dir_S, dir_E, dir_W };
struct Door
{
	int x, y;
	int counter;
	DoorState state;
} doors[MAX_DOORS];
int numDoors = 0;

#define MAX_PUSH_WALLS 20
struct PushWall
{
	int x, y;
	int counter;
	DoorState state;
	Direction direction;
} pushWalls[MAX_PUSH_WALLS];
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
	int nx, ny;
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

#if FOG_LEVEL
unsigned color_lerp(unsigned color1, unsigned color2, double t)
{

	if (t < 0)
		return color1;
	if (t >= 1)
		return color2;

	unsigned r1 = (color1 >> 16) & 0xFF;
	unsigned r2 = (color2 >> 16) & 0xFF;

	unsigned g1 = (color1 >> 8) & 0xFF;
	unsigned g2 = (color2 >> 8) & 0xFF;

	unsigned b1 = (color1) & 0xFF;
	unsigned b2 = (color2) & 0xFF;

	r1 = (1 - t) * r1 + t * r2;
	if (r1 > 0xFF) r1 = 0xFF;

	g1 = (1 - t) * g1 + t * g2;
	if (g1 > 0xFF) g1 = 0xFF;

	b1 = (1 - t) * b1 + t * b2;
	if (b1 > 0xFF) b1 = 0xFF;

	return (r1 << 16) + (g1 << 8) + (b1);
}
#endif

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
			color = color_lerp(color, FOG_COLOR, strip.fog);
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

std::vector<SpritePrepare> preps;

//SPRITE CASTING
void prepareSprites()
{

	preps.clear();

	SpritePrepare spritePrep;

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
		spritePrep.transformY = invDet * (-planeY * spriteX + planeX * spriteY); //this is actually the depth inside the screen, that what Z is in 3D, the distance of sprite to player, matching sqrt(spriteDistance[i])

		if (spritePrep.transformY < 0) continue;

		int spriteScreenX = int((g_frameWidth / 2) * (1 + transformX / spritePrep.transformY));

		//parameters for scaling and moving the sprites
#define uDiv 2
#define vDiv 2
// Note that vMove is 128 rather than 64 to get the sprites on the ground.
// It's because the textures are 32x32, rather than 64x64 as in the original.
#define vMove 128.0
		int vMoveScreen = int(vMove / spritePrep.transformY);

		//calculate height of the sprite on screen
		int spriteHeight = abs(int(g_frameHeight / (spritePrep.transformY))) / vDiv; //using "transformY" instead of the real distance prevents fisheye
		//calculate lowest and highest pixel to fill in current stripe
		spritePrep.drawStartY = -spriteHeight / 2 + g_frameHeight / 2 + vMoveScreen + lookVert + eyePos / spritePrep.transformY;
		spritePrep.drawEndY = spriteHeight / 2 + g_frameHeight / 2 + vMoveScreen + lookVert + eyePos / spritePrep.transformY;

		//calculate width of the sprite
		int spriteWidth = abs(int(g_frameHeight / (spritePrep.transformY))) / uDiv; // same as height of sprite, given that it's square
		spritePrep.drawStartX = -spriteWidth / 2 + spriteScreenX;
		spritePrep.drawEndX = spriteWidth / 2 + spriteScreenX;

		if (spritePrep.drawStartX >= g_frameWidth || spritePrep.drawEndX < 0) continue;

		// Precompute some variables for the vertical strips
		spritePrep.dY = spritePrep.drawEndY - spritePrep.drawStartY;
		spritePrep.cY0 = 0;
		spritePrep.texY0 = 0;
		if (spritePrep.drawStartY < 0) {
			spritePrep.cY0 = -spritePrep.drawStartY * texHeight;
			if (spritePrep.cY0 > spritePrep.dY) {
				div_t res = div(spritePrep.cY0, spritePrep.dY);
				spritePrep.texY0 += res.quot;
				spritePrep.cY0 = res.rem;
			}
			spritePrep.drawStartY = 0;
		}
		if (spritePrep.drawEndY >= g_frameHeight)
			spritePrep.drawEndY = (g_frameHeight - 1);

		spritePrep.texX = 0;
		spritePrep.dX = spritePrep.drawEndX - spritePrep.drawStartX;
		spritePrep.cX = 0;

		if (spritePrep.drawStartX < 0) {
			spritePrep.cX = -spritePrep.drawStartX * texWidth;
			if (spritePrep.cX > spritePrep.dX) {
				div_t res = div(spritePrep.cX, spritePrep.dX);
				spritePrep.texX += res.quot;
				spritePrep.cX = res.rem;
			}
			spritePrep.drawStartX = 0;
		}
		if (spritePrep.drawEndX > g_frameWidth) spritePrep.drawEndX = g_frameWidth;

#if FOG_LEVEL
		spritePrep.fog = spritePrep.transformY / FOG_CONSTANT * FOG_LEVEL;
#endif

		spritePrep.texNum = sprite[i].texture;
		preps.push_back(spritePrep);
	}

	//sort sprites from far to close
	std::sort(preps.begin(), preps.end(), prepsSort);
}

void drawSpriteStrip(SpritePrepare& spritePrep, int stripe)
{

	if (stripe < spritePrep.drawStartX || stripe >= spritePrep.drawEndX)
		return;

	// If the left side of the sprite is concealed by a wall
	// then we need to adjust texX accordingly.
	int delta = stripe - spritePrep.drawStartX;
	if (delta) {
		spritePrep.drawStartX += delta;
		div_t res = div(spritePrep.cX + delta * texWidth, spritePrep.dX);
		spritePrep.texX += res.quot;
		spritePrep.cX = res.rem;
	}

	int texY = spritePrep.texY0, cY = spritePrep.cY0;
	for (int y = spritePrep.drawStartY; y <= spritePrep.drawEndY; y++) {

		unsigned color = texture[spritePrep.texNum][texWidth * texY + spritePrep.texX]; //get current color from the texture
		if ((color & 0x00FFFFFF) != 0) {
#if FOG_LEVEL
			color = color_lerp(color, FOG_COLOR, spritePrep.fog);
#endif
			SetPixel(stripe, y, color); //paint pixel if it isn't black, black is the invisible color
		}

		cY = cY + texHeight;
		while (cY > spritePrep.dY) {
			texY++;
			cY -= spritePrep.dY;
		}
	}

	spritePrep.drawStartX++;
	spritePrep.cX += texWidth;
	while (spritePrep.cX > spritePrep.dX) {
		spritePrep.texX++;
		spritePrep.cX -= spritePrep.dX;
	}
}

struct intersect
{
	double tr, tw;
};

struct intersect wallIntersect(double W0x, double W0y, double W1x, double W1y,
	double Px, double Py, double Dx, double Dy)
{
	struct intersect i = { -1, -1 };
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

	preProcessMap();

	return true;
}
//=============================================================================
void CloseGame()
{
}
//=============================================================================
// Helper prototypes
static void RenderSkybox(int w, int h);
static void RenderFloor(int w, int h);
static void RenderCeiling(int w, int h);
static void RenderWalls(int w, int h);
// Decomposition helpers for RenderWalls
static void CastRayToHit(RayContext& ctx, int x, int h, std::vector<Strip>& strips);
static std::vector<Strip> GatherStripsForColumn(int x, int w, int h);
static void ComposeColumn(int x, int h, const std::vector<Strip>& strips);

// Ray casting helpers
struct RayContext
{
	double rayDirX, rayDirY;
	int mapX, mapY;
	double sideDistX, sideDistY;
	double deltaDistX, deltaDistY;
	int stepX, stepY;
};

/**
 * HitInfo contains the result of analyzing a single ray-map intersection.
 * - texNum: texture index (map cell - 1)
 * - side: side where ray hit (0 = x-side, 1 = y-side, 3 = diagonal special case)
 * - diag: whether this was a diagonal wall
 * - door: optional pointer to a Door (for texNum==8)
 * - pw: optional pointer to a PushWall (for texNum==13)
 * - perpWallDist: precomputed perpendicular distance to hit
 * - wallX: fractional hit location used for texture mapping
 * - resume: true if caller should continue DDA scanning (i.e. ignore this hit)
 */
struct HitInfo
{
	int texNum;
	int side;
	bool diag;
	Door* door;
	PushWall* pw;
	double perpWallDist;
	double wallX;
	bool resume; // whether we should continue scanning for a further hit
};

static void InitRayContext(int x, int w, RayContext& ctx);
static void NextMapHit(RayContext& ctx, int& outMapX, int& outMapY, int& outSide);
// ComputeHitInfo orchestrates specialized handlers to decode what we hit.
// It returns a HitInfo describing the type of wall, its distance, texture,
// and whether the ray must continue scanning (resume). This is a thin
// wrapper dispatching to more specific handlers for clarity.
static HitInfo ComputeHitInfo(RayContext& ctx, int hitMapX, int hitMapY, int side);

// Specific hit handlers: they fill 'info' and return true when the hit
// should be accepted, or false when the caller should continue scanning.
static bool HandleSunkenWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side);
static bool HandlePushWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side);
static bool HandleDiagonalWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side);
static bool HandleRegularWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side);
/**
 * BuildStripForHit tries to construct a Strip from computed HitInfo.
 * Returns a pair<accepted, Strip>. If accepted is false, the caller
 * should continue scanning (i.e. the hit was not yet ready to draw).
 */
static std::pair<bool, Strip> BuildStripForHit(const HitInfo& info, RayContext& ctx, int x, int h);

// Initialize ray casting context for a column
/**
 * Fill the ray context for column `x`.
 * - x: column index (0..w-1)
 * - w: screen width
 * - ctx: output RayContext
 */
static void InitRayContext(int x, int w, RayContext& ctx)
{
	double cameraX = 2 * x / double(w) - 1; // x-coordinate in camera space
	ctx.rayDirX = dirX + planeX * cameraX;
	ctx.rayDirY = dirY + planeY * cameraX;
	ctx.mapX = int(posX);
	ctx.mapY = int(posY);
	ctx.deltaDistX = (ctx.rayDirX == 0) ? 1e30 : std::abs(1 / ctx.rayDirX);
	ctx.deltaDistY = (ctx.rayDirY == 0) ? 1e30 : std::abs(1 / ctx.rayDirY);
	if (ctx.rayDirX < 0){ ctx.stepX = -1; ctx.sideDistX = (posX - ctx.mapX) * ctx.deltaDistX; }
	else { ctx.stepX = 1; ctx.sideDistX = (ctx.mapX + 1.0 - posX) * ctx.deltaDistX; }
	if (ctx.rayDirY < 0){ ctx.stepY = -1; ctx.sideDistY = (posY - ctx.mapY) * ctx.deltaDistY; }
	else { ctx.stepY = 1; ctx.sideDistY = (ctx.mapY + 1.0 - posY) * ctx.deltaDistY; }
}

/**
 * Advances the ray via DDA until it hits a non-empty map cell.
 * Returns map coordinates and which side was crossed.
 */
static void NextMapHit(RayContext& ctx, int& outMapX, int& outMapY, int& outSide);

/**
 * ComputeHitInfo orchestrates specific handlers and produces HitInfo.
 * - ctx: input ray context
 * - hitMapX/hitMapY: the map coords of the intersection
 * - side: which side was crossed (0/1)
 */
static HitInfo ComputeHitInfo(RayContext& ctx, int hitMapX, int hitMapY, int side);

// Step along the ray until hitting a non-empty map cell
static void NextMapHit(RayContext& ctx, int& outMapX, int& outMapY, int& outSide)
{
	while (true) {
		if (ctx.sideDistX < ctx.sideDistY) {
			ctx.sideDistX += ctx.deltaDistX;
			ctx.mapX += ctx.stepX;
			outSide = 0;
		}
		else {
			ctx.sideDistY += ctx.deltaDistY;
			ctx.mapY += ctx.stepY;
			outSide = 1;
		}
		if (worldMap[ctx.mapX][ctx.mapY] > 0) {
			outMapX = ctx.mapX;
			outMapY = ctx.mapY;
			return;
		}
	}
}

// Analyze a single hit and compute info needed for rendering.
// ComputeHitInfo dispatches to specialized handlers and returns HitInfo.
static HitInfo ComputeHitInfo(RayContext& ctx, int hitMapX, int hitMapY, int side)
{
	HitInfo info;
	info.texNum = worldMap[hitMapX][hitMapY] - 1;
	info.side = side;
	info.diag = false;
	info.door = NULL;
	info.pw = NULL;
	info.perpWallDist = 0;
	info.wallX = 0;
	info.resume = false;
	// Try specialized handlers; only one will accept the hit
	if (HandleSunkenWall(ctx, info, hitMapX, hitMapY, side))
		return info;
	if (HandlePushWall(ctx, info, hitMapX, hitMapY, side))
		return info;
	if (HandleDiagonalWall(ctx, info, hitMapX, hitMapY, side))
		return info;
	if (HandleRegularWall(ctx, info, hitMapX, hitMapY, side))
		return info;

	// fallback: resume scanning
	info.resume = true;
	return info;
}

void FrameGame()
{
	const int w = g_frameWidth;
	const int h = g_frameHeight;
	
	// ������� ������
	for (int i = 0; i < g_frameWidth * g_frameHeight; i++)
	{
		g_frameBuffer[i] = ColorToUInt(50, 50, 255); // �����-����� ���
		g_depthBuffer[i] = 10000.0f; // �������������
	}

	// Draw skybox and ceiling/floor via helper functions
	#if SKYBOX
	RenderSkybox(w, h);
	#endif

	// Render floor and ceiling by helper functions
	RenderFloor(w, h);

	#if !SKYBOX
	RenderCeiling(w, h);
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
	RenderWalls(w, h);

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
static void RenderSkybox(int w, int h)
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

	for (int x = 0, cX = 0; x < w; x++) {

		if (texX0 >= SKYBOX_WIDTH) {
			texX = texX0 - SKYBOX_WIDTH;
		}
		else
			texX = texX0;

		for (int y = 0, texY = texY0, cY = 0; y < dy; y++) {

			unsigned color = skybox[SKYBOX_WIDTH * texY + texX];
			SetPixel(x, y, color);

			cY = cY + dtexY;
			while (cY > dy) {
				texY = texY + 1;
				cY = cY - dy;
			}
		}

		cX = cX + dtexX;
		while (cX > w) {
			texX0 = texX0 + 1;
			cX = cX - w;
		}
	}
}

// Renders the floor texture using floor casting
static void RenderFloor(int w, int h)
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

			int checkerBoardPattern = (int(cellX + cellY)) & 1;
			int floorTexture = (checkerBoardPattern == 0) ? 3 : 4;

			unsigned color = texture[floorTexture][texWidth * ty + tx];
			color = (color >> 1) & 8355711;
#if FOG_LEVEL
			color = color_lerp(color, FOG_COLOR, fog);
#endif
			SetPixel(x, y, color);
		}
	}
}

// Renders the ceiling texture when skybox is disabled
static void RenderCeiling(int w, int h)
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

			int ceilingTexture = 6;
			unsigned color = texture[ceilingTexture][texWidth * ty + tx];
			color = (color >> 1) & 8355711;
#if FOG_LEVEL
			color = color_lerp(color, FOG_COLOR, fog);
#endif
			SetPixel(x, y, color);
		}
	}
}

// Renders walls and blended sprites per column
static void RenderWalls(int w, int h)
{
	for (int x = 0; x < w; x++)
	{
		std::vector<Strip> strips = GatherStripsForColumn(x, w, h);
		ComposeColumn(x, h, strips);

        
	}
}

// Build a Strip structure from the computed HitInfo. Returns false when the
// strip should be ignored and the scan continued (matching old "goto rayscan").
static std::pair<bool, Strip> BuildStripForHit(const HitInfo& info, RayContext& ctx, int x, int h)
{
	int lineHeight = (int)(h / info.perpWallDist);
	int drawStart = -lineHeight / 2 + h / 2 + lookVert + eyePos / info.perpWallDist;
	int drawEnd = lineHeight / 2 + h / 2 + lookVert + eyePos / info.perpWallDist;

	double wallX = info.wallX;
	if (!info.diag) {
		if (info.side == 0) wallX = posY + info.perpWallDist * ctx.rayDirY;
		else                 wallX = posX + info.perpWallDist * ctx.rayDirX;
		wallX -= floor((wallX));
	}
	int texX = int(wallX * double(texWidth));
	if (info.door) {
		texX -= texWidth - info.door->counter;
		if (texX < 0) return std::make_pair(false, Strip{0,0,0,0,texture[0],0,0,0,false});
	}
	if (info.side == 0 && ctx.rayDirX > 0) texX = texWidth - texX - 1;
	if (info.side == 1 && ctx.rayDirY < 0) texX = texWidth - texX - 1;
#if FOG_LEVEL
	double fog = info.perpWallDist / FOG_CONSTANT * FOG_LEVEL;
#endif
	Strip strip = { x, drawStart, drawEnd, info.perpWallDist, texture[info.texNum], texX, fog, info.side,
	  info.texNum == 11 || info.texNum == 12 };
	return std::make_pair(true, strip);
}

/**
 * HandleSunkenWall handles walls with an offset (sunken) shape which
 * capture doors, windows and other see-through surfaces.
 *
 * Parameters:
 *  - ctx: current RayContext used by the DDA algorithm
 *  - info: HitInfo to populate with distance/flags
 *  - hitMapX/hitMapY: the map cell coordinates where the hit occurred
 *  - side: which axis side was crossed (0=x or 1=y)
 *
 * Returns:
 *  - true: the hit is accepted and the caller can build a Strip
 *  - false: the ray should continue scanning (same as original 'goto rayscan')
 */
static bool HandleSunkenWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side)
{
	if (!(info.texNum == 8 || info.texNum == 9 || info.texNum == 10 || info.texNum == 11 || info.texNum == 12))
		return false;
	if (info.texNum == 8) info.door = findDoor(hitMapX, hitMapY);
	if (side == 0) {
		double dist = ctx.sideDistX - ctx.deltaDistX * 0.5;
		if (ctx.sideDistY < dist) { info.resume = true; return false; }
		info.perpWallDist = dist;
	}
	else {
		double dist = ctx.sideDistY - ctx.deltaDistY * 0.5;
		if (ctx.sideDistX < dist) { info.resume = true; return false; }
		info.perpWallDist = dist;
	}
	return true;
}

/**
 * HandlePushWall processes secret push-walls that may be partially collapsed.
 *
 * The handler sets info.pw and calculates a modified perpendicular distance
 * based on the current push progress stored in the PushWall structure.
 *
 * Returns true if the hit should be accepted, or false to continue scanning.
 */
static bool HandlePushWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side)
{
	if (info.texNum != 13) return false;
	info.pw = findPushWall(hitMapX, hitMapY);
	if (!info.pw) return false;
	if (side == 0) {
		double dist = ctx.sideDistX - ctx.deltaDistX * (double)info.pw->counter / texWidth;
		if (ctx.sideDistY < dist) { info.resume = true; return false; }
		info.perpWallDist = dist;
	}
	else {
		double dist = ctx.sideDistY - ctx.deltaDistY * (double)info.pw->counter / texWidth;
		if (ctx.sideDistX < dist) { info.resume = true; return false; }
		info.perpWallDist = dist;
	}
	return true;
}

/**
 * HandleDiagonalWall resolves diagonal walls with the special intersection
 * logic implemented in wallIntersect().
 *
 * Parameters:
 *  - ctx: ray context currently being traced
 *  - info: HitInfo structure to populate (perpWallDist, wallX, diag)
 *  - hitMapX/hitMapY: the map cell coordinates
 *  - side: crossing side
 *
 * Returns:
 *  - true if the diagonal hit is valid and accepted; false if scanning
 *    should continue.
 */
static bool HandleDiagonalWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side)
{
	if (!(info.texNum == 14 || info.texNum == 15)) return false;
	struct intersect i;
	double d;
	if (info.texNum == 14) {
		i = wallIntersect(hitMapX, hitMapY, hitMapX + 1, hitMapY + 1, posX, posY, ctx.rayDirX, ctx.rayDirY);
		d = posX - hitMapX - posY + hitMapY;
	}
	else {
		i = wallIntersect(hitMapX, hitMapY + 1, hitMapX + 1, hitMapY, posX, posY, ctx.rayDirX, ctx.rayDirY);
		d = hitMapX - posX - posY + hitMapY + 1;
	}
	if (i.tw < 0.0 || i.tw >= 1.0) { info.resume = true; return false; }
	info.perpWallDist = i.tr;
	info.wallX = i.tw;
	if (d < 0) info.wallX = 1.0 - info.wallX;
	info.diag = true;
	info.side = 3;
	return true;
}

/**
 * HandleRegularWall fills `info` for normal axis-aligned walls.
 *
 * Parameters and semantics are similar to other handlers: it computes
 * `info.perpWallDist` and returns true if handled.
 */
static bool HandleRegularWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side)
{
	// Any map cell that isn't a sunken door, push wall, or diagonal falls here
	if (info.texNum == 8 || info.texNum == 9 || info.texNum == 10 || info.texNum == 11 || info.texNum == 12 || info.texNum == 13 || info.texNum == 14 || info.texNum == 15)
		return false;
	if (side == 0) info.perpWallDist = (ctx.sideDistX - ctx.deltaDistX);
	else           info.perpWallDist = (ctx.sideDistY - ctx.deltaDistY);
	return true;
}

// CastRayToHit performs the classic DDA scanning loop, building strips for
// each valid hit. It will push accepted strips into the provided vector.
static void CastRayToHit(RayContext& ctx, int x, int h, std::vector<Strip>& strips)
{
	while (true) {
		int hitMapX, hitMapY, side;
		NextMapHit(ctx, hitMapX, hitMapY, side);
		HitInfo info = ComputeHitInfo(ctx, hitMapX, hitMapY, side);
		if (info.resume) continue;

		auto buildRes = BuildStripForHit(info, ctx, x, h);
		if (!buildRes.first) continue;
		strips.push_back(buildRes.second);

		if (info.texNum == 9 || info.texNum == 10 || info.texNum == 11 || info.texNum == 12) {
			// transparent or fancy wall — continue scanning
			continue;
		}
		break; // nearest opaque or final wall
	}
}

// GatherStripsForColumn initializes the ray and collects all visible strips
// for the column x by calling CastRayToHit.
static std::vector<Strip> GatherStripsForColumn(int x, int w, int h)
{
	RayContext ctx;
	InitRayContext(x, w, ctx);
	std::vector<Strip> strips;
	CastRayToHit(ctx, x, h, strips);
	return strips;
}

// ComposeColumn composes wall strips and sprites: it draws walls from
// nearest to farthest, blending sprites in between.
static void ComposeColumn(int x, int h, const std::vector<Strip>& strips)
{
	// push strips on stack so drawing order preserves nearest-first semantics
	std::stack<Strip> stack;
	for (const Strip& s : strips) stack.push(s);

	// find closest wall distance (top of stack)
	int farSprite = preps.size() - 1;
	double farthestWall = 1e30;
	if (!stack.empty()) farthestWall = stack.top().perpWallDist;
	while (farSprite >= 0 && preps[farSprite].transformY > farthestWall)
		farSprite--;

	while (!stack.empty()) {
		const Strip& strip = stack.top();
		while (farSprite >= 0 && preps[farSprite].transformY > strip.perpWallDist) {
			drawSpriteStrip(preps[farSprite--], x);
		}
		// need a const_cast because drawStrip wants a non-const reference
		Strip s = strip;
		drawStrip(s);
		stack.pop();
	}
	while (farSprite >= 0)
		drawSpriteStrip(preps[farSprite--], x);
}