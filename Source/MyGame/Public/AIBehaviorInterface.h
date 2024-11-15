// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIBehaviorInterface.generated.h"


/**
 *
 */
UINTERFACE(MinimalAPI)
class UAIBehavior : public UInterface
{
	GENERATED_BODY()
};

class MYGAME_API IAIBehavior
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float Attack();
};
