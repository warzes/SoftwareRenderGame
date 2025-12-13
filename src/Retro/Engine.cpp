#include "stdafx.h"
#include "Engine.h"
//=============================================================================
extern bool IsExitApp;
//=============================================================================
void Print(const std::string& msg)
{
	puts(msg.c_str());
}
//=============================================================================
void Info(const std::string& msg)
{
	Print("\033[32m[INFO]:\033[0m " + msg);
}
//=============================================================================
void Warning(const std::string& msg)
{
	Print("\033[33m[WARNING]:\033[0m " + msg);
}
//=============================================================================
void Debug(const std::string& msg)
{
#if defined(_DEBUG)
	Print("\033[36m[DEBUG]:\033[0m " + msg);
#endif
}
//=============================================================================
void Error(const std::string& msg)
{
	Print("\033[31m[ERROR]:\033[0m " + msg);
}
//=============================================================================
void Fatal(const std::string& msg)
{
	Print("\033[35m[FATAL]:\033[0m " + msg);
	IsExitApp = true;
}
//=============================================================================
unsigned ColorToUInt(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	const unsigned bgra = unsigned(b | (g << 8) | (r << 16) | (a << 24));
	return bgra;
}
void GetBGRAFromUInt(unsigned color, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
{
	b = (color & 0x000000FF);
	g = (color & 0x0000FF00) >> 8;
	r = (color & 0x00FF0000) >> 16;
	a = (color & 0xFF000000) >> 24;
}
//=============================================================================
unsigned ApplyLighting(unsigned color, float distance)
{
	float factor = std::max(0.1f, 1.0f - distance * 0.01f);
	uint8_t r, g, b, a;
	GetBGRAFromUInt(color, r, g, b, a);
	r = static_cast<uint8_t>(r * factor);
	g = static_cast<uint8_t>(g * factor);
	b = static_cast<uint8_t>(b * factor);
	return ColorToUInt(r, g, b, a);
}
//=============================================================================
void loadFile(std::vector<unsigned char>& buffer, const std::string& filename) //designed for loading files from hard disk in an std::vector
{
	std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

	//get filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	//read contents of the file into the vector
	buffer.resize(size_t(size));
	if (size > 0) file.read((char*)(&buffer[0]), size);
}

//write given buffer to the file, overwriting the file, it doesn't append to it.
void saveFile(const std::vector<unsigned char>& buffer, const std::string& filename)
{
	std::ofstream file(filename.c_str(), std::ios::out | std::ios::binary);
	file.write(buffer.size() ? (char*)&buffer[0] : 0, std::streamsize(buffer.size()));
}

int loadImage(std::vector<unsigned>& out, unsigned long& w, unsigned long& h, const std::string& filename)
{
	std::vector<unsigned char> file, image;
	loadFile(file, filename);
	if (decodePNG(image, w, h, file)) return 1;

	out.resize(image.size() / 4);

	for (size_t i = 0; i < out.size(); i++)
	{
		out[i] = 0x1000000 * image[i * 4 + 3] + 0x10000 * image[i * 4 + 0] + 0x100 * image[i * 4 + 1] + image[i * 4 + 2];
	}

	return 0;
}