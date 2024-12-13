// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMovementSpeeds
{
    GENERATED_BODY()

    // X for forward. Y for strafe. Z for backward.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector WalkSpeeds;

    // X for forward. Y for strafe. Z for backward.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector RunSpeeds;

    // X for forward. Y for strafe. Z for backward.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector SprintSpeeds;

    // X for forward. Y for strafe. Z for backward.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector CrouchSpeeds;

    FMovementSpeeds()
    {
        WalkSpeeds = FVector(200.0f, 175.0f, 150.0f);
        RunSpeeds= FVector(400.0f, 350.0f, 400.0f);
        SprintSpeeds= FVector(700.0f, 650.0f, 600.0f);
        CrouchSpeeds = FVector(200.0f, 175.0f, 150.0f);
    }
   
};

USTRUCT(BlueprintType)
struct FMovementSpeedsSet
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMovementSpeeds NormalSpeeds;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMovementSpeeds ArmedSpeeds;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMovementSpeeds AimingSpeeds;

    FMovementSpeedsSet()
    {

    }
};