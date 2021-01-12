#include "Chunk.h"
#include "Camera.h"


Chunk::Chunk(int _chunkX, int _chunkZ) : chunkX(_chunkX), chunkZ(_chunkZ) {
	drawMap = new int** [chunkHeight];
	airMap = new bool[chunkHeight];
	amountLoaded = 0;
	alreadyLoaded = false;
	for (int y = 0; y < chunkHeight; y++)
	{
		drawMap[y] = new int* [chunkSize];
		for (int x = 0; x < chunkSize; x++)
		{
			drawMap[y][x] = new int[chunkSize];
			for (int z = 0; z < chunkSize; z++)
			{
				if (y == 0 || y == 1) {
					drawMap[y][x][z] = 4;
					continue;
				}

				int trueX = x + (chunkX * 16);
				int trueZ = z + (chunkZ * 16);

				float noise = abs(ChunkLoader::landPN.noise(trueX / 10.0, 0, trueZ / 10.0));

				float caveNoise = ChunkLoader::cavePN.noise(trueX / 10.0, y / 20.0, trueZ / 10.0);

				if (noise * 15 + 60 > y)
				{
					if (caveNoise > 0.75)
					{
						drawMap[y][x][z] = 0;
						continue;
					}

					drawMap[y][x][z] = 2;
					if (noise * 15 + 59 <= y)
						drawMap[y][x][z] = 1;

					if (noise * 15 + 50 > y)
					{
						drawMap[y][x][z] = 3;
						if (abs(ChunkLoader::landPN.noise(trueX / 10.0, y / 10.0, trueZ / 10.0)) > 0.8)
							drawMap[y][x][z] = 5;
					}
				}
				else
					drawMap[y][x][z] = 0;
			}
		}
	}

	updateFullAirMap();
}

void Chunk::updateFullAirMap() {
	for (int i = 0; i < chunkHeight; i++)
		updatePartialAirMap(i);
}

void Chunk::updatePartialAirMap(int y) {
	for (int x = 0; x < chunkSize; x++)
	{
		for (int z = 0; z < chunkSize; z++)
		{
			if (drawMap[y][x][z] == 0)
			{
				airMap[y] = true;
				return;
			}
		}
	}
	airMap[y] = false;
}


void Chunk::updatePlayerDistance()
{
	glm::vec2 middle = glm::vec2(chunkX * 16 + 8, chunkZ * 16 + 8);
	distanceFromCam = pow((middle.x - Camera::getPosition().x), 2) + pow((middle.y - Camera::getPosition().z), 2);
}

void Chunk::draw()
{
	TextureLoader::getCubeShader().use();
	for (auto& x : cubeMap)
	{
		x.second.draw();
	}
}

void Chunk::reload()
{
	amountLoaded = 0;
	/*for (int i = 0; i < chunkSize; i++)
	{
		updatePartialCubeMap
	}*/
}

void Chunk::tickLoadCubeMap()
{
	if (!alreadyLoaded)
	{
		int loadSpeed = 4;

		for (int i = 0; i < loadSpeed; i++)
		{
			if(amountLoaded + i < 256)
				updatePartialCubeMap((amountLoaded + i) % 16, (amountLoaded + i) / 16);
		}

		amountLoaded += loadSpeed;
	}
	else
	{
		int reloadSpeed = 1;
		if (amountLoaded >= 0 && amountLoaded <= 15)
			updatePartialCubeMap(amountLoaded%16, 0);
		else if (amountLoaded >= 16 && amountLoaded <= 31)
			updatePartialCubeMap(amountLoaded%16, 15);
		else if (amountLoaded >= 32 && amountLoaded <= 47)
			updatePartialCubeMap(0, amountLoaded%16);
		else if (amountLoaded >= 48 && amountLoaded <= 63)
			updatePartialCubeMap(15, amountLoaded%16);
		else
			amountLoaded = 255;
		
		amountLoaded += reloadSpeed;
	}
	if (amountLoaded >= chunkSize*chunkSize)
	{
		alreadyLoaded = true;
	}
}

void Chunk::updatePartialCubeMap(int x, int z)
{
	for (int y = 0; y < chunkHeight; y++)
	{
		updateBlock(x, y, z);
	}
}

void Chunk::updateBlock(int x, int y, int z)
{
	if (x < 0) { ChunkLoader::updateBlock(chunkX - 1, chunkZ, 15, y, z); return; }
	else if (x > 15) { ChunkLoader::updateBlock(chunkX + 1, chunkZ, 0, y, z); return; }
	else if (z < 0) { ChunkLoader::updateBlock(chunkX, chunkZ - 1, x, y, 15); return; }
	else if (z > 15) { ChunkLoader::updateBlock(chunkX, chunkZ + 1, x, y, 0); return; }
	else if (y < 0) { return; }
	else if (y >= chunkHeight) { return; }

	if (shouldDraw(x, y, z))
	{
		std::map<Vector3, Cube>::iterator it;
		it = cubeMap.find(Vector3(x, y, z));
		if (it == cubeMap.end())
		{
			Cube newCube = Cube();
			newCube.setPosition(glm::vec3(x + (chunkX * chunkSize), y, z + (chunkZ * chunkSize)));
			newCube.blockID = drawMap[y][x][z];
			cubeMap[Vector3(x, y, z)] = newCube;
		}
	}
	else
	{
		std::map<Vector3, Cube>::iterator it;
		it = cubeMap.find(Vector3(x, y, z));
		if (it != cubeMap.end())
		{
			cubeMap[Vector3(x, y, z)].blockID = 0;
			cubeMap.erase(Vector3(x, y, z));
		}
	}
}

void Chunk::destroyBlock(int x, int y, int z)
{
	drawMap[y][x][z] = 0;
	
	updateBlock(x, y, z);
	updateBlock(x - 1, y, z);
	updateBlock(x + 1, y, z);
	updateBlock(x, y - 1, z);
	updateBlock(x, y + 1, z);
	updateBlock(x, y, z - 1);
	updateBlock(x, y, z + 1);
	
}

void Chunk::updateFullCubeMap()
{
	for (int x = 0; x < chunkSize; x++)
	{
		for (int z = 0; z < chunkSize; z++)
		{
			updatePartialCubeMap(x, z);
		}
	}
	alreadyLoaded = true;
	amountLoaded = 255;
}

bool Chunk::isNearbyBlockAir(int x, int y, int z) // Should only be + or - 1 block from in chunk
{
	//Check in this chunk's boundary 
	if (x >= 0 && x <= 15 && z >= 0 && z <= 15)
	{
		if (y >= 0 && y < chunkHeight)
			return drawMap[y][x][z] == 0;
		else
			return false;
	}
	else // Check nearby chunk's boundary
	{
		//Case 1: negative X
		if (x < 0)
			return ChunkLoader::isChunkBlockAir(chunkX - 1, chunkZ, 15, y, z);
		//Case 2: positive X
		else if (x > 15)
			return ChunkLoader::isChunkBlockAir(chunkX + 1, chunkZ, 0, y, z);
		//Case 3: negative Z
		else if (z < 0)
			return ChunkLoader::isChunkBlockAir(chunkX, chunkZ - 1, x, y, 15);
		//Case 4: positive Z
		else if (z > 15)
			return ChunkLoader::isChunkBlockAir(chunkX, chunkZ + 1, x, y, 0);
	}
	return false;
}

bool Chunk::shouldDraw(int x, int y, int z)
{
	// First quick checks; if there is no air blocks in my layer, or above and below, then there is no reason to check nearby
	if (y == 0)
	{
		if (!airMap[y] && !airMap[y + 1])
			return false;
	}
	else if (y == chunkHeight - 1)
	{
		if (!airMap[y] && !airMap[y - 1])
			return false;
	}
	else
	{
		if (!airMap[y] && !airMap[y - 1] && !airMap[y + 1])
			return false;
	}
	// If there are possible near air blocks, and this block is not air, return true if there is a nearby air block.	

	return (drawMap[y][x][z] != 0) && 
		(isNearbyBlockAir(x - 1, y, z) ||
		isNearbyBlockAir(x + 1, y, z) ||
		isNearbyBlockAir(x, y - 1, z) ||
		isNearbyBlockAir(x, y + 1, z) ||
		isNearbyBlockAir(x, y, z - 1) ||
		isNearbyBlockAir(x, y, z + 1));
}