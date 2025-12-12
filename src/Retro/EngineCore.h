#pragma once

#define MOUSE_BUTTON_LEFT 1
#define MOUSE_BUTTON_RIGHT 2
#define MOUSE_BUTTON_MIDDLE 4

/*
std::size_t seed = 0;
HashCombine(seed, h1, h2, h3);
*/
inline void HashCombine([[maybe_unused]] std::size_t& seed) noexcept {}
template <typename T, typename... Rest>
inline void HashCombine(std::size_t& seed, const T& v, Rest... rest) noexcept
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	HashCombine(seed, rest...);
}

void Print(const std::string& msg);
void Info(const std::string& msg);
void Warning(const std::string& msg);
void Debug(const std::string& msg);
void Error(const std::string& msg);
void Fatal(const std::string& msg);