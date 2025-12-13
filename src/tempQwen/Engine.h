#pragma once

#define MOUSE_BUTTON_LEFT 1
#define MOUSE_BUTTON_RIGHT 2
#define MOUSE_BUTTON_MIDDLE 4

void Print(const std::string& msg);
void Info(const std::string& msg);
void Warning(const std::string& msg);
void Debug(const std::string& msg);
void Error(const std::string& msg);
void Fatal(const std::string& msg);

inline float DegToRad(float degrees) { return degrees * M_PI / 180.0f; }

struct Color final
{
	Color() = default;
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b) , a(a) {}

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 0;
};

unsigned ColorToUInt(const Color& color);
unsigned ColorToUInt(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

struct Vector2D final
{
	Vector2D() = default;
	Vector2D(float x, float y) : x(x), y(y) {}

	Vector2D operator+(const Vector2D& v) const { return { x + v.x, y + v.y }; }
	Vector2D operator-(const Vector2D& v) const { return { x - v.x, y - v.y }; }
	Vector2D operator*(float s) const { return { x * s, y * s }; }

	float Length() const { return sqrtf(x * x + y * y); }

	Vector2D Normalize() const
	{
		float len = Length();
		return len > 0.0 ? Vector2D(x / len, y / len) : Vector2D(0, 0);
	}

	float x = 0.0f;
	float y = 0.0f;
};

struct Vector3D final
{
	Vector3D() = default;
	Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector3D operator+(const Vector3D& v) const { return { x + v.x, y + v.y, z + v.z }; }
	Vector3D operator-(const Vector3D& v) const { return { x - v.x, y - v.y, z - v.z }; }
	
	float Length() const { return sqrtf(x * x + y * y + z * z); }
	Vector3D Normalize() const
	{
		const float len = Length();
		return len > 0 ? Vector3D(x / len, y / len, z / len) : Vector3D(0, 0, 0);
	}

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

bool RayIntersect(const Vector2D& rayStart, const Vector2D& rayDir, const Vector2D& wallStart, const Vector2D& wallEnd, float& distance, Vector2D& intersection);

struct WallSegment final
{
	Vector2D start, end;
	int textureId;
	float heightTop, heightBottom;
};

struct SectorO final
{
	std::vector<WallSegment> walls;
	float floorHeight, ceilingHeight;
	int floorTexture, ceilingTexture;
};

struct Camera final
{
	Camera(Vector3D pos, float ang) : position(pos), angle(ang), pitch(0.0f) {}

	Vector3D position;
	float angle; // Угол поворота камеры (в радианах)
	float pitch; // Наклон камеры (в радианах)
};

void MoveCamera(Camera& camera, float dx, float dy, float dz = 0);
void RotateCamera(Camera& camera, float angleChange);

class Texture final
{
public:
	Texture(int w, int h) : width(w), height(h)
	{
		pixels.resize(w * h, ColorToUInt(128, 128, 128)); // Заполняем серым по умолчанию
	}

	uint32_t GetPixel(int x, int y) const
	{
		x = x % width;
		y = y % height;
		if (x < 0) x += width;
		if (y < 0) y += height;
		return pixels[y * width + x];
	}

	void SetPixel(int x, int y, uint32_t color)
	{
		if (x >= 0 && x < width && y >= 0 && y < height) {
			pixels[y * width + x] = color;
		}
	}

	int width, height;
	std::vector<uint32_t> pixels;
};

// Простое затенение на основе расстояния
COLORREF ApplyLighting(COLORREF color, float distance);