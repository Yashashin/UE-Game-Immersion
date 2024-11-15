// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Equipment.generated.h"

struct FEquipmentStruct;

UCLASS()
class MYGAME_API AEquipment : public AActor
{
	GENERATED_BODY()

public:
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	TObjectPtr<UDataTable> DataTable;

	int DataIndex;

public:	
	// Sets default values for this actor's properties
	AEquipment();

	void ChangeData(int DataIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
