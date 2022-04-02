#include "ProcTerrain.h"
#include "DrawDebugHelpers.h"
#include "Spear/Logging.h"
#include "TerrainChunk.h"
#include "InstancesChunk.h"

AProcTerrain::AProcTerrain()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AProcTerrain::BeginPlay()
{
	Super::BeginPlay();

	ValidateChunkConfig(terrainConfig, instancesConfigs);

	noise = SimplexNoise(seed);

	terrainField = std::make_unique<ChunkField>([this](FChunkCoords coords) {
		return ASimplexTerrainChunk::Spawn(
			GetWorld(), coords, &noise, &terrainConfig);
		}, terrainConfig.chunk);


	for (const auto& configRef : instancesConfigs)
	{
		const auto* config = &configRef;
		if (config->enabled)
		{
			instancesFields.push_back(std::move(std::make_unique<ChunkField>([this, config](FChunkCoords coords) {
				return AInstancesChunk::Spawn(
					GetWorld(), coords, coords.makeSeed(seed),
					terrainField.get(), config);
				}, config->chunk)));
		}
	}
}

void AProcTerrain::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (terrainField && terrainField->Update(GetWorld()))
	{
		for (const auto& field : instancesFields)
		{
			if (!field->Update(GetWorld())) // update one chunk per tick
				break;
		}
	}
}
