// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ProgressBar.h"
#include "PlayerStats.generated.h"

class AMyGameCharacter;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAME_API UPlayerStats : public UActorComponent
{
	GENERATED_BODY()

public:
	// Maxmun health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float MaxHealth = 100.0f;

	// Current health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth = 100.0f;

	//// Current level
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	//int Level = 1;

	//// A curve that maps the current level and the xp for level-up
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	//FRuntimeFloatCurve LevelUpXpCurve;

	//// Total XP needed for level-up
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	//int MaxXP = 10.0f;

	//// Current xp
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	//int CurrentXP;

private:
	// Access owner
	TWeakObjectPtr<AMyGameCharacter> MyGameCharacter;

public:	
	UPlayerStats();

	UFUNCTION(BlueprintCallable)
	bool DecreaseCurrentHealth(float Value);

	UFUNCTION(BlueprintCallable)
	void IncreaseCurrentHealth(float Value);

	UFUNCTION(BlueprintCallable)
	void DecreaseMaxHealth(float Value);

	UFUNCTION(BlueprintCallable)
	void IncreaseMaxHealth(float Value);

	//UFUNCTION(BlueprintCallable)
	//bool IncreaseCurrentXP(int Value);

	//UFUNCTION(BlueprintCallable)
	//void IncreaseMaxXP(int Value);

	//UFUNCTION(BlueprintCallable)
	//void LevelUp();

	UFUNCTION(BlueprintPure)
	float GetHealthPercentage() const { return CurrentHealth / MaxHealth; }

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
private:
	void Init();
};
