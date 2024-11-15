// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Controller.h"

void UMyGameInstance::CreateHUDWidget()
{
	if (!HUDWidget)
	{
		HUDWidget = CreateWidget(GetWorld(), HUDWidgetClass);
		if (IsValid(HUDWidget))
		{
			HUDWidget->AddToViewport();
		}
	}
}

void UMyGameInstance::CreateInGameMenuWidget()
{
	if (!InGameMenuWidget)
	{
		InGameMenuWidget = CreateWidget(GetWorld(), InGameMenuWidgetClass);
	}
}

void UMyGameInstance::CreateShootingWidget()
{
	if (!ShootingWidget)
	{
		ShootingWidget = CreateWidget(GetWorld(), ShootingWidgetClass);
	}
}



void UMyGameInstance::ToggleInGameMenuWidget()
{
	if (InGameMenuWidget->IsInViewport())
	{
		InGameMenuWidget->RemoveFromViewport();
		APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
	}
	else
	{
		InGameMenuWidget->AddToViewport();
		InGameMenuWidget->SetKeyboardFocus();
		APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(InGameMenuWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);
	}
	
}

void UMyGameInstance::ToggleShootingWidget()
{
	if (ShootingWidget->IsInViewport())
	{
		ShootingWidget->RemoveFromViewport();
		/*APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);*/
	}
	else
	{
		ShootingWidget->AddToViewport();
		/*InGameMenuWidget->SetKeyboardFocus();
		APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(InGameMenuWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);*/
	}
}

void UMyGameInstance::ToggleHUDWidget()
{
	if (HUDWidget->IsVisible())
	{
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		HUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMyGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("Game Instance Init!"));
}
