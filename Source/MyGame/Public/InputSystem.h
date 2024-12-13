#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputSystem.generated.h"

class AMyGameCharacter;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
struct FEnhancedInputActionValueBinding;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAME_API UInputSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	// MappingContext 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// Jump Input Action
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;*/

	//// Move Input Action 
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputAction> MoveAction;

	//// Look Input Action 
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputAction> LookAction;

	//// Crouch Input Action 
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputAction> CrouchAction;

	//// Vault Input Action 
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputAction> VaultAction;

	//// Assassinate Input Action
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputAction> AssassinateAction;

	///** Sprint Input Action */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputAction> SprintAction;

	//// Lock Input Action
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputAction> LockAction;

	//// Menu Input Action 
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputAction> MenuAction;

	//// Climb Input Action 
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputAction> ClimbAction;

	//// Cover Input Action 
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputAction> CoverAction;

	//// Aim Input Action 
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputAction> AimAction;

	//// Fire Input Action 
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputAction> FireAction;



	// Controller r button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> RStickAction;

	// Controller l button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LStickAction;

	// Controller square button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SquareButtonAction;

	// Controller circle button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CircleButtonAction;

	// Controller triangle button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TriangleButtonAction;

	// Controller cross button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CrossButtonAction;

	// Controller up button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> UpButtonAction;

	// Controller down button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> DownButtonAction;

	// Controller left button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LeftButtonAction;

	// Controller right button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> RightButtonAction;

	// Controller l1 button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> L1ButtonAction;

	// Controller l2 button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> L2ButtonAction;

	// Controller l3 button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> L3ButtonAction;

	// Controller r1 button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> R1ButtonAction;

	// Controller r2 button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> R2ButtonAction;

	// Controller r3 button input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> R3ButtonAction;

private:
	// Access my game character
	TWeakObjectPtr<AMyGameCharacter> MyGameCharacter;

	// We can get action value from this action binding
	FEnhancedInputActionValueBinding* LStickActionBinding;

public:
	UInputSystem();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetupInputBinding(UInputComponent* PlayerInputComponent);

	/**  Retrun L stick input value */
	FInputActionValue GetLStickActionValue();

protected:
	virtual void BeginPlay() override;
	
private:
	/**  Init function call in BeginPlay */
	void Init();

	/** Called for L stick input (Move) */
	UFUNCTION()
	void LStickInput(const FInputActionValue& Value);

	/** Called for R stick input (Look) */
	UFUNCTION()
	void RStickInput(const FInputActionValue& Value);

	/** Called for up button input (Medkit/Flashlight) */
	UFUNCTION()
	void OnUpButtonPressed();

	/** Called for up button input (Dagger/Thrown) */
	UFUNCTION()
	void OnDownButtonPressed();

	/** Called for left button input (Rifle/Shotgun) */
	UFUNCTION()
	void OnLeftButtonPressed();

	/** Called for right button input (Pistol/Melee) */
	UFUNCTION()
	void OnRightButtonPressed();

	/** Called for square button pressed input (Melee) */
	UFUNCTION()
	void OnSquareButtonPressed();

	/** Called for triangle button pressed input (Interact) */
	UFUNCTION()
	void OnTriangleButtonPressed();

	/** Called for circle button pressed input (Crouch) */
	UFUNCTION()
	void OnCircleButtonPressed();

	/** Called for cross button pressed input (Jump) */
	UFUNCTION()
	void OnCrossButtonPressed();

	/** Called for L1 pressed input (Start sprinting) */
	UFUNCTION()
	void OnL1ButtonPressed();

	/** Called for L1 released input (Start sprinting) */
	UFUNCTION()
	void OnL1ButtonReleased();

	/** Called for L2 button pressed input (Start aiming) */
	UFUNCTION()
	void OnL2ButtonPressed();

	/** Called for L2 button released input (End aiming) */
	UFUNCTION()
	void OnL2ButtonReleased();


	void OnL3ButtonPressed();
	void OnR1ButtonPressed();

	/** Called for R1 button released input (Equipment Swap) */
	UFUNCTION()
	void OnR1ButtonReleased();

	/** Called for R2 button pressed input (Reload/Start firing) */
	UFUNCTION()
	void OnR2ButtonPressed();

	/** Called for R2 button released input (End firing) */
	UFUNCTION()
	void OnR2ButtonReleased();

	void OnR3ButtonPressed();
};
