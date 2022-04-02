#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"

#include "ChunkConfig.generated.h"


USTRUCT(BlueprintType)
struct FChunkConfig
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool showDebug = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "128"))
		int chunksFromPlayer = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "128"))
		int chunksFromEachPlayerServer = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "10.0", ClampMax = "100000.0"))
		float size = 2000.0f;	
};

USTRUCT(BlueprintType)
struct FTerrainChunkConfig
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UMaterialInterface* material = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FChunkConfig chunk;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "2", ClampMax = "256"))
		int quads = 16;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "100000.0"))
		float amplitude = 2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float offset = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float lacunarity = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float persistance = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "32"))
		int octaves = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "100000.0"))
		float coordScale = 0.15f;
};


USTRUCT(BlueprintType)
struct FInstancesChunkConfig
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool enabled = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool enabledInServer = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FChunkConfig chunk;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "50.0"))
		float spacing = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<UInstancedStaticMeshComponent> Class;
};

void ValidateChunkConfig(FTerrainChunkConfig& terrainConfig, TArray<FInstancesChunkConfig>& instancesConfigs);