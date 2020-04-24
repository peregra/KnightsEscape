// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_Normal		UMETA(DisplayName = "Normal"),
	EMS_Sprint		UMETA(DisplayName = "Sprinting"),
	EMS_Dead		UMETA(DisplayName = "Dead"),

	EMS_MAX			UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaState : uint8
{
	ESS_Normal					UMETA(DisplayName = "Normal"), 
	ESS_BelowMin				UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted				UMETA(DisplayName = "Exhausted"),
	ESS_RecoveringExhausted		UMETA(DisplayName = "Recovering"),

	ESS_Default					UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class KNIGHTSESCAPE_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	TArray<FVector> PickupLocations;

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementState MovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaState StaminaState;

	FORCEINLINE void SetStaminaState(EStaminaState State) { StaminaState = State; };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	float InterpolationSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	/** Set movement state and running speed */
	void SetMovementState(EMovementState State);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintSpeed;

	bool bVKeyDown;

	/** Press down to start sprinting */
	void VKeyDown();

	/** Release to stop sprinting */
	void VKeyUp();

	/** Camera boom | Positioning camera behind player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera | Stays behind user */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Scale turning functions for camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/**
	/* PLAYER STATS
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 Coins;

	void DecrementHealth(float DamageAmount);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float HealAmount);
	void Die();

	void DecrementStamina(float DamageAmount);
	void IncrementStamina(float GainAmount);

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 CoinAmount);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for forward/backward input */
	void MoveForward(float MoveRate);

	bool CanMove(float MoveRate);

	/** Called for left/right input */
	void MoveRight(float MoveRate);

	bool bMovingForward;
	bool bMovingRight;

	/** Called for yaw rotation input */
	void Turn(float MoveRate);

	/** Called for pitch rotation */
	void LookUp(float MoveRate);


	/** Called via input to turn at given rate 
	*   @param Rate: Normalized rate (1.0 means 100%)
	*/
	void TurnAtRate(float TurnRate);

	/** Called via input to look up/down at given rate
	*   @param Rate: Normalized rate (1.0 means 100%)
	*/
	void LookUpAtRate(float LookRate);

	bool bEKeyDown;
	void EKeyDown();
	void EKeyUp();

	bool bAttackPrimaryButtonDown;
	void AttackPrimaryButtonDown();
	void AttackPrimaryButtonUp();

	bool bAttackSecondaryButtonDown;
	void AttackSecondaryButtonDown();
	void AttackSecondaryButtonUp();

	bool bQuitKeyDown;
	void QuitKeyDown();
	void QuitKeyUp();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; };
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	void SetEquippedWeapon(AWeapon* WeaponToSet);
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* OverlappingItem) { ActiveOverlappingItem = OverlappingItem; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	bool Alive();

	virtual void Jump() override;

	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool bSetPosition);

	UFUNCTION(BlueprintCallable)
	void LoadGameNoSwitch();
};
