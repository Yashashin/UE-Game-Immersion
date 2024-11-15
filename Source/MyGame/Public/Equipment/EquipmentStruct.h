// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentStruct.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Melee UMETA(DisplayName = "Melee"),
	Ranged UMETA(DisplayName = "Ranged"),
	Thrown UMETA(DisplayName = "Thrown"),
};

/**
 *
 */
USTRUCT(BlueprintType)
struct MYGAME_API FEquipmentStruct : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Name"))
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Description"))
	FText Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Icon"))
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Static Mesh"))
	TObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Type"))
	EItemType Type = EItemType::Melee;

public:
};
