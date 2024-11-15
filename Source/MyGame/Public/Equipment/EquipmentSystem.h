
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "EquipmentSystem.generated.h"

class AMyGameCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAME_API UEquipmentSystem : public UActorComponent
{
	GENERATED_BODY()

private:
	TObjectPtr<AMyGameCharacter> MyGameCharacter;


public:	
	// Sets default values for this component's properties
	UEquipmentSystem();

	void Init();


	void DetectEquipmentNearby();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
