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