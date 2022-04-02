#pragma once

#include "Engine.h"
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Runtime/Core/Public/Misc/MessageDialog.h"


DECLARE_LOG_CATEGORY_EXTERN(LogSpearInit, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSpearDebug, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSpearTrace, Log, All);


enum class PrintSameLineKey
{
	ProcTerrainStats
};

#define PRINT_SAMELINE(Key, Format, ...) \
	{ \
	if (GEngine) { \
		FColor printColor = FColor::Green; \
		FString printMessage = FString::Printf(Format, ##__VA_ARGS__); \
		GEngine->AddOnScreenDebugMessage(static_cast<int>(PrintSameLineKey::Key), 5.0f, printColor, printMessage); \
	} \
}

#define PRINT_SAMELINE_N_LOG(Key, Format, ...) \
	{ \
	if (GEngine) { \
		FColor printColor = FColor::Green; \
		FString printMessage = FString::Printf(Format, ##__VA_ARGS__); \
		GEngine->AddOnScreenDebugMessage(static_cast<int>(PrintSameLineKey::Key), 5.0f, printColor, printMessage); \
	} \
	UE_LOG(LogModTrace, Display, Format, ##__VA_ARGS__); \
}


#define PRINT_N_LOG(CategoryName, Verbosity, Format, ...) \
	{ \
	if (GEngine) { \
		FColor printColor = FColor::Silver; \
		if (ELogVerbosity::Verbosity == ELogVerbosity::Warning) printColor = FColor::Orange; \
		else if (ELogVerbosity::Verbosity == ELogVerbosity::Error) printColor = FColor::Red; \
		FString printMessage = FString::Printf(Format, ##__VA_ARGS__); \
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15.0f, printColor, printMessage); \
	} \
	UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__); \
}


#define ERROR_MESSAGE(CategoryName, Format, ...) \
	{ \
	FString msg = FString::Printf(TEXT(__FUNCSIG__ " : " Format), ##__VA_ARGS__); \
	FText title = FText::FromString("Error"); \
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(msg), &title); \
	UE_LOG(CategoryName, Error, TEXT("%s"), *msg); \
}

