#include "AttackSystem.h"
#include "InputActionValue.h"
#include "MotionWarpingComponent.h"
#include "MyGameCharacter.h"
#include "MyGameAI.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"

#ifndef ATTACKSYSTEM_MACRO
#define DETECT_ENEMY_RADIUS 200.0f

#define H2H_TRACE_SPHERE_SIZE 5
#define H2H_TRACE_INTERVAL 0.01f
#define H2H_TRACE_DEBUG_DURATION 0.2f

#define SWORD_TRACE_SPHERE_SIZE 12
#define FORWARD_TRACE_SPHERE_SIZE 25
#define SWORD_TRACE_INTERVAL 0.01f
#define SWORD_TRACE_DEBUG_DURATION 0.2f
#define TIME_BETWEEN_DAMAGE 0.5f
#define BLOOD_VFX_SIZE 0.4f

#endif // ATTACKSYSTEM_MACRO


UAttackSystem::UAttackSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttackSystem::Init()
{
	MyGameCharacter = Cast<AMyGameCharacter>(GetOwner());
	/*SwordTopPointArrow = Cast<UArrowComponent>(MyGameCharacter->GetDefaultSubobjectByName(TEXT("Sword Top Point")));
	SwordBottomPointArrow = Cast<UArrowComponent>(MyGameCharacter->GetDefaultSubobjectByName(TEXT("Sword Bottom Point")));
	ForwardArrow = Cast<UArrowComponent>(MyGameCharacter->GetDefaultSubobjectByName(TEXT("Forward Arrow")));*/
	SetUpWarpingTargetMap();
}

void UAttackSystem::Attack()
{
	if (bCanAttack)
	{
		switch (AttackMode)
		{
		case EAttackMode::Melee:
			MeleeAttack();
			break;
		case EAttackMode::Ranged:
			break;
		case EAttackMode::Assissinate:
			Assassinate();
			break;
		case EAttackMode::Thrown:
			Thrown();
			break;
		default:
			break;
		}
	}
}

void UAttackSystem::MeleeAttack()
{

	switch (MeleeMode)
	{
	case EMeleeMode::H2H:
		H2HAttack();
		break;
	case EMeleeMode::Axe:
		break;
	case EMeleeMode::Staff:
		break;
	case EMeleeMode::Sword:
		break;
	default:
		break;
	}
}

void UAttackSystem::RangedAttack()
{
}

void UAttackSystem::Assassinate()
{
}

void UAttackSystem::Thrown()
{
}

void UAttackSystem::H2HAttack()
{
	DetectEnemyNearby();
	if (bIsAttacking)
	{
		bDoCombo = true;
	}
	else
	{
		H2HCombo();
	}
}

void UAttackSystem::ShuffleH2HComboList()
{
	MyShuffleArray(PunchIndexList);
	MyShuffleArray(KickIndexList);
}

void UAttackSystem::H2HContinue()
{
	if (bDoCombo) // Combo continue
	{
		bDoCombo = false;
		H2HCombo();
	}
	else  // Combo stop
	{
		H2HKickComboStop();
	}
}

void UAttackSystem::H2HCombo()
{
	if (CurrentComboStage != TargetComboStage) // Prevent repeating same animation
	{
		if (MyGameCharacter.IsValid())
		{
			AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
			TObjectPtr<UAnimInstance> AnimInstance = MyGameCharacterPtr->GetMesh()->GetAnimInstance();
			if (IsValid(AnimInstance))
			{
				switch (TargetComboStage)
				{
				case EComboStage::Stage1:
					if (bIsAttacking) // Don't enter stage 1 before the combo ends. This create the time gaps between combo sets.
					{
						return;
					}
					bIsAttacking = true;
					SetWarpingTarget(PunchMontages[PunchIndexList[0]]);
					AnimInstance->Montage_Play(PunchMontages[PunchIndexList[0]]);
					//SetWarpingTarget(KickMontages[KickIndexList[2]]);
					//AnimInstance->Montage_Play(KickMontages[KickIndexList[2]]);

					CurrentComboStage = TargetComboStage;
					TargetComboStage = EComboStage::Stage2;
					break;
				case EComboStage::Stage2:
					SetWarpingTarget(PunchMontages[2]);
					AnimInstance->Montage_Play(PunchMontages[2]);
					/*SetWarpingTarget(KickMontages[KickIndexList[3]]);
					AnimInstance->Montage_Play(KickMontages[KickIndexList[3]]);*/

					CurrentComboStage = TargetComboStage;
					TargetComboStage = EComboStage::Stage3;
					break;
				case EComboStage::Stage3:
					SetWarpingTarget(KickMontages[KickIndexList[0]]);
					AnimInstance->Montage_Play(KickMontages[KickIndexList[0]]);
					CurrentComboStage = TargetComboStage;
					TargetComboStage = EComboStage::Stage4;
					break;
				case EComboStage::Stage4:
					SetWarpingTarget(KickMontages[KickIndexList[1]]);
					AnimInstance->Montage_Play(KickMontages[KickIndexList[1]]);
					CurrentComboStage = TargetComboStage;
					TargetComboStage = EComboStage::Stage1;
					break;
				default:
					TargetComboStage = EComboStage::Stage1;
					break;
				}
			}
		}
	}
}

void UAttackSystem::H2HKickComboStop()
{
	bIsAttacking = false;
	TargetComboStage = EComboStage::Stage1;
	CurrentComboStage = EComboStage::None;
	ShuffleH2HComboList();
}

void UAttackSystem::StartH2HTrace(FName SocketName)
{
	GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, [this, SocketName]()
		{	if (MyGameCharacter.IsValid())
			{
				AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
				FHitResult HitResult;
				FVector Start = MyGameCharacterPtr->GetMesh()->GetSocketLocation(SocketName);
				FVector End = Start;
				TArray<AActor*> IgnoredActors;
				IgnoredActors.Add(MyGameCharacterPtr);
				if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, H2H_TRACE_SPHERE_SIZE, ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::Type::ForDuration, HitResult, true, FColor::Green, FColor::Red, H2H_TRACE_DEBUG_DURATION))
				{
					if (HitResult.GetActor()->ActorHasTag(TEXT("Enemy")))
					{
						
						AActor* Enemy = HitResult.GetActor();
						if (IsValid(Enemy))
						{
							// Apply damage to enemy
							UGameplayStatics::ApplyDamage(Enemy, 10, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);

							// Play camera shake
							UGameplayStatics::PlayWorldCameraShake(GetWorld(), SwordHitCameraShakeClass, HitResult.Location, 0, 2000);

							// Play blood vfx
							FRotator BloodNormal(FMath::FRandRange(0.0f, 360.0f), FMath::FRandRange(0.0f, 360.0f), FMath::FRandRange(0.0f, 360.0f));
							FTransform SpawnTransform(BloodNormal, HitResult.Location, FVector(BLOOD_VFX_SIZE));
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodVFX, SpawnTransform, true, EPSCPoolMethod::AutoRelease, true);

							StopH2HTrace();
						}
					}
				}
			}
		}, H2H_TRACE_INTERVAL, true);
}

void UAttackSystem::StopH2HTrace()
{
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
}



//void UAttackSystem::ForwardTrace()
//{
//	FHitResult HitResult;
//	FVector Start = ForwardArrow->GetComponentLocation();
//	FVector End = ForwardArrow->GetComponentLocation();
//	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, FORWARD_TRACE_SPHERE_SIZE, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, HitResult, true, FColor::Blue, FColor::Red, SWORD_TRACE_DEBUG_DURATION))
//	{
//		if (HitResult.GetActor()->ActorHasTag(TEXT("Enemy")))
//		{
//			ACharacter* Enemy = Cast<ACharacter>(HitResult.GetActor());
//			if (IsValid(Enemy))
//			{
//				UGameplayStatics::ApplyDamage(Enemy, 10, MyGameCharacter->GetInstigatorController(), MyGameCharacter, nullptr);
//
//				// Play camera shake
//				UGameplayStatics::PlayWorldCameraShake(GetWorld(), SwordHitCameraShakeClass, HitResult.Location, 0, 2000);
//
//				// Play blood vfx
//				FRotator BloodNormal(FMath::FRandRange(0.0f, 360.0f), FMath::FRandRange(0.0f, 360.0f), FMath::FRandRange(0.0f, 360.0f));
//				FTransform SpawnTransform(BloodNormal, HitResult.Location, FVector(BLOOD_VFX_SIZE));
//				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodVFX, SpawnTransform, true, EPSCPoolMethod::AutoRelease, true);
//			}
//		}
//	}
//}

void UAttackSystem::MyShuffleArray(TArray<int>& Array)
{
	if (Array.Num() > 0)
	{
		for (int i = 0; i < Array.Num(); ++i)
		{
			int Index = FMath::RandRange(i, Array.Num() - 1);
			if (i != Index)
			{
				Array.Swap(i, Index);
			}
		}
	}
}

void UAttackSystem::DetectEnemyNearby()
{
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		TargetEnemy = nullptr;
		TArray<FHitResult> HitResults;
		FVector Start = MyGameCharacterPtr->GetActorLocation();
		FVector End = Start;
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(MyGameCharacterPtr);
		if (UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, DETECT_ENEMY_RADIUS, ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::ForDuration, HitResults, true, FLinearColor::Blue, FLinearColor::Red, 3.0f))
		{
			for (FHitResult& HitResult : HitResults)
			{
				if (HitResult.GetActor()->ActorHasTag("Enemy"))
				{
					TargetEnemy = HitResult.GetActor();
					break;
				}
			}
		}
	}
}

void UAttackSystem::SetUpWarpingTargetMap()
{
	WarpingTargetMap.Empty();
	for (int i = 0; i < PunchMontages.Num(); i++)
	{
		FString String = "Punch " + FString::FromInt(i + 1) + " Warp";
		WarpingTargetMap.Add(PunchMontages[i], FName(*String));
	}
	for (int i = 0; i < KickMontages.Num(); i++)
	{
		FString String = "Kick " + FString::FromInt(i + 1) + " Warp";
		WarpingTargetMap.Add(KickMontages[i], FName(*String));
	}
}

void UAttackSystem::SetWarpingTarget(UAnimMontage* Montage)
{
	if (MyGameCharacter.IsValid())
	{
		AMyGameCharacter* MyGameCharacterPtr = MyGameCharacter.Get();
		FRotator LookAtRotation;
		if (IsValid(TargetEnemy))
		{
			LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MyGameCharacterPtr->GetActorLocation(), TargetEnemy->GetActorLocation());
		}
		else
		{
			LookAtRotation = MyGameCharacterPtr->GetActorRotation();
		}
		FName TargetName = WarpingTargetMap[Montage];
		FMotionWarpingTarget* Target = new FMotionWarpingTarget();
		Target->Name = TargetName;
		Target->Rotation = LookAtRotation;
		MyGameCharacterPtr->MotionWarping->AddOrUpdateWarpTarget(*Target);
	}
}

// Called when the game starts
void UAttackSystem::BeginPlay()
{
	Super::BeginPlay();

	Init();

	for (int i = 0; i < PunchMontages.Num() - 1; i++)
	{
		PunchIndexList.Add(i);
	}
	for (int i = 0; i < KickMontages.Num(); i++)
	{
		KickIndexList.Add(i);
	}
	ShuffleH2HComboList();
}


// Called every frame
void UAttackSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

