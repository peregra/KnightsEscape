// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class KNIGHTSESCAPE_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USphereComponent* AxeComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UColliderMovementComponent* ColliderMovementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	FORCEINLINE UStaticMeshComponent* GetMeshComponent() { return MeshComponent; };
	FORCEINLINE void SetMeshComponent(UStaticMeshComponent* Mesh) { MeshComponent = Mesh; };
	FORCEINLINE USphereComponent* GetSphereComponent() { return AxeComponent; };
	FORCEINLINE void SetSphereComponent(USphereComponent* Sphere) { AxeComponent = Sphere; };

	FORCEINLINE UCameraComponent* GetCameraComponent() { return CameraComponent; };
	FORCEINLINE void SetCameraComponent(UCameraComponent* Camera) { CameraComponent = Camera; };
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() { return SpringArmComponent; };
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* SpringArm) { SpringArmComponent = SpringArm; };

private:

	void MoveForward(float MoveRate);
	void MoveRight(float MoveRate);

	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);

	FVector2D CameraInput;
};
