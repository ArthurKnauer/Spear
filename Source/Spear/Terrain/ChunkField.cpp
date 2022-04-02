#include "ChunkField.h"

#include "Chunk.h"
#include "DrawDebugHelpers.h"
#include "Spear/Logging.h"
#include "Engine.h"


ChunkField::ChunkField(SpawnChunk spawnChunk, FChunkConfig config) :
	spawnChunk(spawnChunk),
	config(config),
	chunksFromPlayerSquared(config.chunksFromPlayer* config.chunksFromPlayer),
	chunksFromEachPlayerInServerSquared(config.chunksFromEachPlayerServer* config.chunksFromEachPlayerServer)
{
	const auto chunksPerRow = 1 + 2 * config.chunksFromPlayer;
	visibleChunks.Reserve(chunksPerRow * chunksPerRow);
}

IChunk* ChunkField::GetChunkAt(FVector location, FIntVector worldOrigin) const
{
	const auto coords = ToCoords(location, worldOrigin);
	if (visibleChunks.Contains(coords))
		return visibleChunks[coords];

	return nullptr;
}

bool ChunkField::Update(UWorld* world)
{
	if (CalcNewPlayerCoords(world))
	{
		for (auto it = visibleChunks.CreateIterator(); it; ++it)
		{
			if (!it->Value->IsDone())
				continue;

			if (!CloseToAnyPlayer(it->Value->GetCoords()))
			{
				it->Value->Disable();
				hiddenChunks.Add(it->Value);
				it.RemoveCurrent();
			}
		}

		for (int p = 0; p < playerCoords.Num(); p++)
		{
			auto maxDistSquared = chunksFromPlayerSquared;
			if (p > 0)
				maxDistSquared = chunksFromEachPlayerInServerSquared;

			for (int dy = -config.chunksFromPlayer; dy <= config.chunksFromPlayer; ++dy)
			{
				for (int dx = -config.chunksFromPlayer; dx <= config.chunksFromPlayer; ++dx)
				{
					const int distToPlayerSquared = dx * dx + dy * dy;
					if (distToPlayerSquared <= maxDistSquared)
					{
						const FChunkCoords coords{ playerCoords[p].x + dx, playerCoords[p].y + dy };
						if (!visibleChunks.Contains(coords) && !chunksToCreate.Contains(coords))
						{
							chunksToCreate.Add(coords);
						}
					}
				}
			}
		}

		if (config.showDebug)
			PRINT_N_LOG(LogSpearDebug, Display, TEXT("ChunkField toCreate %i, hidden %i"),
				chunksToCreate.Num(), hiddenChunks.Num());
	}

	if (chunksToCreate.Num() > 0)
	{
		const auto coords = chunksToCreate.Pop();
		CreateOrRecylceAt(coords);

		if (chunksToCreate.Num() == 0 && config.showDebug)
			PRINT_N_LOG(LogSpearDebug, Display, TEXT("ChunkField created, hidden %i"), hiddenChunks.Num());

		return false;
	}

	return true; // no more chunks to create
}

bool ChunkField::CalcNewPlayerCoords(UWorld* world)
{
	bool newCoords = false;
	int idx = 0;
	for (auto controller = world->GetPlayerControllerIterator(); controller; ++controller)
	{
		const auto pawn = controller->Get()->GetPawn();
		if (pawn)
		{
			const auto coord = ToCoords(pawn->GetActorLocation(), world->OriginLocation);
			if (playerCoords.Num() <= idx)
			{
				playerCoords.Add(coord);
				newCoords = true;
			}
			else if (playerCoords[idx] != coord)
			{
				playerCoords[idx] = coord;
				newCoords = true;
			}
			idx++;
		}
	}

	playerCoords.SetNum(idx); // cap off players who left

	return newCoords;
}

bool ChunkField::CloseToAnyPlayer(FChunkCoords coords)
{
	for (int p = 0; p < playerCoords.Num(); p++)
	{
		const auto distSquared = coords.distSquared(playerCoords[p]);
		if (p == 0)
		{
			if (distSquared <= chunksFromPlayerSquared)
				return true;
		}
		else if (distSquared <= chunksFromEachPlayerInServerSquared)
			return true;
	}
	return false;
}

void ChunkField::CreateOrRecylceAt(FChunkCoords coords)
{
	if (hiddenChunks.Num() > 0)
	{
		auto chunk = hiddenChunks.Pop();
		chunk->RecycleTo(coords);
		visibleChunks.Add(coords, chunk);
	}
	else
	{
		auto chunk = spawnChunk(coords);
		visibleChunks.Add(coords, chunk);
	}
}

FChunkCoords ChunkField::ToCoords(FVector location, FIntVector worldOrigin) const
{
	float chunkX = location.X / config.size + worldOrigin.X / config.size;
	float chunkY = location.Y / config.size + worldOrigin.Y / config.size;
	if (location.X + worldOrigin.X < 0)
		chunkX -= 1;
	if (location.Y + worldOrigin.Y < 0)
		chunkY -= 1;

	return FChunkCoords(chunkX, chunkY);
}