#pragma once

#include <iostream>

#include "Shader.h"

static class TextureLoader
{
public:
	static glm::mat4 projection;

	static void initialize();

	static Shader getCubeShader();

	static GLuint getCubemap(int blockID);

private:
	static unsigned int** blockTexIDs;
	static char** textures;
	static GLuint* cubeMapIDs;
	static Shader* cubeShader;

	static void loadTexture(int blockID);
};
