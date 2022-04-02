#include "TerrainChunk.h"
#include "DrawDebugHelpers.h"
#include "Spear/Logging.h"

#include "ProceduralMeshComponent.h"
#include "Math/SimplexNoise.h"
#include "ChunkConfig.h"

ASimplexTerrainChunk* ASimplexTerrainChunk::Spawn(
	UWorld* world,
	FChunkCoords coords,
	const SimplexNoise* noise,
	const FTerrainChunkConfig* config)
{
	check(world);
	check(noise);
	check(config);

	const FVector location = coords.worldLocation(config->chunk.size, world->OriginLocation);
	auto chunk = (ASimplexTerrainChunk*)world->SpawnActor(ASimplexTerrainChunk::StaticClass(), &location);

	chunk->coords = coords;
	chunk->noise = noise;
	chunk->config = config;

	chunk->StartGenTask();
	return chunk;
}

ASimplexTerrainChunk::ASimplexTerrainChunk()
{
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Terrain"));
	RootComponent = mesh;
	mesh->bUseAsyncCooking = true;
}

ASimplexTerrainChunk::~ASimplexTerrainChunk()
{
	if (genTask)
	{
		genTask->Cancel();
		genTask->EnsureCompletion();
		genTask.reset();
	}
}

void ASimplexTerrainChunk::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TryFinishGenTask();
	SetActorTickEnabled(!IsDone());
}

bool ASimplexTerrainChunk::IsDone()
{
	return !genTask;
}

void ASimplexTerrainChunk::Disable()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	ShowDebugBox(FColor::Red, 3.0f);
}

void ASimplexTerrainChunk::Enable()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	ShowDebugBox(FColor::Magenta, 3.0f);

	if (config->chunk.showDebug)
		PRINT_N_LOG(LogSpearDebug, Display, TEXT("TerrainChunk re-enabled at (%i, %i) !!"),
			coords.x, coords.y);
}

void ASimplexTerrainChunk::RecycleTo(FChunkCoords newCoords)
{
	isRecycled = true;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	if (coords != newCoords)
	{
		coords = newCoords;
		SetActorLocation(coords.worldLocation(config->chunk.size, GetWorld()->OriginLocation));
		StartGenTask();  // regenerate
		ShowDebugBox(FColor::Yellow, 3.0f);
	}
	else
		ShowDebugBox(FColor::Green, 3.0f);


	if (config->chunk.showDebug)
		PRINT_N_LOG(LogSpearDebug, Display, TEXT("TerrainChunk recycled to (%i, %i)"),
			coords.x, coords.y);
}

void ASimplexTerrainChunk::ShowDebugBox(FColor color, float lifetime) const
{
	if (config->chunk.showDebug)
	{
		const float extents = config->chunk.size / 2;
		FVector location = GetActorLocation();
		const auto extentsV = FVector(extents, extents, extents);
		DrawDebugBox(GetWorld(), location + extentsV, extentsV, color, false, lifetime, 0, 100.0f);
	}
}

void ASimplexTerrainChunk::StartGenTask()
{
	vertices.Reset();
	genTask = std::make_unique<FAsyncTask<FGenGroundTask>>(
		coords, noise, config, vertices);

	genTask->StartBackgroundTask();

	ShowDebugBox(FColor::Blue, 3.0f);
}

void ASimplexTerrainChunk::TryFinishGenTask()
{
	if (!genTask)
		return; // not started or already completed

	if (genTask->IsDone())
	{
		const auto task = genTask->GetTask();

		if (isRecycled)
		{
			mesh->UpdateMeshSection(0, vertices, task.normals, {}, {}, task.tangents);
		}
		else
		{
			mesh->CreateMeshSection(0, vertices, task.triangles, task.normals, {}, {}, task.tangents, true);
			mesh->ContainsPhysicsTriMeshData(true);
			mesh->SetMaterial(0, config->material);
		}

		genTask.reset();
	}
}

float ASimplexTerrainChunk::GetHeight(const FVector& pos) const
{
	check(vertices.Num() == (config->quads + 1) * (config->quads + 1));

	const auto location = GetActorLocation();
	const float xFloat = config->quads * (pos.X - location.X) / config->chunk.size;
	const float yFloat = config->quads * (pos.Y - location.Y) / config->chunk.size;

	// interpolate height inside a quad
	const int xCeil = FMath::Clamp(FMath::CeilToInt(xFloat), 0, config->quads);
	const int yCeil = FMath::Clamp(FMath::CeilToInt(yFloat), 0, config->quads);
	const int xFloor = FMath::Clamp(FMath::FloorToInt(xFloat), 0, config->quads);
	const int yFloor = FMath::Clamp(FMath::FloorToInt(yFloat), 0, config->quads);

	const float fracXFloor = xCeil - xFloat;
	const float fracYFloor = yCeil - yFloat;
	const float fracXCeil = 1.0f - fracXFloor;
	const float fracYCeil = 1.0f - fracYFloor;

	const int quadsPlus1 = config->quads + 1;
	return vertices[xFloor + yFloor * quadsPlus1].Z * fracXFloor * fracYFloor
		+ vertices[xCeil + yFloor * quadsPlus1].Z * fracXCeil * fracYFloor
		+ vertices[xCeil + yCeil * quadsPlus1].Z * fracXCeil * fracYCeil
		+ vertices[xFloor + yCeil * quadsPlus1].Z * fracXFloor * fracYCeil;
}