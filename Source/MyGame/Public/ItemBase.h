#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interact/InteractSystem.h"
#include "ItemBase.generated.h"

class UInteractiveRadius;
class UWidgetComponent;

UCLASS(BlueprintType, Blueprintable)
class MYGAME_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Mesh of this item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) // Set static mesh in blueprint
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact")
	TObjectPtr<UInteractiveRadius> InteractiveRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact") // Set widget in blueprint
	TObjectPtr<UWidgetComponent> InteractWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interact") // Set in blueprint
	EInteractMode InteractMode = EInteractMode::None;

public:
	AItemBase();

	virtual void Tick(float DeltaTime) override;

	void EnableInteraction(AActor* Player);
	void DisableInteraction(AActor* Player);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
