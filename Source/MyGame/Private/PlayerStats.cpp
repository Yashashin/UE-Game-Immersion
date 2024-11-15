#include "PlayerStats.h"

#include "MyGameCharacter.h"
#include "MyGameInstance.h"


UPlayerStats::UPlayerStats()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPlayerStats::BeginPlay()
{
	Super::BeginPlay();

	Init();
}


// Called every frame
void UPlayerStats::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


bool UPlayerStats::DecreaseCurrentHealth(float Value)
{
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		CurrentHealth -= Value;
		if (CurrentHealth <= 0)
		{
			CurrentHealth = 0;
			MyGameCharacterPtr->UpdateUINotify(EUpdateUI::HUD_HealthBar);
			return true;
		}
		MyGameCharacterPtr->UpdateUINotify(EUpdateUI::HUD_HealthBar);
	}
	return false;
}

void UPlayerStats::IncreaseCurrentHealth(float Value)
{
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		CurrentHealth += Value;
		if (CurrentHealth > MaxHealth)
		{
			CurrentHealth = MaxHealth;
		}
		MyGameCharacterPtr->UpdateUINotify(EUpdateUI::HUD_HealthBar);
	}
}

void UPlayerStats::DecreaseMaxHealth(float Value)
{
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MaxHealth -= Value;
		MyGameCharacterPtr->UpdateUINotify(EUpdateUI::HUD_HealthBar);
	}
}

void UPlayerStats::IncreaseMaxHealth(float Value)
{
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MaxHealth += Value;
		MyGameCharacterPtr->UpdateUINotify(EUpdateUI::HUD_HealthBar);
	}
}



//bool UPlayerStats::IncreaseCurrentXP(int Value)
//{
//	CurrentXP += Value;
//	if (CurrentXP >= MaxXP)
//	{
//		LevelUp();
//		return true;
//	}
//	return false;
//}
//
//void UPlayerStats::IncreaseMaxXP(int Value)
//{
//	MaxXP += Value;
//}
//
//void UPlayerStats::LevelUp()
//{
//	Level++;
//	CurrentXP = 0;
//
//	// Update max xp
//	MaxXP = LevelUpXpCurve.EditorCurveData.Eval(Level);
//}

void UPlayerStats::Init()
{
	MyGameCharacter = Cast<AMyGameCharacter>(GetOwner());
	
}

