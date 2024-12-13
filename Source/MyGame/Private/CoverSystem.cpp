

#include "CoverSystem.h"
#include "MyGameCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "InputActionValue.h"
#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


#ifndef COVER_SYSTEM_MACRO
#define ATTACH_TO_COVER_TRACE_DISTANCE 100.0f
#define COVER_SNEAK_MAX_SPEED 300.0f 
#endif // !COVER_SYSTEM_MACRO



UCoverSystem::UCoverSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	StandToCoverMontageEndDelegate.BindUFunction(this, "OnStandToCoverMontageEnd");

	bIsCovering = false;
	bIsCoverFacingLeft = false;

}

void UCoverSystem::Init()
{
	MyGameCharacter = Cast<AMyGameCharacter>(GetOwner());
}

bool UCoverSystem::DetectFrontCover(FHitResult& HitResult)
{
	FVector Start = MyGameCharacter->GetActorLocation();
	FVector End = Start + MyGameCharacter->GetActorForwardVector() * ATTACH_TO_COVER_TRACE_DISTANCE;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::None, HitResult, true))
	{
		if (HitResult.GetActor()->ActorHasTag("Cover"))
		{
			return true;
		}
		return false;
	}
	return false;
}

bool UCoverSystem::DetectBehindCover(FHitResult& HitResult)
{
	FVector Start = MyGameCharacter->GetActorLocation();
	FVector End = Start + MyGameCharacter->GetActorForwardVector() * ATTACH_TO_COVER_TRACE_DISTANCE*-1.0f;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::None, HitResult, true))
	{
		if (HitResult.GetActor()->ActorHasTag("Cover"))
		{
			return true;
		}
		return false;
	}
	return false;
}

void UCoverSystem::AttachToCover(FHitResult HitResult)
{
	FVector TargetLocation = HitResult.Location + MyGameCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius() * HitResult.Normal;
	FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(HitResult.Normal);

	FMotionWarpingTarget* Target = new FMotionWarpingTarget();
	Target->Name = FName("WalkToCoverWarp");
	Target->Location = TargetLocation;
	Target->Rotation = TargetRotation;
	MyGameCharacter->MotionWarping->AddOrUpdateWarpTarget(*Target);

	UAnimInstance* AnimInstance = MyGameCharacter->GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(StandToCoverMontage);
	AnimInstance->Montage_SetEndDelegate(StandToCoverMontageEndDelegate,StandToCoverMontage);

	MyGameCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	MyGameCharacter->GetCharacterMovement()->MaxFlySpeed = COVER_SNEAK_MAX_SPEED;
	MyGameCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	bIsCovering = true;
}

void UCoverSystem::CoverSneakMovement(FVector Direction, float ScaleValue)
{
	FHitResult HitResult;
	if (DetectBehindCover(HitResult))
	{
		MyGameCharacter->AddMovementInput(Direction, ScaleValue);
		MyGameCharacter->SetActorRotation(UKismetMathLibrary::MakeRotFromX(HitResult.Normal));
		//ClimbRotation = UKismetMathLibrary::MakeRotFromX(HitResult.Normal * -1.0f);
		//CalculateMantleLedge();
		//ClimbAlignToSurfaceTimeline->PlayFromStart();
	}
	else
	{
		//StopClimbing();
	}
}

void UCoverSystem::StopCovering()
{
	MyGameCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	MyGameCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	FRotator Rotation = FRotator::ZeroRotator;
	Rotation.Yaw = MyGameCharacter->GetActorRotation().Yaw - 180.0f;
//	MyGameCharacter->SetActorRotation(Rotation);

	UAnimInstance* AnimInstance = MyGameCharacter->GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(CoverToStandMontage);
	bIsCovering = false;
}

void UCoverSystem::OnStandToCoverMontageEnd()
{
	
}


// Called when the game starts
void UCoverSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCoverSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

