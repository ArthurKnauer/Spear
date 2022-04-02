#pragma once

#include "CoreMinimal.h"
#include "Tasks/GenPoissonTask.h"
#include "ChunkCoords.h"
#include "Chunk.h"
#include <memory>

#include "InstancesChunk.generated.h"

class ChunkField;
struct FInstancesChunkConfig;
class UHierarchicalInstancedStaticMeshComponent;


UCLASS()
class SPEAR_API AInstancesChunk : public AActor, public IChunk
{
	GENERATED_BODY()

public:
	AInstancesChunk();
	~AInstancesChunk();

	static AInstancesChunk* Spawn(UWorld* world,
		FChunkCoords coords,
		int seed,
		const ChunkField* terrainField,
		const FInstancesChunkConfig* config);

	const FChunkCoords& GetCoords() override { return coords; }
	bool IsDone() override;
	void Disable() override;
	void Enable() override;
	void RecycleTo(FChunkCoords newCoords) override;

	virtual void Tick(float DeltaTime) override;

private:
	void ShowDebugBox(FColor color, float lifetime);

	void StartGenTask();
	void TryFinishGenTask();

	int seed;
	FChunkCoords coords;

	const ChunkField* terrainField = nullptr;
	const FInstancesChunkConfig* config = nullptr;

	UHierarchicalInstancedStaticMeshComponent* mesh = nullptr;

	bool isRecycled = false;
	
	std::unique_ptr<FAsyncTask<FGenPoissonTask>> genTask;
};
