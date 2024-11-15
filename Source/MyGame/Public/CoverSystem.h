// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoverSystem.generated.h"

class AMyGameCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAME_API UCoverSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Cover")  // Set it in blueprint
	TObjectPtr<UAnimMontage> StandToCoverMontage;

	UPROPERTY(EditAnywhere, Category = "Cover")  // Set it in blueprint
	TObjectPtr<UAnimMontage> CoverToStandMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover")
	float CoverSneakLeftAndRightValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover")
	bool bIsCovering;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover")
	bool bIsCoverFacingLeft;

private:
	TObjectPtr<AMyGameCharacter> MyGameCharacter;

	FOnMontageEnded StandToCoverMontageEndDelegate;

public:	
	UCoverSystem();

	UFUNCTION()
	void Init();

	UFUNCTION()
	bool DetectFrontCover(FHitResult& HitResult);

	UFUNCTION()
	bool DetectBehindCover(FHitResult& HitResult);

	UFUNCTION()
	void AttachToCover(FHitResult HitResult);

	UFUNCTION()
	void CoverSneakMovement(FVector Direction, float ScaleValue);

	UFUNCTION()
	void StopCovering();

	UFUNCTION()
	void OnStandToCoverMontageEnd();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
