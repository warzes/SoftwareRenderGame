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

unsigned ColorToUInt(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

inline unsigned ColorLerp(unsigned color1, unsigned color2, double t)
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

// Простое затенение на основе расстояния
unsigned ApplyLighting(unsigned color, float distance);

void loadFile(std::vector<unsigned char>& buffer, const std::string& filename);
void saveFile(const std::vector<unsigned char>& buffer, const std::string& filename);

int loadImage(std::vector<unsigned>& out, unsigned long& w, unsigned long& h, const std::string& filename);
int decodePNG(std::vector<unsigned char>& out_image, unsigned long& image_width, unsigned long& image_height, const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);
int decodePNG(std::vector<unsigned char>& out_image_32bit, unsigned long& image_width, unsigned long& image_height, const std::vector<unsigned char>& in_png);