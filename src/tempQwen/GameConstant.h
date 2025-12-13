#pragma once

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

#define numSprites 23

#define MAX_DOORS 20
enum DoorState { closed, opening, open, closing };
enum Direction { dir_N, dir_S, dir_E, dir_W };

#define MAX_PUSH_WALLS 20

// Floor and ceiling tile types
enum FloorTileType
{
	FLOOR_TILE_DEFAULT = 0,
	FLOOR_TILE_1,
	FLOOR_TILE_2,
	FLOOR_TILE_3,
	FLOOR_TILE_4,
	FLOOR_TILE_5,
	FLOOR_TILE_COUNT
};

enum CeilingType
{
	CEILING_SKY = 0,        // Sky visible
	CEILING_SOLID,          // Solid ceiling texture
	CEILING_TEXTURED,       // Textured ceiling
	CEILING_TYPE_COUNT
};