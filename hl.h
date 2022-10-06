#pragma once

#include <stdio.h>

#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define uint unsigned int

#define uint8 unsigned char
#define uint16 unsigned short
#define uint32 unsigned int
#define uint64 unsigned long

#define int8 char
#define int16 short
#define int32 int
#define int64 long

namespace HL
{
#include "core.h"
#include "texture.h"
#include "shader.h"
#include "mesh.h"
}