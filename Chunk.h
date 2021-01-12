#pragma once

#include "Cube.h"
#include "ChunkLoader.h"
#include "Vector.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <map>
#include <vector>

const int chunkHeight = 256;
const int chunkSize = 16;



class Chunk
{
	friend class ChunkLoader;
public:

	Chunk(int _chunkX, int _chunkZ);

	void draw();

	void destroyBlock(int x, int y, int z);

	Chunk(const Chunk& copy) = delete;
	Chunk(Chunk& copy) = delete;
	Chunk& operator=(const Chunk& other) = delete;
	Chunk& operator=(Chunk& other) = delete;

private:
	const int chunkX, chunkZ;
	int amountLoaded;
	bool alreadyLoaded;
	int*** drawMap;
	bool* airMap;
	std::map<Vector3, Cube> cubeMap;
	float distanceFromCam;

	void reload();

	void updateFullCubeMap();
	void tickLoadCubeMap();
	void updatePartialCubeMap(int x, int z);
	void updateBlock(int x, int y, int z);

	void updateFullAirMap();
	void updatePartialAirMap(int y);

	void updatePlayerDistance();

	bool isNearbyBlockAir(int x, int y, int z);

	bool shouldDraw(int x, int y, int z);
};


