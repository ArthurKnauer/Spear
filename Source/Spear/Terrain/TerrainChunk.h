#pragma once

#include "CoreMinimal.h"
#include "Tasks/GenGroundTask.h"
#include "Chunk.h"
#include <memory>

#include "TerrainChunk.generated.h"

class SimplexNoise;
struct FTerrainChunkConfig;
class UProceduralMeshComponent;


UCLASS()
class SPEAR_API ASimplexTerrainChunk : public AActor, public ITerrainChunk
{
	GENERATED_BODY()

public:
	ASimplexTerrainChunk();
	~ASimplexTerrainChunk();

	static ASimplexTerrainChunk* Spawn(UWorld* world,
		FChunkCoords coords,
		const SimplexNoise* noise,
		const FTerrainChunkConfig* terrain);

	virtual void Tick(float DeltaTime) override;

	const FChunkCoords& GetCoords() override { return coords; }

	bool IsDone() override;
	void Disable() override;
	void Enable() override;
	void RecycleTo(FChunkCoords newCoords) override;

	float GetHeight(const FVector& location) const override;	

private:
	void ShowDebugBox(FColor color, float lifetime) const;

	void StartGenTask();
	void TryFinishGenTask();

	FChunkCoords coords;
	const SimplexNoise* noise = nullptr;
	const FTerrainChunkConfig* config = nullptr;

	UProceduralMeshComponent* mesh = nullptr;
	TArray<FVector> vertices;

	bool isRecycled = false;
	std::unique_ptr<FAsyncTask<FGenGroundTask>> genTask;
};
