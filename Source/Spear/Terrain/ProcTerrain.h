#pragma once

#include "CoreMinimal.h"
#include "Math/SimplexNoise.h"
#include "ChunkField.h"
#include "ChunkConfig.h"
#include <memory>
#include <vector>

#include "ProcTerrain.generated.h"


UCLASS()
class SPEAR_API AProcTerrain : public AActor
{
	GENERATED_BODY()
public:
	AProcTerrain();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int seed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool showDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FTerrainChunkConfig terrainConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<FInstancesChunkConfig> instancesConfigs;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	SimplexNoise noise;
	std::unique_ptr<ChunkField> terrainField;
	std::vector<std::unique_ptr<ChunkField>> instancesFields;
};
