#include "Interact/InteractSystem.h"
#include "Interact/InteractInterface.h"

#include "MyGameCharacter.h"
#include "Components/SphereComponent.h"
#include "InputActionValue.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Quest/QuestGiverInterface.h"


UInteractSystem::UInteractSystem()
{
	PrimaryComponentTick.bCanEverTick = true;

	PickItemMontageEndDelegate.BindUFunction(this, "OnPickItemMontageEnd");
}

void UInteractSystem::Init()
{
	MyGameCharacter = Cast<AMyGameCharacter>(GetOwner());
}

void UInteractSystem::DoInteraction()
{
	switch (InteractMode)
	{
	case EInteractMode::Talk:
		Talk();
		break;
	case EInteractMode::PickItem:
		PickItem();
		break;
	case EInteractMode::None:
		break;
	default:
		break;
	}
}

void UInteractSystem::Talk()
{
	TArray<AActor*> Actors;
	/*InteractRadius->GetOverlappingActors(Actors);
	for (AActor* Actor : Actors)
	{
		if (UKismetSystemLibrary::DoesImplementInterface(Actor, UQuestGiver::StaticClass()))
		{
			IQuestGiver::Execute_GiveQuest(Actor);
		}
	}*/
}

void UInteractSystem::OnPickItemMontageEnd()
{
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		MyGameCharacterPtr->bEnableTurnInPlace = false;
	}
}

void UInteractSystem::PickItem()
{
	if (MyGameCharacter.IsValid() && IsValid(InteractingTarget))
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();

		FVector StartLocation = MyGameCharacterPtr->GetActorLocation(); 
		FVector TargetLocation= InteractingTarget->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
		LookAtRotation.Pitch = MyGameCharacterPtr->GetActorRotation().Pitch; // Leave pitch unchanged

		// Motion warping
		FMotionWarpingTarget* Target = new FMotionWarpingTarget();
		Target->Name = FName("Pick Item Target");
		Target->Rotation = LookAtRotation;
		MyGameCharacterPtr->MotionWarpingComponent->AddOrUpdateWarpTarget(*Target);

		// Play pick item montage
		UAnimInstance* AnimInstance = MyGameCharacterPtr->GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(PickItemMontage);
		AnimInstance->Montage_SetEndDelegate(PickItemMontageEndDelegate);

		// We want the lower body to be oriented towards the controller's direction after the montage ends.
		// However, since our design doesn't allow the controller to affect the lower body's direction,
		// we need to enable the 'turn in place' feature to maintain correct orientation during the montage playback.
		// By doing this, the character can face the correct direction after transitioning from the montage to locomotion.
		MyGameCharacterPtr->bEnableTurnInPlace = true;
	}
}

//void UInteractSystem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor->ActorHasTag(TEXT("NPC")))
//	{
//		if (UKismetSystemLibrary::DoesImplementInterface(OtherActor, UInteract::StaticClass()))
//		{
//			IInteract::Execute_ShowInteractWidget(OtherActor);
//		}
//	}
//}
//
//void UInteractSystem::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	if (OtherActor->ActorHasTag(TEXT("NPC")))
//	{
//		if (UKismetSystemLibrary::DoesImplementInterface(OtherActor, UInteract::StaticClass()))
//		{
//			IInteract::Execute_HideInteractWidget(OtherActor);
//		}
//	}
//}


void UInteractSystem::BeginPlay()
{
	Super::BeginPlay();
	Init();
}


void UInteractSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

