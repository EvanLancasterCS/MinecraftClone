#pragma once

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <vector>
#include "Camera.h"
#include "Chunk.h"
#include "PerlinNoise.h"

static class ChunkLoader
{
	friend class Chunk;
public:

	static void initialize();

	static void drawChunks(Camera cam);

	static void tickLoadChunks(Camera cam);

	static void forceLoadChunk(int chunkX, int chunkZ);

	static void updateBlock(int chunkX, int chunkZ, int x, int y, int z);
	
	static void destroyBlock(int chunkX, int chunkZ, int x, int y, int z);
	
	static void loadChunk(int chunkX, int chunkZ);

	static void unloadChunk(int chunkX, int chunkZ);

	static PerlinNoise landPN;
	static PerlinNoise cavePN;
	
private:
	static std::vector<Chunk*> loadedChunks;

	static int numChunksLoaded;

	static bool isChunkBlockAir(int chunkX, int chunkZ, int x, int y, int z);

	static int getChunkIndex(int chunkX, int chunkZ);

	static Chunk& getChunk(int chunkX, int chunkZ);

	static bool isChunkLoaded(int chunkX, int chunkZ);
};

