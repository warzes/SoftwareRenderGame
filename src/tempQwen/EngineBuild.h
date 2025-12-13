#pragma once

#define ENABLE_RGFW 0
#define ENABLE_WINAPI 0

#if defined(__EMSCRIPTEN__)
#endif

#if defined(_WIN32)
#	if !ENABLE_RGFW
#		undef ENABLE_WINAPI
#		define ENABLE_WINAPI 1
#	endif
#endif