#include "stdafx.h"
#include "GameApp.h"
#include "Engine.h"
//=============================================================================
extern int g_frameWidth;
extern int g_frameHeight;
extern unsigned* g_frameBuffer;
extern float* g_depthBuffer;
extern bool keys[256];

const float FOV = 60.0f; // Угол обзора в градусах

//std::vector<Texture> textures;
//std::vector<SectorO> sectors;
//Camera camera(Vector3D(0, 0, 0), 0);

#define numSect 4
#define numWall 16

struct
{
	float cos[360];
	float sin[360];
} math;

struct
{
	int x, y, z;             //player position. Z is up
	int a;                 //player angle of rotation left right
	int l;                 //variable to look up and down
} player;

struct Wall
{
	int x1, y1;             //bottom line point 1
	int x2, y2;             //bottom line point 2
	int c;                 //wall color
};
Wall walls[30];

struct Sector
{
	Sector()
	{
		// temp
		size_t s = (480 * (float)1600 / (float)900);
		surf = new int[s];
	}

	int ws, we;             //wall number start and end
	int z1, z2;             //height of bottom and top 
	int d;                 //add y distances to sort drawing order
	int c1, c2;             //bottom and top color
	int *surf;       //to hold points for surfaces
	int surface;          //is there a surfaces to draw
};
Sector sectors[30];

void drawPixel(int x, int y, int c)
{
	Color clr;
	if (c == 0) { clr.r = 255; clr.g = 255; clr.b = 0; } // Yellow
	if (c == 1) { clr.r = 160; clr.g = 160; clr.b = 0; } // Yellow darker
	if (c == 2) { clr.r = 0; clr.g = 255; clr.b = 0; } // Green
	if (c == 3) { clr.r = 0; clr.g = 160; clr.b = 0; } // Green darker
	if (c == 4) { clr.r = 0; clr.g = 255; clr.b = 255; } // Cyan
	if (c == 5) { clr.r = 0; clr.g = 160; clr.b = 160; } // Cyan darker
	if (c == 6) { clr.r = 160; clr.g = 100; clr.b = 0; } // Brown
	if (c == 7) { clr.r = 110; clr.g = 50; clr.b = 0; } // Brown darker
	if (c == 8) { clr.r = 0; clr.g = 60; clr.b = 130; } // background

	if (x < 0 || x >= g_frameWidth || y < 0 || y >= g_frameHeight)
		return;
	g_frameBuffer[y * g_frameWidth + x] = ColorToUInt(clr);
}
//=============================================================================
void movePlayer()
{
	//move up, down, left, right
	if (keys['A']) { player.a -= 4; if (player.a < 0) { player.a += 360; } }
	if (keys['D']) { player.a += 4; if (player.a > 359) { player.a -= 360; } }
	int dx = math.sin[player.a] * 10.0;
	int dy = math.cos[player.a] * 10.0;
	if (keys['W']) { player.x += dx; player.y += dy; }
	if (keys['S']) { player.x -= dx; player.y -= dy; }
	//strafe left, right
	if (keys['Q']) { player.x += dy; player.y -= dx; }
	if (keys['E']) { player.x -= dy; player.y += dx; }
	////move up, down, look up, look down
	if (keys[VK_LEFT]) { player.l -= 1; }
	if (keys[VK_RIGHT]) { player.l += 1; }
	if (keys[VK_UP]) { player.z -= 4; }
	if (keys[VK_DOWN]) { player.z += 4; }
}
//=============================================================================
void clearBackground()
{
	int x, y;
	for (y = 0; y < g_frameHeight; y++)
	{
		for (x = 0; x < g_frameWidth; x++) 
		{ 
			drawPixel(x, y, 8);
			g_depthBuffer[x*y] = 10000.0f; // Бесконечность
		}
	}
}
void clipBehindPlayer(int* x1, int* y1, int* z1, int x2, int y2, int z2) //clip line
{
	float da = *y1;                                 //distance plane -> point a
	float db = y2;                                 //distance plane -> point b
	float d = da - db; if (d == 0) { d = 1; }
	float s = da / (da - db);                         //intersection factor (between 0 and 1)
	*x1 = *x1 + s * (x2 - (*x1));
	*y1 = *y1 + s * (y2 - (*y1)); if (*y1 == 0) { *y1 = 1; } //prevent divide by zero 
	*z1 = *z1 + s * (z2 - (*z1));
}

void drawWall(int x1, int x2, int b1, int b2, int t1, int t2, int c, int s)
{
	int x, y;
	//Hold difference in distance
	int dyb = b2 - b1;                       //y distance of bottom line
	int dyt = t2 - t1;                       //y distance of top    line
	int dx = x2 - x1; if (dx == 0) { dx = 1; }    //x distance
	int xs = x1;                              //hold initial x1 starting position 
	//CLIP X
	if (x1 < 1) { x1 = 1; } //clip left
	if (x2 < 1) { x2 = 1; } //clip left
	if (x1 > g_frameWidth - 1) { x1 = g_frameWidth - 1; } //clip right
	if (x2 > g_frameWidth - 1) { x2 = g_frameWidth - 1; } //clip right
	//draw x verticle lines
	for (x = x1; x < x2; x++)
	{
		//The Y start and end point
		int y1 = dyb * (x - xs + 0.5) / dx + b1; //y bottom point
		int y2 = dyt * (x - xs + 0.5) / dx + t1; //y bottom point
		//Clip Y
		if (y1 < 1) { y1 = 1; } //clip y 
		if (y2 < 1) { y2 = 1; } //clip y 
		if (y1 > g_frameHeight - 1) { y1 = g_frameHeight - 1; } //clip y 
		if (y2 > g_frameHeight - 1) { y2 = g_frameHeight - 1; } //clip y 
		//surface
		if (sectors[s].surface == 1) { sectors[s].surf[x] = y1; continue; } //save bottom points
		if (sectors[s].surface == 2) { sectors[s].surf[x] = y2; continue; } //save top    points
		if (sectors[s].surface == -1) { for (y = sectors[s].surf[x]; y < y1; y++) { drawPixel(x, y, sectors[s].c1); }; } //bottom
		if (sectors[s].surface == -2) { for (y = y2; y < sectors[s].surf[x]; y++) { drawPixel(x, y, sectors[s].c2); }; } //top
		for (y = y1; y < y2; y++) { drawPixel(x, y, c); } //normal wall
	}
}

int dist(int x1, int y1, int x2, int y2)
{
	int distance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	return distance;
}

void draw3D()
{
	int SW2 = g_frameWidth / 2; //screen width  divided by 2
	int SH2 = g_frameHeight / 2; //screen height divided by 2

	int s, w, loop, wx[4], wy[4], wz[4]; float CS = math.cos[player.a], SN = math.sin[player.a];
	//order sectors by distance
	for (s = 0; s < numSect - 1; s++)
	{
		for (w = 0; w < numSect - s - 1; w++)
		{
			if (sectors[w].d < sectors[w + 1].d)
			{
				Sector st = sectors[w]; sectors[w] = sectors[w + 1]; sectors[w + 1] = st;
			}
		}
	}

	//draw sectors
	for (s = 0; s < numSect; s++)
	{
		sectors[s].d = 0; //clear distance
		if (player.z < sectors[s].z1) { sectors[s].surface = 1; }  //bottom surface
		else if (player.z > sectors[s].z2) { sectors[s].surface = 2; }  //top    surface
		else { sectors[s].surface = 0; }  //no     surfaces
		for (loop = 0; loop < 2; loop++)
		{
			for (w = sectors[s].ws; w < sectors[s].we; w++)
			{
				//offset bottom 2 points by player
				int x1 = walls[w].x1 - player.x, y1 = walls[w].y1 - player.y;
				int x2 = walls[w].x2 - player.x, y2 = walls[w].y2 - player.y;
				//swap for surface
				if (loop == 0) { int swp = x1; x1 = x2; x2 = swp; swp = y1; y1 = y2; y2 = swp; }
				//world X position 
				wx[0] = x1 * CS - y1 * SN;
				wx[1] = x2 * CS - y2 * SN;
				wx[2] = wx[0];                          //top line has the same x
				wx[3] = wx[1];
				//world Y position (depth)
				wy[0] = y1 * CS + x1 * SN;
				wy[1] = y2 * CS + x2 * SN;
				wy[2] = wy[0];                          //top line has the same y 
				wy[3] = wy[1];
				sectors[s].d += dist(0, 0, (wx[0] + wx[1]) / 2, (wy[0] + wy[1]) / 2);  //store this wall distance
				//world z height
				wz[0] = sectors[s].z1 - player.z + ((player.l * wy[0]) / 32.0);
				wz[1] = sectors[s].z1 - player.z + ((player.l * wy[1]) / 32.0);
				wz[2] = wz[0] + sectors[s].z2;                       //top line has new z 
				wz[3] = wz[1] + sectors[s].z2;
				//dont draw if behind player
				if (wy[0] < 1 && wy[1] < 1) { continue; }      //wall behind player, dont draw
				//point 1 behind player, clip
				if (wy[0] < 1)
				{
					clipBehindPlayer(&wx[0], &wy[0], &wz[0], wx[1], wy[1], wz[1]); //bottom line
					clipBehindPlayer(&wx[2], &wy[2], &wz[2], wx[3], wy[3], wz[3]); //top line
				}
				//point 2 behind player, clip
				if (wy[1] < 1)
				{
					clipBehindPlayer(&wx[1], &wy[1], &wz[1], wx[0], wy[0], wz[0]); //bottom line
					clipBehindPlayer(&wx[3], &wy[3], &wz[3], wx[2], wy[2], wz[2]); //top line
				}
				//screen x, screen y position
				wx[0] = wx[0] * 200 / wy[0] + SW2; wy[0] = wz[0] * 200 / wy[0] + SH2;
				wx[1] = wx[1] * 200 / wy[1] + SW2; wy[1] = wz[1] * 200 / wy[1] + SH2;
				wx[2] = wx[2] * 200 / wy[2] + SW2; wy[2] = wz[2] * 200 / wy[2] + SH2;
				wx[3] = wx[3] * 200 / wy[3] + SW2; wy[3] = wz[3] * 200 / wy[3] + SH2;
				//draw points
				drawWall(wx[0], wx[1], wy[0], wy[1], wy[2], wy[3], walls[w].c, s);
			}
			sectors[s].d /= (sectors[s].we - sectors[s].ws); //find average sector distance
			sectors[s].surface *= -1;          //flip to negative to draw surface
		}
	}
}

int loadSectors[] =
{//wall start, wall end, z1 height, z2 height, bottom color, top color
 0,  4, 0, 40, 2,3, //sector 1
 4,  8, 0, 40, 4,5, //sector 2
 8, 12, 0, 40, 6,7, //sector 3
 12,16, 0, 40, 0,1, //sector 4
};

int loadWalls[] =
{//x1,y1, x2,y2, color
  0, 0, 32, 0, 0,
 32, 0, 32,32, 1,
 32,32,  0,32, 0,
  0,32,  0, 0, 1,

 64, 0, 96, 0, 2,
 96, 0, 96,32, 3,
 96,32, 64,32, 2,
 64,32, 64, 0, 3,

 64, 64, 96, 64, 4,
 96, 64, 96, 96, 5,
 96, 96, 64, 96, 4,
 64, 96, 64, 64, 5,

  0, 64, 32, 64, 6,
 32, 64, 32, 96, 7,
 32, 96,  0, 96, 6,
  0, 96,  0, 64, 7,
};
//=============================================================================
bool InitGame()
{
	int x;
	//store sin/cos in degrees
	for (x = 0; x < 360; x++)                         //precalulate sin cos in degrees
	{
		math.cos[x] = cos(x / 180.0 * M_PI);
		math.sin[x] = sin(x / 180.0 * M_PI);
	}
	//init player
	player.x = 70; player.y = -110; player.z = 20; player.a = 0; player.l = 0;    //init player variables
	//load sectors
	int s, w, v1 = 0, v2 = 0;
	for (s = 0; s < numSect; s++)
	{
		sectors[s].ws = loadSectors[v1 + 0];                   //wall start number
		sectors[s].we = loadSectors[v1 + 1];                   //wall end   number
		sectors[s].z1 = loadSectors[v1 + 2];                   //sector bottom height
		sectors[s].z2 = loadSectors[v1 + 3] - loadSectors[v1 + 2]; //sector top    height
		sectors[s].c1 = loadSectors[v1 + 4];                   //sector top    color
		sectors[s].c2 = loadSectors[v1 + 5];                   //sector bottom color
		v1 += 6;
		for (w = sectors[s].ws; w < sectors[s].we; w++)
		{
			walls[w].x1 = loadWalls[v2 + 0]; //bottom x1
			walls[w].y1 = loadWalls[v2 + 1]; //bottom y1
			walls[w].x2 = loadWalls[v2 + 2]; //top    x2
			walls[w].y2 = loadWalls[v2 + 3]; //top    y2
			walls[w].c = loadWalls[v2 + 4]; //wall color
			v2 += 5;
		}
	}


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
	//for (int i = 0; i < g_frameWidth * g_frameHeight; i++)
	//{
	//	g_frameBuffer[i] = ColorToUInt(255, 50, 50); // Темно-синий фон
	//	g_depthBuffer[i] = 10000.0f; // Бесконечность
	//}
	clearBackground();

	movePlayer();
	draw3D();
}

//=============================================================================