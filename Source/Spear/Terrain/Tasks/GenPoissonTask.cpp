#include "GenPoissonTask.h"

#include "../ChunkConfig.h"
#include "../Chunk.h"
#include "../Math/PoissonDiskSampling.h"

FGenPoissonTask::FGenPoissonTask(int seed,
	FVector location,
	const ITerrainChunk* const terrain,
	const FInstancesChunkConfig* const config)
	: seed(seed),
	location(location),
	terrain(terrain),
	config(config)
{
}

void FGenPoissonTask::DoWork()
{
	const auto min = std::array<float, 2>{ {0, 0}};
	const auto max = std::array<float, 2>{ {config->chunk.size, config->chunk.size}};
	const auto placements = thinks::PoissonDiskSampling(config->spacing, min, max, 5, seed);

	FTransform transform = FTransform();

	for (const auto& placement : placements)
	{
		if (abort)
			return;

		const auto& posX = placement[0];
		const auto& posY = placement[1];

		const float scale = FMath::FRand() * 0.6f + 0.7f;
		transform.SetScale3D({ 1.0f, scale, scale });
		transform.SetRotation({ FVector::UpVector, FMath::FRand() * PI * 2.0f });

		const float height = terrain->GetHeight(location + FVector(posX, posY, 0));
		transform.SetLocation({posX, posY, height });

		transforms.Add(transform);
	}
}