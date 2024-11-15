// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractSystem.generated.h"

UENUM()
enum class EInteractMode :uint8
{
	Talk,
	PickItem,
	None,
};

class AMyGameCharacter;
class USphereComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAME_API UInteractSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	// This mode indicate what interaction to do
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category= "Interact")
	EInteractMode InteractMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact") // Set in blueprint
	TObjectPtr<UAnimMontage> PickItemMontage;

	// Target to interact with
	AActor* InteractingTarget = nullptr;

private:
	// Access my game character
	TWeakObjectPtr<AMyGameCharacter> MyGameCharacter;

	// Delegate binds pick item montage and OnPickItemMontageEnd()
	FOnMontageEnded PickItemMontageEndDelegate;

public:	
	UInteractSystem();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void DoInteraction();

	UFUNCTION()
	void Talk();

	/*UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Init function called in BeginPlay
	void Init();

	UFUNCTION()
	void PickItem();

	UFUNCTION()
	void OnPickItemMontageEnd();
		
};
