#pragma once

#ifdef _WIN32
#include <Windows.h>
#define IS_WINDOWS

#define _USE_MATH_DEFINES

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#elif defined(__APPLE__)
#define IS_APPLE 1
#define GLFW_INCLUDE_GLCOREARB
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

#include "OSXHelper.hpp"
#endif

// GLFW
// These preprocessor macros suppress documentation errors in glfw3.h
#ifdef IS_APPLE
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#endif

#include <GLFW/glfw3.h>

#ifdef IS_APPLE
#pragma clang pop
#endif

// Text
#include <ft2build.h>
#include FT_FREETYPE_H

// Boost
#include <boost/filesystem.hpp>

// Assimp
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

// SOIL
#include <libSOIL/SOIL.h>

// Other Includes
#include <algorithm>
#include <iostream>
#include <random>
#include <array>
#include <map>
#include <cassert>
