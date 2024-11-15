// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "ClimbSystem.generated.h"

class AMyGameCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAME_API UClimbSystem : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category = "Climb")
	TObjectPtr<UTimelineComponent> ClimbAlignToSurfaceTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb") // Set it in blueprint
	TObjectPtr<UCurveFloat> ClimbAlignToSurfaceCurve;

	UPROPERTY(VisibleAnywhere, Category = "Climb")
	FRotator ClimbRotation;

	UPROPERTY(EditAnywhere, Category = "Climb")  // Set it in blueprint
	TObjectPtr<UAnimMontage> MantleLedgeMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Climb")
	bool bIsClimbing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Climb")
	float ClimbUpAndDownValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Climb")
	float ClimbLeftAndRightValue;

private:
	TObjectPtr<AMyGameCharacter> MyGameCharacter;

	FTimerHandle TimerHandle;

	/** Timeline callback binding function */
	FOnTimelineFloat ClimbAlignToSurfaceTimelineDelegate;

	bool bIsPlayingMontage;

public:
	UClimbSystem();

	UFUNCTION()
	void Init();

	UFUNCTION()
	bool DetectWall(FHitResult& HitResult);

	UFUNCTION()
	void AttachToSurface(FHitResult HitResult);

	UFUNCTION()
	void ClimbMovement(FVector Direction, float ScaleValue);

	/** Timeline callback function */
	UFUNCTION()
	void ClimbAlignToSurfaceTimelineReturn(float value);

	UFUNCTION()
	void StopClimbing();

	UFUNCTION()
	void CalculateMantleLedge();

	UFUNCTION()
	void MantleLedgeMontageNotify();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
