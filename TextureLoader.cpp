#include "TextureLoader.h"
#include "Camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int** TextureLoader::blockTexIDs = nullptr;
char** TextureLoader::textures = nullptr;
GLuint* TextureLoader::cubeMapIDs = nullptr;
Shader* TextureLoader::cubeShader = nullptr;

void TextureLoader::initialize()
{
	blockTexIDs = new unsigned int* [6];
	cubeMapIDs = new GLuint[6];
	for (int i = 0; i < 6; i++)
		cubeMapIDs[i] = 0;
	blockTexIDs[0] = new unsigned int[6]{ 0, 0, 0, 0, 0, 0 };
	blockTexIDs[1] = new unsigned int[6]{ 1, 1, 0, 2, 1, 1 };
	blockTexIDs[2] = new unsigned int[6]{ 2, 2, 2, 2, 2, 2 };
	blockTexIDs[3] = new unsigned int[6]{ 3, 3, 3, 3, 3, 3 };
	blockTexIDs[4] = new unsigned int[6]{ 4, 4, 4, 4, 4, 4 };
	blockTexIDs[5] = new unsigned int[6]{ 5, 5, 5, 5, 5, 5 };

	textures = new char* [6];
	for (int i = 0; i < 6; i++)
		textures[i] = new char[256];
	strcpy_s(textures[0], 256 * sizeof(char), "top.png");
	strcpy_s(textures[1], 256 * sizeof(char), "side.png");
	strcpy_s(textures[2], 256 * sizeof(char), "bottom.png");
	strcpy_s(textures[3], 256 * sizeof(char), "stone.png");
	strcpy_s(textures[4], 256 * sizeof(char), "bedrock.png");
	strcpy_s(textures[5], 256 * sizeof(char), "ironore.png");

	cubeShader = new Shader("shader.vs", "shader.fs");
	cubeShader->use();
	cubeShader->setMat4("projection", Camera::getProjection());
}

Shader TextureLoader::getCubeShader()
{
	return *cubeShader;
}

GLuint TextureLoader::getCubemap(int blockID)
{
	if (!cubeMapIDs[blockID])
		loadTexture(blockID);
	return cubeMapIDs[blockID];
}

void TextureLoader::loadTexture(int blockID)
{
	GLuint texID;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	int width, height, nrChannels;
	for (GLuint i = 0; i < 6; i++)
	{
		unsigned char* data = new unsigned char(256);
		data = stbi_load(textures[blockTexIDs[blockID][i]], &width, &height, &nrChannels, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	cubeMapIDs[blockID] = texID;
}