#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "DebugDraw.generated.h"

UCLASS()
class UDebugDrawBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "DebugDraw", meta = (WorldContext = "callee"))
		static void DebugDrawStringPretty(UObject* callee,
			FVector TextLocation,
			const FString& Text,
			class AActor* TestBaseActor = NULL,
			FLinearColor TextColor = FLinearColor::Yellow,
			float Duration = 0.0f,
			bool bDrawShadow = true,
			float FontScale = 1.0f);
};