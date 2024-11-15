// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AssassinateInterface.h"
#include "AIBehaviorInterface.h"
#include "MyGameAI.generated.h"

class USphereComponent;
class UWidgetComponent;
class UAIAttackSystem;

UCLASS()
class MYGAME_API AMyGameAI : public ACharacter , public IAssassinate, public IAIBehavior
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category = "Assassinate")
	TObjectPtr<USphereComponent> AssassinateRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assassinate") // set it in blueprint
	TSubclassOf<UUserWidget> AssassinateWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assassinate")
	TObjectPtr<UWidgetComponent> AssassinateWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigate") // set it in blueprint
	TSubclassOf<UUserWidget> InvestigateWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigate")
	TObjectPtr<UWidgetComponent> InvestigateWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged") // set it in blueprint
	TArray<TObjectPtr<UAnimMontage>> HitAnims;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged") // set it in blueprint
	TObjectPtr<UAnimMontage> RagDollAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI") // set it in blueprint
	TObjectPtr<UAnimMontage> AttackAnim;

	UPROPERTY(VisibleAnywhere, Category = "AI Component");
	TObjectPtr<UAIAttackSystem> AIAttackSystem;

public:
	// Sets default values for this character's properties
	AMyGameAI();

	UFUNCTION()
	void Die();

	UFUNCTION()
	void RagDoll();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void SetInvestigateWidgetVisible();

	UFUNCTION(BlueprintCallable)
	void SetInvestigateWidgetHidden();
private:
	FTimerHandle TimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Override TakeDamage to handle the damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// Interface function
	virtual void StealthBackAssassinate(FVector& RefLocation, FRotator& AssassinateRotation) override;

	// Interface function
	virtual float Attack_Implementation() override;
};
