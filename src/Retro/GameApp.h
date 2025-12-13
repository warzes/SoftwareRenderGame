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

// Decomposition helpers for RenderWalls
void CastRayToHit(RayContext& ctx, int x, int h, std::vector<Strip>& strips);
std::vector<Strip> GatherStripsForColumn(int x, int w, int h);
void ComposeColumn(int x, int h, const std::vector<Strip>& strips);

void InitRayContext(int x, int w, RayContext& ctx);
void NextMapHit(RayContext& ctx, int& outMapX, int& outMapY, int& outSide);
// ComputeHitInfo orchestrates specialized handlers to decode what we hit.
// It returns a HitInfo describing the type of wall, its distance, texture,
// and whether the ray must continue scanning (resume). This is a thin
// wrapper dispatching to more specific handlers for clarity.
HitInfo ComputeHitInfo(RayContext& ctx, int hitMapX, int hitMapY, int side);

// Specific hit handlers: they fill 'info' and return true when the hit
// should be accepted, or false when the caller should continue scanning.
bool HandleSunkenWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side);
bool HandlePushWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side);
bool HandleDiagonalWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side);
bool HandleRegularWall(RayContext& ctx, HitInfo& info, int hitMapX, int hitMapY, int side);
/**
 * BuildStripForHit tries to construct a Strip from computed HitInfo.
 * Returns a pair<accepted, Strip>. If accepted is false, the caller
 * should continue scanning (i.e. the hit was not yet ready to draw).
 */
std::pair<bool, Strip> BuildStripForHit(const HitInfo& info, RayContext& ctx, int x, int h);

/**
 * Advances the ray via DDA until it hits a non-empty map cell.
 * Returns map coordinates and which side was crossed.
 */
void NextMapHit(RayContext& ctx, int& outMapX, int& outMapY, int& outSide);

/**
 * ComputeHitInfo orchestrates specific handlers and produces HitInfo.
 * - ctx: input ray context
 * - hitMapX/hitMapY: the map coords of the intersection
 * - side: which side was crossed (0/1)
 */
HitInfo ComputeHitInfo(RayContext& ctx, int hitMapX, int hitMapY, int side);