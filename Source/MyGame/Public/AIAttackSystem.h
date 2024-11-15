// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIAttackSystem.generated.h"

UENUM()
enum class EAIAttackMode :uint8
{
	SWORD,
	BOW,
	GUN,
	FIST,
};

UENUM()
enum class EAIAttackStage :uint8
{
	STAGE1,
	STAGE2,
	STAGE3,
	NONE,
};

class AMyGameAI;
class UArrowComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYGAME_API UAIAttackSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "AI Attack")
	bool bCanAttack;

	UPROPERTY(VisibleAnywhere, Category = "AI Attack")
	bool bIsAttacking;

	UPROPERTY(VisibleAnywhere, Category = "AI Attack")
	bool bDoCombo;

	UPROPERTY(VisibleAnywhere, Category = "AI Attack")
	TObjectPtr<UArrowComponent> SwordTopPointArrow;

	UPROPERTY(VisibleAnywhere, Category = "AI Attack")
	TObjectPtr<UArrowComponent> SwordBottomPointArrow;

	UPROPERTY(VisibleAnywhere, Category = "AI Attack")
	TObjectPtr<UArrowComponent> ForwardArrow;

	UPROPERTY(VisibleAnywhere, Category = "AI Attack")
	EAIAttackMode AttackMode;

	UPROPERTY(VisibleAnywhere, Category = "AI Attack")
	EAIAttackStage CurrentPlayingStage;

	UPROPERTY(VisibleAnywhere, Category = "AI Attack")
	EAIAttackStage AttackStage;

	UPROPERTY(EditAnywhere, Category = "AI Attack") // Set it in blueprint
	TObjectPtr<UAnimMontage> SwordAttackMontage1;

	UPROPERTY(EditAnywhere, Category = "AI Attack") // Set it in blueprint
	TObjectPtr<UAnimMontage> SwordAttackMontage2;

	UPROPERTY(EditAnywhere, Category = "AI Attack") // Set it in blueprint
	TObjectPtr<UAnimMontage> SwordAttackMontage3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attack") // Set it in blueprint
	TSubclassOf<UCameraShakeBase> SwordHitCameraShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attack") // Set it in blueprint
	TObjectPtr<UParticleSystem> BloodVFX;

private:
	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	FTimerHandle DelayTimerHandle;

	bool bHasApplyDamage;

	TObjectPtr<AMyGameAI> MyGameAI;
public:
	UAIAttackSystem();

	UFUNCTION()
	void Init();

	UFUNCTION()
	void Attack();

	UFUNCTION()
	void SwordAttack();

	UFUNCTION()
	void SwordAttackContinue();

	UFUNCTION()
	void StopSwordAttack();

	UFUNCTION()
	void SwordAttackCombo();

	UFUNCTION()
	void StartSwordTrace();

	UFUNCTION()
	void StopSwordTrace();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
