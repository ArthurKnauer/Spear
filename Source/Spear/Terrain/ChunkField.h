#pragma once

#include "CoreMinimal.h"
#include "ChunkCoords.h"
#include "ChunkConfig.h"
#include <functional>

class IChunk;

class ChunkField
{
public:
	using SpawnChunk = std::function<IChunk*(FChunkCoords coords)>;

	ChunkField(SpawnChunk spawnChunk, FChunkConfig config);

	IChunk* GetChunkAt(FVector location, FIntVector worldOrigin) const;

	// Returns true if no more chunks to create
	bool Update(UWorld* world);

private:
	FChunkCoords ToCoords(FVector location, FIntVector worldOrigin) const;
	void CreateOrRecylceAt(FChunkCoords coords);
	bool CalcNewPlayerCoords(UWorld* world);
	bool CloseToAnyPlayer(FChunkCoords coords);

	SpawnChunk spawnChunk;
	const FChunkConfig config;
	const int chunksFromPlayerSquared;
	const int chunksFromEachPlayerInServerSquared;

	TMap<FChunkCoords, IChunk*> visibleChunks;
	TArray<IChunk*> hiddenChunks;
	TArray<FChunkCoords> chunksToCreate;

	TArray<FChunkCoords> playerCoords;
};
