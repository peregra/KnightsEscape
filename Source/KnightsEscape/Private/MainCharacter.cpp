// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "SaveGameProgress.h"
#include "ItemStorage.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom | Pulls toward player in case of collision
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 500.f; // Camera follow distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm with controller

	// It is okay to hard code the main character's collision capsule size since it will not change
	GetCapsuleComponent()->SetCapsuleSize(34, 88);

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach camera to end of boom; Boom matches controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	// Set turn rates
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// Character can rotate without camera rotating
	// Set to true if camera should rotate with character
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;	// Just in case
	bUseControllerRotationRoll = false;		// Just in case

	// Character faces the direction he moves
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.f, 800.f, 0.f); // Character only faces yaw movement direction
	GetCharacterMovement()->JumpZVelocity = 450.f; 
	GetCharacterMovement()->AirControl = 0.3f; // Some control is available in the air

	// Default starts
	MaxHealth = 100.f;
	Health = 100.f;
	MaxStamina = 300.f;
	Stamina = 300.f;
	Coins = 0;

	RunSpeed = 500.f;
	SprintSpeed = 700.f;
	bVKeyDown = false;
	bEKeyDown = false;
	bQuitKeyDown = false;

	bMovingForward = false;
	bMovingRight = false;

	// Initialize Enums
	MovementState = EMovementState::EMS_Normal;
	StaminaState = EStaminaState::ESS_Normal;

	StaminaDrainRate = 20.f;
	MinSprintStamina = 25.f;


	// Interpolation Initializations
	InterpolationSpeed = 10.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	MovementState = EMovementState::EMS_Normal;
	StaminaState = EStaminaState::ESS_Normal;

	MainPlayerController = Cast<AMainPlayerController>(GetController());
	
	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (Map != "Dungeon")
	{
		LoadGameNoSwitch();
		if (MainPlayerController)
		{
			MainPlayerController->GameModeOnly();
		}
	}

}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Alive())
	{
		return;
	}

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaState)
	{
	case EStaminaState::ESS_Normal:
		if (bVKeyDown)
		{
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaState(EStaminaState::ESS_BelowMin);
				Stamina -= DeltaStamina;
			}
			else
			{
				Stamina -= DeltaStamina;
			}
			if (bMovingForward || bMovingRight)
			{
				SetMovementState(EMovementState::EMS_Sprint);
			}
			else
			{
				SetMovementState(EMovementState::EMS_Normal);
			}
		}
		else
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementState(EMovementState::EMS_Normal);
		}
		break;
	case EStaminaState::ESS_BelowMin:
		if (bVKeyDown)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				SetStaminaState(EStaminaState::ESS_Exhausted);
				Stamina = 0;
				SetMovementState(EMovementState::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				if (bMovingForward || bMovingRight)
				{
					SetMovementState(EMovementState::EMS_Sprint);
				}
				else
				{
					SetMovementState(EMovementState::EMS_Normal);
				}
			}
		}
		else
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaState(EStaminaState::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementState(EMovementState::EMS_Normal);
		}
		break;
	case EStaminaState::ESS_Exhausted:
		if (bVKeyDown)
		{
			Stamina = 0.f;
		}
		else
		{
			SetStaminaState(EStaminaState::ESS_RecoveringExhausted);
			Stamina += DeltaStamina;
		}
		SetMovementState(EMovementState::EMS_Normal);
		break;
	case EStaminaState::ESS_RecoveringExhausted:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaState(EStaminaState::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementState(EMovementState::EMS_Normal);
		break;
	default:
		;
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpolationSpeed);

		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}


FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}


// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Bind actions
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::VKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::VKeyUp);

	PlayerInputComponent->BindAction("Quit", IE_Pressed, this, &AMainCharacter::QuitKeyDown);
	PlayerInputComponent->BindAction("Quit", IE_Released, this, &AMainCharacter::QuitKeyUp);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::EKeyDown);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMainCharacter::EKeyUp);

	PlayerInputComponent->BindAction("AttackPrimary", IE_Pressed, this, &AMainCharacter::AttackPrimaryButtonDown);
	PlayerInputComponent->BindAction("AttackPrimary", IE_Released, this, &AMainCharacter::AttackPrimaryButtonUp);

	PlayerInputComponent->BindAction("AttackSecondary", IE_Pressed, this, &AMainCharacter::AttackSecondaryButtonDown);
	PlayerInputComponent->BindAction("AttackSecondary", IE_Released, this, &AMainCharacter::AttackSecondaryButtonUp);

	// Bind axes movements
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);
}


void AMainCharacter::MoveForward(float MoveRate)
{
	bMovingForward = false;
	if (CanMove(MoveRate))
	{
		// Get forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// Move character
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, MoveRate);

		bMovingForward = true;
	}
}

bool AMainCharacter::CanMove(float MoveRate)
{
	if (MainPlayerController)
	{
		return MoveRate != 0.f &&
			(!bAttacking) && Alive() &&
			!MainPlayerController->bPauseMenuVisible;
	}
	
	return false;
}


void AMainCharacter::MoveRight(float MoveRate)
{
	bMovingRight = false;
	if (CanMove(MoveRate))
	{
		// Get forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// Move character
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, MoveRate);

		bMovingRight = true;
	}
}


void AMainCharacter::Turn(float MoveRate)
{
	AddControllerYawInput(MoveRate);
}


void AMainCharacter::LookUp(float MoveRate)
{
	AddControllerPitchInput(MoveRate);
}


void AMainCharacter::TurnAtRate(float TurnRate)
{
	AddControllerYawInput(TurnRate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void AMainCharacter::LookUpAtRate(float TurnRate)
{
	AddControllerPitchInput(TurnRate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void AMainCharacter::EKeyDown()
{
	bEKeyDown = true;

	if (!Alive())
	{
		return;
	}

	if (MainPlayerController) 
	{
		if (MainPlayerController->bPauseMenuVisible)
		{
			return;
		}

		if (ActiveOverlappingItem)
		{
			AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
			if (Weapon)
			{
				Weapon->Equip(this);
				SetActiveOverlappingItem(nullptr);
			}
		}
	}
}


void AMainCharacter::EKeyUp()
{
	bEKeyDown = false;
}


void AMainCharacter::AttackPrimaryButtonDown()
{
	bAttackPrimaryButtonDown = true;

	if (EquippedWeapon)
	{
		Attack();
	}
}


void AMainCharacter::AttackPrimaryButtonUp()
{
	bAttackPrimaryButtonDown = false;

}


void AMainCharacter::AttackSecondaryButtonDown()
{
	bAttackSecondaryButtonDown = true;

	if (EquippedWeapon)
	{
		Attack();
	}
}

void AMainCharacter::AttackSecondaryButtonUp()
{
	bAttackSecondaryButtonDown = false;
}


void AMainCharacter::QuitKeyDown()
{
	bQuitKeyDown = true;

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}


void AMainCharacter::QuitKeyUp()
{
	bQuitKeyDown = false;
}


void AMainCharacter::DecrementHealth(float DamageAmount)
{

}


float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die();
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}


void AMainCharacter::IncrementHealth(float HealAmount)
{
	if (Health + HealAmount <= MaxHealth)
	{
		Health += HealAmount;
	}
	else
	{
		Health = MaxHealth;
	}
}

void AMainCharacter::Die()
{
	if (!Alive())
	{
		return;
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetMovementState(EMovementState::EMS_Dead);
}


void AMainCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}


bool AMainCharacter::Alive()
{
	return (MovementState != EMovementState::EMS_Dead);
}


void AMainCharacter::DecrementStamina(float DamageAmount)
{
	if (Stamina - DamageAmount <= 0.f)
	{
		Stamina = 0.f;
	}
}


void AMainCharacter::IncrementStamina(float GainAmount)
{
	if (Stamina + GainAmount <= MaxHealth)
	{
		Stamina += GainAmount;
	}
	else
	{
		Stamina = MaxStamina;
	}
}


void AMainCharacter::IncrementCoins(int32 CoinAmount)
{
	Coins += CoinAmount;
}


void AMainCharacter::SetMovementState(EMovementState State)
{
	MovementState = State;
	if (MovementState == EMovementState::EMS_Sprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}


void AMainCharacter::VKeyDown()
{
	bVKeyDown = true;
}


void AMainCharacter::VKeyUp()
{
	bVKeyDown = false;
}


void AMainCharacter::ShowPickupLocations()
{
	for (FVector Location : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 30.f, 24, FLinearColor::Green, 5.f, 0.5f);
	}
	
}


void AMainCharacter::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = WeaponToSet;
}


void AMainCharacter::Attack()
{
	if (!bAttacking && Alive())
	{
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			if (bAttackPrimaryButtonDown)
			{
				int32 Selection = FMath::RandRange(0, 1);
				switch (Selection)
				{
				case 0:
					AnimInstance->Montage_Play(CombatMontage, 1.6f);
					AnimInstance->Montage_JumpToSection(FName("Attack_Primary"), CombatMontage);
					break;
				case 1:
					AnimInstance->Montage_Play(CombatMontage, 1.6f);
					AnimInstance->Montage_JumpToSection(FName("Attack_Primary_Alt"), CombatMontage);
					break;
				default:
					;
				}
			}
			else if (bAttackSecondaryButtonDown)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.6f);
				AnimInstance->Montage_JumpToSection(FName("Attack_Secondary"), CombatMontage);
			}
		}
	}
}


void AMainCharacter::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bAttackPrimaryButtonDown || bAttackSecondaryButtonDown)
	{
		Attack();
	}
}


void AMainCharacter::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}


void AMainCharacter::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}


void AMainCharacter::Jump()
{
	if (MainPlayerController->bPauseMenuVisible)
	{
		return;
	}

	if (Alive())
	{
		ACharacter::Jump();
	}
}


void AMainCharacter::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation();
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}

		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}


void AMainCharacter::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();
		FName CurrentLevelName(*CurrentLevel);

		if (LevelName != CurrentLevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}


void AMainCharacter::SaveGame()
{
	USaveGameProgress* SaveGameInstance = Cast<USaveGameProgress>(UGameplayStatics::CreateSaveGameObject(USaveGameProgress::StaticClass()));

	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Coins = Coins;

	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	SaveGameInstance->CharacterStats.LevelName = MapName;

	if (EquippedWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
	}


	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveName, SaveGameInstance->UserIndex);
}


void AMainCharacter::LoadGame(bool bSetPosition)
{
	USaveGameProgress* LoadGameInstance = Cast<USaveGameProgress>(UGameplayStatics::CreateSaveGameObject(USaveGameProgress::StaticClass()));

	LoadGameInstance = Cast<USaveGameProgress>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;

	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

			if (WeaponName != TEXT(""))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);

				WeaponToEquip->Equip(this);
			}
		}
	}
	
	if (bSetPosition)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}

	SetMovementState(EMovementState::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;

	if (LoadGameInstance->CharacterStats.LevelName != TEXT(""))
	{
		FName LevelName(*LoadGameInstance->CharacterStats.LevelName);
		SwitchLevel(LevelName);
	}
}

void AMainCharacter::LoadGameNoSwitch()
{
	USaveGameProgress* LoadGameInstance = Cast<USaveGameProgress>(UGameplayStatics::CreateSaveGameObject(USaveGameProgress::StaticClass()));

	LoadGameInstance = Cast<USaveGameProgress>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveName, LoadGameInstance->UserIndex));

	if (!LoadGameInstance)
	{
		return;
	}
	 
	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;
	
	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;
			if (WeaponName != TEXT(""))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);

				WeaponToEquip->Equip(this);
			}
		}
	}

	SetMovementState(EMovementState::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;
}


