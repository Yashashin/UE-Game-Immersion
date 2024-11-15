// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AssassinateInterface.generated.h"

UINTERFACE(MinimalAPI)
class UAssassinate : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class MYGAME_API IAssassinate
{
	GENERATED_BODY()

public:
	virtual void StealthBackAssassinate(FVector& RefLocation, FRotator& AssassinateRotation) = 0;
};