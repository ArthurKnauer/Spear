#pragma once

#include "Async/AsyncWork.h"

class ITerrainChunk;
struct FInstancesChunkConfig;

class FGenPoissonTask
{
	friend class FAsyncTask<FGenPoissonTask>;
public:
	TArray<FTransform> transforms;

	FGenPoissonTask(int seed,
		FVector location,
		const ITerrainChunk* const terrain,
		const FInstancesChunkConfig* const config
	);

	bool CanAbandon() { return true; }
	void Abandon() { abort = true; }
	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FGenPoissonTask, STATGROUP_ThreadPoolAsyncTasks);
	}

private:
	bool abort = false;
	const int seed;
	const FVector location;
	const ITerrainChunk* const terrain;
	const FInstancesChunkConfig* const config;
};
