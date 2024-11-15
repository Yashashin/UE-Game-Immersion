
#include "./Equipment/EquipmentSystem.h"
#include "./Equipment/Equipment.h"

#include "Kismet/KismetSystemLibrary.h"
#include "MyGameCharacter.h"
#include "Camera/CameraComponent.h"


#ifndef EQUIPMENTSYSTEM_MACRO
#define EQUIPMENT_TRACE_SIZE 15.0f
#define EQUIPMENT_TRACE_DISTANCE 1000.0f

#endif // !EQUIPMENTSYSTEM_MACRO


// Sets default values for this component's properties
UEquipmentSystem::UEquipmentSystem( )
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEquipmentSystem::Init()
{
	MyGameCharacter = Cast<AMyGameCharacter>(GetOwner());
}





void UEquipmentSystem::DetectEquipmentNearby()
{
	/*FVector Start = MyGameCharacter->GetMesh()->GetSocketLocation(TEXT("head"));
	FVector End = MyGameCharacter->GetFollowCamera()->GetComponentLocation() + MyGameCharacter->GetFollowCamera()->GetForwardVector() * EQUIPMENT_TRACE_DISTANCE;

	TArray<FHitResult> HitResults;
	if (UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, EQUIPMENT_TRACE_SIZE, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, HitResults, true))
	{
		for (FHitResult Result : HitResults)
		{
			if (Cast<AEquipment>(Result.GetActor()))
			{
				Result.GetActor()->Destroy();
			}
		}
	}*/
}


// Called when the game starts
void UEquipmentSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEquipmentSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

