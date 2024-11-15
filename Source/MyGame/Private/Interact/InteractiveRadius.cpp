#include "Interact/InteractiveRadius.h"

#include "Interact/InteractSystem.h"
#include "ItemBase.h"
#include "MyGameCharacter.h"
#include "Blueprint/UserWidget.h"

UInteractiveRadius::UInteractiveRadius()
{	
	// Bind shpere component overlap functions
	OnComponentBeginOverlap.AddDynamic(this, &UInteractiveRadius::OnOverlapBegin);
	OnComponentEndOverlap.AddDynamic(this, &UInteractiveRadius::OnOverlapEnd);
}

void UInteractiveRadius::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void UInteractiveRadius::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Player enters interactive radius
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		if (ItemBase.IsValid())
		{
			AItemBase* ItemBasePtr = ItemBase.Get();
			ItemBasePtr->EnableInteraction(OtherActor);
		}
	}
}

void UInteractiveRadius::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Player leaves interactive radius
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		if (ItemBase.IsValid())
		{
			AItemBase* ItemBasePtr = ItemBase.Get();
			ItemBasePtr->DisableInteraction(OtherActor);
		}
	}
}

void UInteractiveRadius::Init()
{
	ItemBase = Cast<AItemBase>(GetOwner());

}

