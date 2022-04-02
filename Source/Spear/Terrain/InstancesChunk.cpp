#include "InstancesChunk.h"
#include "DrawDebugHelpers.h"
#include "Spear/Logging.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Tasks/GenPoissonTask.h"
#include "ChunkConfig.h"
#include "ChunkField.h"

AInstancesChunk* AInstancesChunk::Spawn(
	UWorld* world,
	FChunkCoords coords,
	int seed, 
	const ChunkField* terrainField,
	const FInstancesChunkConfig* config)
{
	check(world);
	check(terrainField);
	check(config);

	const FVector location = coords.worldLocation(config->chunk.size, world->OriginLocation);
	auto chunk = (AInstancesChunk*)world->SpawnActor(AInstancesChunk::StaticClass(), &location);

	chunk->coords = coords;
	chunk->seed = seed;
	chunk->terrainField = terrainField;
	chunk->config = config;

	chunk->StartGenTask();
	return chunk;
}

AInstancesChunk::AInstancesChunk()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default"));
}

AInstancesChunk::~AInstancesChunk()
{
	if (genTask)
	{
		genTask->Cancel();
		genTask->EnsureCompletion();
		genTask.reset();
	}
}

void AInstancesChunk::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TryFinishGenTask();
	SetActorTickEnabled(!IsDone());
}

bool AInstancesChunk::IsDone()
{
	return !genTask;
}

void AInstancesChunk::Disable()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	ShowDebugBox(FColor::Red, 3.0f);
}

void AInstancesChunk::Enable()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	ShowDebugBox(FColor::Magenta, 3.0f);
}

void AInstancesChunk::RecycleTo(FChunkCoords newCoords)
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
	}
	else
		ShowDebugBox(FColor::Green, 3.0f);


	if (config->chunk.showDebug)
		PRINT_N_LOG(LogSpearDebug, Display, TEXT("InstancesChunk recycled to (%i, %i)"),
			coords.x, coords.y);
}



void AInstancesChunk::ShowDebugBox(FColor color, float lifetime)
{
	if (config->chunk.showDebug)
	{
		const float extents = config->chunk.size / 2;
		FVector location = GetActorLocation();
		const auto extentsV = FVector(extents, extents, extents);
		DrawDebugBox(GetWorld(), location + extentsV, extentsV, color, false, lifetime, 0, 100.0f);
	}
}

void AInstancesChunk::StartGenTask()
{
	if (genTask || !config->enabled || !config->Class)
		return;

	if (!mesh) // freshly created, not recycled
	{
		mesh = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, config->Class, TEXT("Instances"), RF_Transient);
		mesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		mesh->RegisterComponent();
		ShowDebugBox(FColor::Blue, 3.0f);
	}
	else
	{
		ShowDebugBox(FColor::Yellow, 3.0f);
	}

	const auto terrain = static_cast<ITerrainChunk*>(
		terrainField->GetChunkAt(GetActorLocation() + FVector(config->chunk.size / 2, config->chunk.size / 2, 0),
			GetWorld()->OriginLocation));

	if (terrain)
	{
		genTask = std::make_unique<FAsyncTask<FGenPoissonTask>>(seed, GetActorLocation(), terrain, config);
		genTask->StartBackgroundTask();
	}
	else
	{
		PRINT_N_LOG(LogSpearDebug, Error, TEXT("InstancesChunk has no underlying terrain at (%i, %i)"),
			coords.x, coords.y);
	}
}

void AInstancesChunk::TryFinishGenTask()
{
	if (!genTask)
		return; // not started or already completed

	if (genTask->IsDone())
	{
		mesh->ClearInstances();
		mesh->AddInstances(genTask->GetTask().transforms, false);

		genTask.reset();
	}
}