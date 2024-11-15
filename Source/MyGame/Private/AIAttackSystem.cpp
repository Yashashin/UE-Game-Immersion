#include "AIAttackSystem.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameAI.h"
#include "MyGameCharacter.h"

#ifndef AIATTACKSYSTEM_MACRO
#define SWORD_TRACE_SPHERE_SIZE 12
#define FORWARD_TRACE_SPHERE_SIZE 25
#define SWORD_TRACE_INTERVAL 0.01f
#define SWORD_TRACE_DEBUG_DURATION 0.2f
#define TIME_BETWEEN_DAMAGE 0.5f
#define BLOOD_VFX_SIZE 0.4f

#endif // AIATTACKSYSTEM_MACRO


UAIAttackSystem::UAIAttackSystem()
{
	PrimaryComponentTick.bCanEverTick = true;

	bCanAttack = true;
	bIsAttacking = false;
	bDoCombo = false;
	bHasApplyDamage = false;
	AttackMode = EAIAttackMode::SWORD;
	AttackStage = EAIAttackStage::STAGE1;
	CurrentPlayingStage = EAIAttackStage::NONE;
}

void UAIAttackSystem::Init()
{
	MyGameAI = Cast<AMyGameAI>(GetOwner());
}

void UAIAttackSystem::Attack()
{
	SwordAttack();
}

void UAIAttackSystem::SwordAttack()
{
	if (bIsAttacking)
	{
		bDoCombo = true;
	}
	else
	{
		bIsAttacking = true;
		SwordAttackCombo();
	}
}

void UAIAttackSystem::SwordAttackContinue()
{
	if (bDoCombo)
	{
		bDoCombo = false;
		SwordAttackCombo();
	}
	else // Stop combo
	{
		StopSwordAttack();
	}
}

void UAIAttackSystem::StopSwordAttack()
{
	bIsAttacking = false;
	AttackStage = EAIAttackStage::STAGE1;
	CurrentPlayingStage = EAIAttackStage::NONE;
}

void UAIAttackSystem::SwordAttackCombo()
{
	if (CurrentPlayingStage != AttackStage) // Prevent repeating same animation
	{
		TObjectPtr<UAnimInstance> AnimInstance = MyGameAI->GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance))
		{
			switch (AttackStage)
			{
			case EAIAttackStage::STAGE1:
				AnimInstance->Montage_Play(SwordAttackMontage1);
				CurrentPlayingStage = AttackStage;
				AttackStage = EAIAttackStage::STAGE2;
				break;
			case EAIAttackStage::STAGE2:
				AnimInstance->Montage_Play(SwordAttackMontage2);
				CurrentPlayingStage = AttackStage;
				AttackStage = EAIAttackStage::STAGE3;
				break;
			case EAIAttackStage::STAGE3:
				AnimInstance->Montage_Play(SwordAttackMontage3);
				CurrentPlayingStage = AttackStage;
				AttackStage = EAIAttackStage::STAGE1;
				break;
			default:
				AttackStage = EAIAttackStage::STAGE1;
				break;
			}
		}
	}
}

void UAIAttackSystem::StartSwordTrace()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			FHitResult HitResult;
			FVector Start = SwordTopPointArrow->GetComponentLocation();
			FVector End = SwordBottomPointArrow->GetComponentLocation();
			if (!bHasApplyDamage && UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, SWORD_TRACE_SPHERE_SIZE, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::Type::ForDuration, HitResult, true, FColor::Blue, FColor::Red, SWORD_TRACE_DEBUG_DURATION))
			{
				if (HitResult.GetActor()->ActorHasTag(TEXT("Character")))
				{
					TObjectPtr<AMyGameCharacter> Character = Cast<AMyGameCharacter>(HitResult.GetActor());
					if (IsValid(Character))
					{
						UGameplayStatics::ApplyDamage(Character, 10, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);
						bHasApplyDamage = true;

						// Play camera shake
						UGameplayStatics::PlayWorldCameraShake(GetWorld(), SwordHitCameraShakeClass, HitResult.Location, 0, 2000);

						// Play blood vfx
						FRotator BloodNormal(FMath::FRandRange(0.0f, 360.0f), FMath::FRandRange(0.0f, 360.0f), FMath::FRandRange(0.0f, 360.0f));
						FTransform SpawnTransform(BloodNormal, HitResult.Location, FVector(BLOOD_VFX_SIZE));
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodVFX, SpawnTransform, true, EPSCPoolMethod::AutoRelease, true);


						// Do once reset
						GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, [this]()
							{
								bHasApplyDamage = false;
							}, TIME_BETWEEN_DAMAGE, false);
					}
				}
			}
		}, SWORD_TRACE_INTERVAL, true);
}

void UAIAttackSystem::StopSwordTrace()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}




// Called when the game starts
void UAIAttackSystem::BeginPlay()
{
	Super::BeginPlay();

	SwordTopPointArrow = Cast<UArrowComponent>(MyGameAI->GetDefaultSubobjectByName(TEXT("Sword Top Point")));
	SwordBottomPointArrow = Cast<UArrowComponent>(MyGameAI->GetDefaultSubobjectByName(TEXT("Sword Bottom Point")));
	ForwardArrow = Cast<UArrowComponent>(MyGameAI->GetDefaultSubobjectByName(TEXT("Forward Arrow")));
	
}


// Called every frame
void UAIAttackSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

