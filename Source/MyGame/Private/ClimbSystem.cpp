#include "ClimbSystem.h"
#include "MyGameCharacter.h"
#include "InputActionValue.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


#ifndef CLIMB_SYSTEM_MACRO
#define ATTACH_TO_SURFACE_TRACE_DISTANCE 100.0f
#define ATTACH_TO_SURFACE_OVERTIME 2.2f
#define TIMELINE_LENGTH 0.25f
#define MANTLE_LEDGE_HEAD_DISTANCE 110.0f
#define MANTLE_LEDGE_TRACE_DISTANCE 100.0f
#define MANTLE_LEDGE_WARP_X_FORWARD 48.0f
#define MANTLE_LEDGE_WARP_Y_FORWARD 48.0f
#endif // !CLIMB_SYSTEM_MACRO


void UClimbSystem::Init()
{
	MyGameCharacter = Cast<AMyGameCharacter>(GetOwner());
}

bool UClimbSystem::DetectWall(FHitResult& HitResult)
{
	FVector Start = MyGameCharacter->GetActorLocation();
	FVector End = Start + MyGameCharacter->GetActorForwardVector() * ATTACH_TO_SURFACE_TRACE_DISTANCE;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::None, HitResult, true))
	{
		return true;
	}
	return false;
}

void UClimbSystem::AttachToSurface(FHitResult HitResult)
{
	bIsClimbing = true;
	MyGameCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	MyGameCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;

	FVector TargetLocation = HitResult.Location + MyGameCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius() * HitResult.Normal;
	FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(HitResult.Normal * -1.0f);
	FLatentActionInfo LantentInfo;
	UKismetSystemLibrary::MoveComponentTo(MyGameCharacter->GetCapsuleComponent(), TargetLocation, TargetRotation, false, false, ATTACH_TO_SURFACE_OVERTIME, false, EMoveComponentAction::Move, LantentInfo);
}

void UClimbSystem::ClimbMovement(FVector Direction, float ScaleValue)
{
	FHitResult HitResult;
	if (DetectWall(HitResult))
	{
		MyGameCharacter->AddMovementInput(Direction, ScaleValue);
		ClimbRotation = UKismetMathLibrary::MakeRotFromX(HitResult.Normal * -1.0f);
		CalculateMantleLedge();
		ClimbAlignToSurfaceTimeline->PlayFromStart();
	}
	else
	{
		//StopClimbing();
	}
}

void UClimbSystem::ClimbAlignToSurfaceTimelineReturn(float value)
{
	FRotator LerpRotation = FMath::Lerp(MyGameCharacter->GetActorRotation(), ClimbRotation, value);
	MyGameCharacter->SetActorRotation(LerpRotation);
}

void UClimbSystem::StopClimbing()
{
	bIsClimbing = false;
	MyGameCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	MyGameCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			FRotator Rotation = FRotator::ZeroRotator;
			Rotation.Yaw = MyGameCharacter->GetActorRotation().Yaw;
			MyGameCharacter->SetActorRotation(Rotation);
			
		}, 0.05f, false);

}

void UClimbSystem::CalculateMantleLedge()
{
	FVector Start = MyGameCharacter->GetActorLocation() + FVector(0, 0, MANTLE_LEDGE_HEAD_DISTANCE);
	FVector End = Start + MyGameCharacter->GetActorForwardVector() * MANTLE_LEDGE_TRACE_DISTANCE;
	FHitResult HitResult;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, HitResult, true, FLinearColor::Blue))
	{
	}
	else
	{
		for (int i = 0; i < 20; i++)
		{
			FVector SecondStart = (MyGameCharacter->GetActorForwardVector() * i * 3) + HitResult.TraceStart + FVector(0, 0, 90);
			FVector SecondEnd = SecondStart - FVector(0, 0, 100);
			FHitResult SecondHitResult;
			if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), SecondStart, SecondEnd, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, SecondHitResult, true, FLinearColor::Green))
			{
				if (!bIsPlayingMontage) // do once flag
				{
					DrawDebugSphere(GetWorld(), SecondHitResult.Location, 16, 32, FColor::Cyan, false, 50.0f);

					FVector ActorLocation = MyGameCharacter->GetActorLocation();

					FMotionWarpingTarget* Target = new FMotionWarpingTarget();
					Target->Name = FName("LedgeClimbUp");
					Target->Location = FVector(ActorLocation.X + MyGameCharacter->GetActorForwardVector().X * MANTLE_LEDGE_WARP_X_FORWARD, ActorLocation.Y + MyGameCharacter->GetActorForwardVector().Y * MANTLE_LEDGE_WARP_Y_FORWARD, SecondHitResult.Location.Z);
					Target->Rotation = MyGameCharacter->GetActorRotation();
					MyGameCharacter->MotionWarpingComponent->AddOrUpdateWarpTarget(*Target);

					FMotionWarpingTarget* Target2 = new FMotionWarpingTarget();
					Target2->Name = FName("LedgeClimbForward");
					Target2->Location = FVector(SecondHitResult.Location.X, SecondHitResult.Location.Y, ActorLocation.Z + 115.0f);
					Target2->Rotation = MyGameCharacter->GetActorRotation();
					MyGameCharacter->MotionWarpingComponent->AddOrUpdateWarpTarget(*Target2);

					UAnimInstance* AnimInstance = MyGameCharacter->GetMesh()->GetAnimInstance();
					AnimInstance->Montage_Play(MantleLedgeMontage);
					//AnimInstance->Montage_SetEndDelegate(MantleLedgeMontageDelegate, MantleLedgeMontage);
					bIsPlayingMontage = true;
					MyGameCharacter->SetActorEnableCollision(false);
//					MyGameCharacter->GetCameraBoom()->bDoCollisionTest = false;
					break;
				}
			}
		}
	}

}

void UClimbSystem::MantleLedgeMontageNotify()
{
	MyGameCharacter->SetActorEnableCollision(true);
	//MyGameCharacter->GetCameraBoom()->bDoCollisionTest = true;
	StopClimbing();
	bIsPlayingMontage = false;
}




// Sets default values for this component's properties
UClimbSystem::UClimbSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ClimbAlignToSurfaceTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ClimbAlignToSurfaceTimeline"));
	ClimbAlignToSurfaceTimelineDelegate.BindUFunction(this, "ClimbAlignToSurfaceTimelineReturn");
	ClimbRotation = FRotator::ZeroRotator;

	bIsPlayingMontage = false;
	bIsClimbing = false;

	//MantleLedgeMontageDelegate.BindUFunction(this, "OnMantleLedgeMontageEnd");
}


// Called when the game starts
void UClimbSystem::BeginPlay()
{
	Super::BeginPlay();
	// Add Timeline Curve
	ClimbAlignToSurfaceTimeline->AddInterpFloat(ClimbAlignToSurfaceCurve, ClimbAlignToSurfaceTimelineDelegate);
	ClimbAlignToSurfaceTimeline->SetTimelineLength(TIMELINE_LENGTH);

}


// Called every frame
void UClimbSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

