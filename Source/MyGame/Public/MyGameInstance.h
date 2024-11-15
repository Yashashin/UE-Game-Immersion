// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"


UENUM(BlueprintType)
enum class EGameState : uint8
{
	MAINMENU UMETA(DisplayName = "Main Menu"),
	INGAME UMETA(DisplayName = "In Game"),
	INGAMEMENU UMETA(DisplayName = "In Game Menu"),
};


/**
 *
 */
UCLASS()
class MYGAME_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	EGameState GameState = EGameState::INGAME;

	// In-game HUD widget class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI") // Set in blueprint
	TSubclassOf<UUserWidget> HUDWidgetClass;

	// In-game HUD widget
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UUserWidget> HUDWidget;

	// In-game menu widget class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI") // Set in blueprint
	TSubclassOf<UUserWidget> InGameMenuWidgetClass;

	// In-game menu widget
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UUserWidget> InGameMenuWidget;

	// Shooting widget class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI") // Set in blueprint
	TSubclassOf<UUserWidget> ShootingWidgetClass;

	// Shooting widget 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI") 
	TObjectPtr<UUserWidget> ShootingWidget;

public:
	void CreateHUDWidget();

	void CreateInGameMenuWidget();

	void CreateShootingWidget();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleHUDWidget();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleInGameMenuWidget();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleShootingWidget();

public:
	virtual void Init() override;
};
