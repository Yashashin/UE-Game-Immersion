// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestStruct.generated.h"

UENUM(BlueprintType)
enum class EQuestType : uint8
{
	MainStory UMETA(DisplayName = "Main"),
	Side UMETA(DisplayName = "Side"),
	None UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
	Fetch UMETA(DisplayName = "Fetch"),
	Kill UMETA(DisplayName = "Kill"),
	Delivery UMETA(DisplayName = "Delivery"),
	Escort UMETA(DisplayName = "Escort"),
	Puzzle UMETA(DisplayName = "Puzzle"),
	Exploration UMETA(DisplayName = "Exploration"),
	Boss UMETA(DisplayName = "Boss"),
	Investigation UMETA(DisplayName = "Investigation"),
	None UMETA(DisplayName = "None"),
};

USTRUCT(BlueprintType)
struct MYGAME_API FQuestObjective
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Quest")
	FString Description;

	UPROPERTY(VisibleAnywhere, Category = "Quest")
	bool bIsCompleted;

	UPROPERTY(VisibleAnywhere, Category = "Quest")
	EObjectiveType Type;

	FQuestObjective() :
		Description(TEXT("")),
		bIsCompleted(false),
		Type(EObjectiveType::None)
	{
	}
};

USTRUCT(BlueprintType)
struct MYGAME_API FQuestData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Quest")
	int32 ID;

	UPROPERTY(VisibleAnywhere, Category = "Quest")
	FString Name;

	UPROPERTY(VisibleAnywhere, Category = "Quest")
	FString Description;

	UPROPERTY(VisibleAnywhere, Category = "Quest")
	TArray<FQuestObjective> Objectives;

	UPROPERTY(VisibleAnywhere, Category = "Quest")
	bool bIsCompleted;

	UPROPERTY(VisibleAnywhere, Category = "Quest")
	EQuestType Type;

	FQuestData() :
		ID(0),
		Name(TEXT("")),
		Description(TEXT("")),
		bIsCompleted(false),
		Type(EQuestType::None)
	{
	}
};
