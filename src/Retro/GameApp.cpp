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

// Initialize ray casting context for a column
/**
 * Fill the ray context for column `x`.
 * - x: column index (0..w-1)
 * - w: screen width
 * - ctx: output RayContext
 */
void InitRayContext(int x, int w, RayContext& ctx)
{
	double cameraX = 2 * x / double(w) - 1; // x-coordinate in camera space
	ctx.rayDirX = dirX + planeX * cameraX;
	ctx.rayDirY = dirY + planeY * cameraX;
	ctx.mapX = int(posX);
	ctx.mapY = int(posY);
	ctx.deltaDistX = (ctx.rayDirX == 0) ? 1e30 : std::abs(1 / ctx.rayDirX);
	ctx.deltaDistY = (ctx.rayDirY == 0) ? 1e30 : std::abs(1 / ctx.rayDirY);
	if (ctx.rayDirX < 0) { ctx.stepX = -1; ctx.sideDistX = (posX - ctx.mapX) * ctx.deltaDistX; }
	else { ctx.stepX = 1; ctx.sideDistX = (ctx.mapX + 1.0 - posX) * ctx.deltaDistX; }
	if (ctx.rayDirY < 0) { ctx.stepY = -1; ctx.sideDistY = (posY - ctx.mapY) * ctx.deltaDistY; }
	else { ctx.stepY = 1; ctx.sideDistY = (ctx.mapY + 1.0 - posY) * ctx.deltaDistY; }
}

// Step along the ray until hitting a non-empty map cell
void NextMapHit(RayContext& ctx, int& outMapX, int& outMapY, int& outSide)
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
HitInfo ComputeHitInfo(RayContext& ctx, int hitMapX, int hitMapY, int side)
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
void FrameGame()
{
	// Очищаем буферы
	for (int i = 0; i < g_frameWidth * g_frameHeight; i++)
	{
		g_frameBuffer[i] = ColorToUInt(50, 50, 255); // Темно-синий фон
		g_depthBuffer[i] = 10000.0f; // Бесконечность
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

			int checkerBoardPattern = (int(cellX + cellY)) & 1;
			int floorTexture = (checkerBoardPattern == 0) ? 3 : 4;

			unsigned color = texture[floorTexture][texWidth * ty + tx];
			color = (color >> 1) & 8355711;
#if FOG_LEVEL
			color = ColorLerp(color, FOG_COLOR, fog);
#endif
			SetPixel(x, y, color);
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

			int ceilingTexture = 6;
			unsigned color = texture[ceilingTexture][texWidth * ty + tx];
			color = (color >> 1) & 8355711;
#if FOG_LEVEL
			color = ColorLerp(color, FOG_COLOR, fog);
#endif
			SetPixel(x, y, color);
		}
	}
}

// Renders walls and blended sprites per column
void RenderWalls(int w, int h)
{
#if 0
	for (int x = 0; x < w; x++)
	{
		std::vector<Strip> strips = GatherStripsForColumn(x, w, h);
		ComposeColumn(x, h, strips);
	}
#else
	std::stack<Strip> stack;
	for (int x = 0; x < w; x++)
	{
		//calculate ray position and direction
		double cameraX = 2 * x / double(w) - 1; //x-coordinate in camera space
		double rayDirX = dirX + planeX * cameraX;
		double rayDirY = dirY + planeY * cameraX;

		//which box of the map we're in
		int mapX = int(posX);
		int mapY = int(posY);

		//length of ray from current position to next x or y-side
		double sideDistX;
		double sideDistY;

		//length of ray from one x or y-side to next x or y-side
		double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
		double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);
		double perpWallDist;

		//what direction to step in x or y-direction (either +1 or -1)
		int stepX;
		int stepY;

		int hit = 0; //was there a wall hit?
		int side; //was a NS or a EW wall hit?

		//calculate step and initial sideDist
		if (rayDirX < 0)
		{
			stepX = -1;
			sideDistX = (posX - mapX) * deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (mapX + 1.0 - posX) * deltaDistX;
		}
		if (rayDirY < 0)
		{
			stepY = -1;
			sideDistY = (posY - mapY) * deltaDistY;
		}
		else
		{
			stepY = 1;
			sideDistY = (mapY + 1.0 - posY) * deltaDistY;
		}

	rayscan:
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

		double wallX; //where exactly the wall was hit
		bool diag = false;
		Door* door = NULL;
		PushWall* pw = NULL;
		if (texNum == 8 || texNum == 9 || texNum == 10 || texNum == 11 || texNum == 12) {
			/* Sunken wall encountered */
			if (texNum == 8)
				door = findDoor(mapX, mapY); /* Door encountered */
			if (side == 0) {
				double dist = sideDistX - deltaDistX * 0.5;
				if (sideDistY < dist) {
					hit = 0;
					goto rayscan;
				}
				perpWallDist = dist;
			}
			else {
				double dist = sideDistY - deltaDistY * 0.5;
				if (sideDistX < dist) {
					hit = 0;
					goto rayscan;
				}
				perpWallDist = dist;
			}
		}
		else if (texNum == 13 && (pw = findPushWall(mapX, mapY))) {
			/* Secret push wall encountered */
			if (side == 0) {
				double dist = sideDistX - deltaDistX * (double)pw->counter / texWidth;
				if (sideDistY < dist) {
					hit = 0;
					goto rayscan;
				}
				perpWallDist = dist;
			}
			else {
				double dist = sideDistY - deltaDistY * (double)pw->counter / texWidth;
				if (sideDistX < dist) {
					hit = 0;
					goto rayscan;
				}
				perpWallDist = dist;
			}
		}
		else if (texNum == 14 || texNum == 15) {
			/* Diagonal wall */
			struct intersect i;
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
				goto rayscan;
			}
			perpWallDist = i.tr;
			wallX = i.tw;
			if (d < 0) wallX = 1.0 - wallX;
			diag = true;
			side = 3;
		}
		else {
			if (side == 0) perpWallDist = (sideDistX - deltaDistX);
			else          perpWallDist = (sideDistY - deltaDistY);
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

		if (door) {
			texX -= texWidth - door->counter;
			if (texX < 0) {
				hit = 0;
				goto rayscan;
			}
		}

		if (side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
		if (side == 1 && rayDirY < 0) texX = texWidth - texX - 1;

#if FOG_LEVEL
		double fog = perpWallDist / FOG_CONSTANT * FOG_LEVEL;
#endif

		Strip strip = { x, drawStart, drawEnd, perpWallDist, texture[texNum], texX, fog, side,
		  texNum == 11 || texNum == 12 };
		stack.push(strip);

		if (texNum == 9 || texNum == 10 || texNum == 11 || texNum == 12) {
			hit = 0;
			goto rayscan;
		}

		int farSprite = spritePrep.size() - 1;

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
#endif
}

// GatherStripsForColumn initializes the ray and collects all visible strips for the column x by calling CastRayToHit.
std::vector<Strip> GatherStripsForColumn(int x, int w, int h)
{
	RayContext ctx;
	InitRayContext(x, w, ctx);
	std::vector<Strip> strips;
	CastRayToHit(ctx, x, h, strips);
	return strips;
}

// CastRayToHit performs the classic DDA scanning loop, building strips for
// each valid hit. It will push accepted strips into the provided vector.
void CastRayToHit(RayContext& ctx, int x, int h, std::vector<Strip>& strips)
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

// Build a Strip structure from the computed HitInfo. Returns false when the
// strip should be ignored and the scan continued (matching old "goto rayscan").
std::pair<bool, Strip> BuildStripForHit(const HitInfo& info, RayContext& ctx, int x, int h)
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
		if (texX < 0) return std::make_pair(false, Strip{ 0,0,0,0,texture[0],0,0,0,false });
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

// ComposeColumn composes wall strips and sprites: it draws walls from nearest to farthest, blending sprites in between.
void ComposeColumn(int x, int h, const std::vector<Strip>& strips)
{
	// push strips on stack so drawing order preserves nearest-first semantics
	std::stack<Strip> stack;
	for (const Strip& s : strips) stack.push(s);

	// find closest wall distance (top of stack)
	int farSprite = spritePrep.size() - 1;
	double farthestWall = 1e30;
	if (!stack.empty()) farthestWall = stack.top().perpWallDist;
	while (farSprite >= 0 && spritePrep[farSprite].transformY > farthestWall)
		farSprite--;

	while (!stack.empty()) {
		const Strip& strip = stack.top();
		while (farSprite >= 0 && spritePrep[farSprite].transformY > strip.perpWallDist) {
			drawSpriteStrip(spritePrep[farSprite--], x);
		}
		// need a const_cast because drawStrip wants a non-const reference
		Strip s = strip;
		drawStrip(s);
		stack.pop();
	}
	while (farSprite >= 0)
		drawSpriteStrip(spritePrep[farSprite--], x);
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
bool HandleSunkenWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side)
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
bool HandlePushWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side)
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
bool HandleDiagonalWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side)
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
bool HandleRegularWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side)
{
	// Any map cell that isn't a sunken door, push wall, or diagonal falls here
	if (info.texNum == 8 || info.texNum == 9 || info.texNum == 10 || info.texNum == 11 || info.texNum == 12 || info.texNum == 13 || info.texNum == 14 || info.texNum == 15)
		return false;
	if (side == 0) info.perpWallDist = (ctx.sideDistX - ctx.deltaDistX);
	else           info.perpWallDist = (ctx.sideDistY - ctx.deltaDistY);
	return true;
}