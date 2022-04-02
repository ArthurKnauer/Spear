#include "DebugDraw.h"
#include "Engine.h"

void UDebugDrawBlueprintLibrary::DebugDrawStringPretty(
	UObject* callee,
	FVector TextLocation, 
	const FString& Text, 
	class AActor* TestBaseActor, 
	FLinearColor TextColor,
	float Duration, 
	bool bDrawShadow, 
	float FontScale)
{
#if !UE_BUILD_SHIPPING
	if (!callee)
		return;

	DrawDebugString(callee->GetWorld(), TextLocation, Text, TestBaseActor, TextColor.ToFColor(false), Duration, bDrawShadow, FontScale);
#endif
}