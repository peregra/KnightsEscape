// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Creature.generated.h"

UCLASS()
class KNIGHTSESCAPE_API ACreature : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACreature();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxSpeed;

private:
	/** Input binding */
	void MoveForward(float MoveRate);
	void MoveRight(float MoveRate);
	void AttackPrimary();
	void AttackSecondary();
	void AttackSpecial();
	void Dodge();
	void Interact();
	void UseConsumable();
	void CycleTargetRight();
	void CycleTargetLeft();
	void OpenInventory();

	FVector CurrentVelocity;
	
};
