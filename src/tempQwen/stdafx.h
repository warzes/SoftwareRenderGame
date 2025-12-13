#pragma once

#include "EngineBuild.h"

#if defined(_MSC_VER)
#	pragma warning(disable : 4514)
#	pragma warning(disable : 5045)
#	pragma warning(push, 3)
#	pragma warning(disable : 4865)
#	pragma warning(disable : 5039)
#endif

#define _USE_MATH_DEFINES

#if ENABLE_RGFW
#else
#	if defined(_WIN32)
#		define NOMINMAX
#		define WIN32_LEAN_AND_MEAN

#		include <SDKDDKVer.h>
#		include <windows.h>
#	endif
#endif

#include <cstdlib>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>
#include <stack>
#include <vector>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#include "GameConstant.h"