#pragma once

#include "CoreMinimal.h"
#include "Misc/Crc.h"

#include "ChunkCoords.generated.h"

USTRUCT()
struct FChunkCoords
{
	GENERATED_USTRUCT_BODY()

public:
	int x;
	int y;
	uint32 hash;

	FChunkCoords() : x(0), y(0), hash(0) {}
	FChunkCoords(int x, int y) : x(x), y(y), hash(FCrc::MemCrc32(this, sizeof(int) * 2)) {}

	FORCEINLINE int distSquared(const FChunkCoords& other) const
	{
		return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
	}

	FORCEINLINE FVector worldLocation(float chunkSize, const FIntVector& worldOrigin) const
	{
		return  FVector(
			chunkSize * (static_cast<float>(x) - worldOrigin.X / chunkSize),
			chunkSize * (static_cast<float>(y) - worldOrigin.Y / chunkSize), 
			0);
	}

	FORCEINLINE uint32 makeSeed(uint32 seed) const
	{
		return FCrc::MemCrc32(&hash, sizeof(hash), seed);
	}
};

FORCEINLINE bool operator==(const FChunkCoords& lhs, const FChunkCoords& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;

}
FORCEINLINE bool operator!=(const FChunkCoords& lhs, const FChunkCoords& rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y;
}

FORCEINLINE uint32 GetTypeHash(const FChunkCoords& coords)
{
	return coords.hash;
}

