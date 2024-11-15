#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "InteractiveRadius.generated.h"

class AItemBase;

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class MYGAME_API UInteractiveRadius : public USphereComponent
{
	GENERATED_BODY()

private:
	// Item actor, owner of this class
	TWeakObjectPtr<AItemBase> ItemBase;

public:
	UInteractiveRadius();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Init function called in BeginPlay
	void Init();

};
