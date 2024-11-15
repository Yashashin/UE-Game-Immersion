// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackSystem.generated.h"

UENUM(BlueprintType)
enum class EAttackMode :uint8
{
	Melee UMETA(DisplayName = "Melee"),
	Ranged UMETA(DisplayName = "Ranged"),
	Assissinate UMETA(DisplayName = "Assissinate"),
	Thrown UMETA(DisplayName = "Thrown"),
};

UENUM(BlueprintType)
enum class EMeleeMode :uint8
{
	H2H UMETA(DisplayName = "H2H"),
	Axe UMETA(DisplayName = "Axe"),
	Staff UMETA(DisplayName = "Staff"),
	Sword UMETA(DisplayName = "Sword"),
};

UENUM(BlueprintType)
enum class EComboStage :uint8
{
	Stage1 UMETA(DisplayName = "Stage 1"),
	Stage2 UMETA(DisplayName = "Stage 2"),
	Stage3 UMETA(DisplayName = "Stage 3"),
	Stage4 UMETA(DisplayName = "Stage 4"),
	None UMETA(DisplayName = "None"),
};

class AMyGameCharacter;
class UInputAction;
class UArrowComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYGAME_API UAttackSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	/*UPROPERTY(VisibleAnywhere, Category = "Attack")
	TObjectPtr<UArrowComponent> SwordTopPointArrow;

	UPROPERTY(VisibleAnywhere, Category = "Attack")
	TObjectPtr<UArrowComponent> SwordBottomPointArrow;

	UPROPERTY(VisibleAnywhere, Category = "Attack")
	TObjectPtr<UArrowComponent> ForwardArrow;*/

	// Current attack mode
	UPROPERTY(VisibleAnywhere, Category = "Attack")
	EAttackMode AttackMode = EAttackMode::Melee;

	// Current melee mode
	UPROPERTY(VisibleAnywhere, Category = "Attack")
	EMeleeMode MeleeMode = EMeleeMode::H2H;

	// Sword attack montages
	//UPROPERTY(EditAnywhere, Category = "Attack") // Set in blueprint
	//	TObjectPtr<UAnimMontage> SwordAttackMontage1;
	//UPROPERTY(EditAnywhere, Category = "Attack") // Set in blueprint
	//	TObjectPtr<UAnimMontage> SwordAttackMontage2;
	//UPROPERTY(EditAnywhere, Category = "Attack") // Set in blueprint
	//	TObjectPtr<UAnimMontage> SwordAttackMontage3;
	//UPROPERTY(EditAnywhere, Category = "Attack") // Set in blueprint
	//	TObjectPtr<UAnimMontage> SwordAttackMontage4;

	// H2H kick montages
	UPROPERTY(EditAnywhere, Category = "Attack") // Set in blueprint
		TArray<TObjectPtr<UAnimMontage>> KickMontages;

	// H2H punch montages
	UPROPERTY(EditAnywhere, Category = "Attack") // Set in blueprint
		TArray<TObjectPtr<UAnimMontage>> PunchMontages;

	// Camera shake when hit enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack") // Set in blueprint
		TSubclassOf<UCameraShakeBase> SwordHitCameraShakeClass;

	// Blood particle 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack") // Set in blueprint
		TObjectPtr<UParticleSystem> BloodVFX;

private:
	// Can attack now?
	bool bCanAttack = true;

	// Is attacking now?
	bool bIsAttacking = false;

	// Should do next combo?
	bool bDoCombo = false;

	// Combo stage being played currently
	EComboStage CurrentComboStage = EComboStage::None;

	// Combo stage want to play
	EComboStage TargetComboStage = EComboStage::Stage1;

	// Time handle for attack tracing
	FTimerHandle TraceTimerHandle;

	// Access my game character
	TWeakObjectPtr<AMyGameCharacter> MyGameCharacter = nullptr;

	// The closest enemy when attacking
	// We will warp the attack motion towards this target
	AActor* TargetEnemy = nullptr;

	// Shuffled punch montage list
	TArray<int32> PunchIndexList;

	// Shuffled kick montage list
	TArray<int32> KickIndexList;

	// Maps the montages to motion warping target names
	TMap<UAnimMontage*, FName> WarpingTargetMap;

public:
	UAttackSystem();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void Attack();

	UFUNCTION(BlueprintPure)
	bool GetIsAttacking() { return bIsAttacking; }

	UFUNCTION(BlueprintCallable)
	void H2HContinue();

	UFUNCTION(BlueprintCallable)
	void H2HKickComboStop();

	UFUNCTION(BlueprintCallable)
	void StartH2HTrace(FName SocketName);

	UFUNCTION(BlueprintCallable)
	void StopH2HTrace();

protected:
	virtual void BeginPlay() override;

private:
	// Init function called in BeginPlay
	void Init();

	// Shuufle H2H combo order
	void ShuffleH2HComboList();

	// Helper function
	void MyShuffleArray(TArray<int>& Array);

	// Detect enemy nearby so that we can warp the motion to the target
	void DetectEnemyNearby();

	// Set up warping target map
	void SetUpWarpingTargetMap();

	// Set the values of warping target 
	void SetWarpingTarget(UAnimMontage* Montage);



	void MeleeAttack();

	void RangedAttack();

	void Assassinate();

	void Thrown();

	void H2HAttack();

	void H2HCombo();


};
