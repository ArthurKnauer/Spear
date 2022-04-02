#pragma once

#include "Async/AsyncWork.h"
#include "../ChunkCoords.h"
#include "ProceduralMeshComponent.h"

class SimplexNoise;
struct FTerrainChunkConfig;

class FGenGroundTask
{
	friend class FAsyncTask<FGenGroundTask>;
public:
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;

	FGenGroundTask(
		FChunkCoords coords,
		const SimplexNoise* noise,		
		const FTerrainChunkConfig* ground,
		TArray<FVector>& groundVertices
	);

	bool CanAbandon() { return true; }
	void Abandon() { abort = true; }
	void DoWork();	

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FGenGroundTask, STATGROUP_ThreadPoolAsyncTasks);
	}

private:
	bool abort = false;
	const FChunkCoords coords;
	const SimplexNoise* const noise;
	const FTerrainChunkConfig* const ground;
	TArray<FVector>& vertices;
};
