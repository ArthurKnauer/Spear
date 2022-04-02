#pragma once

#include "CoreMinimal.h"
#include "ChunkCoords.h"

class IChunk
{
public:
	virtual const FChunkCoords& GetCoords() = 0;
	
	virtual bool IsDone() = 0;
	virtual void Disable() = 0;
	virtual void Enable() = 0;
	virtual void RecycleTo(FChunkCoords coords) = 0;
};


class ITerrainChunk : public IChunk
{
public:
	virtual float GetHeight(const FVector& location) const = 0;
};
