#include "ChunkLoader.h"

int ChunkLoader::numChunksLoaded = 0;
std::vector<Chunk*> ChunkLoader::loadedChunks = std::vector<Chunk*>();

PerlinNoise ChunkLoader::landPN = PerlinNoise(5);
PerlinNoise ChunkLoader::cavePN = PerlinNoise(25);

const int unloadDistance = 50;

bool ChunkLoader::isChunkBlockAir(int chunkX, int chunkZ, int x, int y, int z)
{
	if (isChunkLoaded(chunkX, chunkZ)) {
		Chunk& chunk = getChunk(chunkX, chunkZ);
		return (chunk.drawMap[y][x][z] == 0);
	}
	return false;
}

void ChunkLoader::initialize()
{
	
}

void ChunkLoader::tickLoadChunks(Camera cam)
{
	Chunk* closestUnloaded = nullptr;
	for (int i = 0; i < loadedChunks.size(); i++)
	{
		if (loadedChunks[i]->amountLoaded < chunkSize*chunkSize)
		{
			bool lowerDistance, loadAdvantage = false;
			if (closestUnloaded != nullptr)
			{
				loadAdvantage = closestUnloaded->alreadyLoaded && !loadedChunks[i]->alreadyLoaded;
				lowerDistance = closestUnloaded->distanceFromCam > loadedChunks[i]->distanceFromCam;
			}

			if (closestUnloaded == nullptr && loadedChunks[i]->distanceFromCam < pow(unloadDistance, 2))
				closestUnloaded = loadedChunks[i];
			else if (closestUnloaded != nullptr && (lowerDistance || loadAdvantage))
				closestUnloaded = loadedChunks[i];
			//loadedChunks[i]->tickLoadCubeMap();
			//break;
		}
	}
	if (closestUnloaded != nullptr)
		closestUnloaded->tickLoadCubeMap();
}

void ChunkLoader::drawChunks(Camera cam)
{
	for (int i = 0; i < loadedChunks.size(); i++)
	{
		loadedChunks[i]->updatePlayerDistance();
		if (loadedChunks[i]->distanceFromCam > pow(unloadDistance, 2) || !cam.chunkWithinFrustrum(glm::vec2(loadedChunks[i]->chunkX * 16, loadedChunks[i]->chunkZ * 16)))
		{
			continue;
		}
		if(loadedChunks[i]->alreadyLoaded)
			loadedChunks[i]->draw();
	}
}

void ChunkLoader::forceLoadChunk(int chunkX, int chunkZ)
{
	if (!isChunkLoaded(chunkX, chunkZ))
	{
		Chunk* newChunk = new Chunk(chunkX, chunkZ);
		loadedChunks.push_back(newChunk);
		newChunk->updateFullCubeMap();
		
		if (isChunkLoaded(chunkX - 1, chunkZ))
			getChunk(chunkX - 1, chunkZ).reload();
		if (isChunkLoaded(chunkX + 1, chunkZ))
			getChunk(chunkX + 1, chunkZ).reload();
		if (isChunkLoaded(chunkX, chunkZ - 1))
			getChunk(chunkX, chunkZ - 1).reload();
		if (isChunkLoaded(chunkX, chunkZ + 1))
			getChunk(chunkX, chunkZ + 1).reload();
	}
}

void ChunkLoader::loadChunk(int chunkX, int chunkZ)
{
	if (!isChunkLoaded(chunkX, chunkZ))
	{
		Chunk* newChunk = new Chunk(chunkX, chunkZ);
		loadedChunks.push_back(newChunk);

		if (isChunkLoaded(chunkX - 1, chunkZ))
			getChunk(chunkX - 1, chunkZ).reload();
		if (isChunkLoaded(chunkX + 1, chunkZ))
			getChunk(chunkX + 1, chunkZ).reload();
		if (isChunkLoaded(chunkX, chunkZ - 1))
			getChunk(chunkX, chunkZ - 1).reload();
		if (isChunkLoaded(chunkX, chunkZ + 1))
			getChunk(chunkX, chunkZ + 1).reload();
	}

}

void ChunkLoader::unloadChunk(int chunkX, int chunkZ)
{
	if (isChunkLoaded(chunkX, chunkZ))
	{
		int index = getChunkIndex(chunkX, chunkZ);
		Chunk* chunkToDelete = loadedChunks[index];

		loadedChunks.erase(loadedChunks.begin() + index);
		chunkToDelete->~Chunk();
	}
}

void ChunkLoader::destroyBlock(int chunkX, int chunkZ, int x, int y, int z)
{
	if (isChunkLoaded(chunkX, chunkZ))
	{
		Chunk& chunk = getChunk(chunkX, chunkZ);
		chunk.destroyBlock(x, y, z);
	}
}

void ChunkLoader::updateBlock(int chunkX, int chunkZ, int x, int y, int z)
{
	if (isChunkLoaded(chunkX, chunkZ))
	{
		Chunk& chunk = getChunk(chunkX, chunkZ);
		chunk.updateBlock(x, y, z);
	}
}

int ChunkLoader::getChunkIndex(int chunkX, int chunkZ)
{
	for (int i = 0; i < loadedChunks.size(); i++)
	{
		if (loadedChunks[i]->chunkX == chunkX && loadedChunks[i]->chunkZ == chunkZ)
			return i;
	}
	std::cout << "Error; requested chunk at " << chunkX << ", " << chunkZ << " could not be found\n";
	return -1;
}

Chunk& ChunkLoader::getChunk(int chunkX, int chunkZ)
{
	for (int i = 0; i < loadedChunks.size(); i++)
	{
		if (loadedChunks[i]->chunkX == chunkX && loadedChunks[i]->chunkZ == chunkZ)
			return *loadedChunks[i];
	}
	std::cout << "Error; requested chunk at " << chunkX << ", " << chunkZ << " could not be found\n";
	return *loadedChunks[0];
}

bool ChunkLoader::isChunkLoaded(int chunkX, int chunkZ)
{
	for (int i = 0; i < loadedChunks.size(); i++)
	{
		if (loadedChunks[i]->chunkX == chunkX && loadedChunks[i]->chunkZ == chunkZ)
			return true;
	}
	return false;
}