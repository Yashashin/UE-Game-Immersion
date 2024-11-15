
#include "ItemBase.h"

#include "Components/WidgetComponent.h"
#include "MyGameCharacter.h"
#include "Interact/InteractiveRadius.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	InteractiveRadius = CreateDefaultSubobject<UInteractiveRadius>(TEXT("Interactive Radius"));
	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Wdiget Component"));


	Mesh->SetupAttachment(GetRootComponent());
	InteractiveRadius->SetupAttachment(Mesh);
	InteractWidgetComponent->SetupAttachment(Mesh);
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
	InteractWidgetComponent->SetVisibility(false);
}

void AItemBase::EnableInteraction(AActor* Player)
{
	// Set interact mode
	AMyGameCharacter* MyGameCharacter = Cast<AMyGameCharacter>(Player);
	if (MyGameCharacter)
	{
		MyGameCharacter->InteractSystem->InteractMode = InteractMode;
		MyGameCharacter->InteractSystem->InteractingTarget = this;
	}

	// Show interactive widget
	InteractWidgetComponent->SetVisibility(true);
}

void AItemBase::DisableInteraction(AActor* Player)
{
	// Set interact mode
	AMyGameCharacter* MyGameCharacter = Cast<AMyGameCharacter>(Player);
	if (MyGameCharacter)
	{
		MyGameCharacter->InteractSystem->InteractMode = EInteractMode::None;
		MyGameCharacter->InteractSystem->InteractingTarget = nullptr;
	}

	// Hide interactive widget
	InteractWidgetComponent->SetVisibility(false);
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

