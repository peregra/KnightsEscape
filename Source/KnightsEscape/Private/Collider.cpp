// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ColliderMovementComponent.h"

// Sets default values
ACollider::ACollider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	AxeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("AxeComponent"));
	AxeComponent->SetupAttachment(GetRootComponent());
	AxeComponent->InitSphereRadius(40.f);
	AxeComponent->SetCollisionProfileName(TEXT("Pawn"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	/*static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/Weapon_Pack/Mesh/Weapons/Weapons_Kit/SM_Axe.SM_Axe'"));
	if (MeshComponentAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MeshComponentAsset.Object);
		MeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -20.f));
		MeshComponent->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
	}
*/
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->RelativeRotation = FRotator(-45.f, 0.f, 0.f);
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 3.f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	ColliderMovementComponent = CreateDefaultSubobject<UColliderMovementComponent>(TEXT("MovementComponent"));
	ColliderMovementComponent->UpdatedComponent = RootComponent;

	CameraInput = FVector2D(0.f, 0.f);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += CameraInput.X;
	SetActorRotation(NewRotation);

	FRotator NewSpringArmRotation = SpringArmComponent->GetComponentRotation();
	NewSpringArmRotation.Pitch = FMath::Clamp(NewSpringArmRotation.Pitch += CameraInput.Y, -60.f, -5.f);

	SpringArmComponent->SetWorldRotation(NewSpringArmRotation);
}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACollider::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACollider::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &ACollider::YawCamera);
	PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &ACollider::PitchCamera);
}


void ACollider::MoveForward(float MoveRate)
{
	FVector Forward = GetActorForwardVector();
	if (ColliderMovementComponent)
	{
		ColliderMovementComponent->AddInputVector(Forward * MoveRate);
	}
}


void ACollider::MoveRight(float MoveRate)
{
	FVector Right = GetActorRightVector();
	{
		ColliderMovementComponent->AddInputVector(Right * MoveRate);
	}
}


void ACollider::YawCamera(float AxisValue)
{
	CameraInput.X = AxisValue;
}


void ACollider::PitchCamera(float AxisValue)
{
	CameraInput.Y = AxisValue;
}


UPawnMovementComponent* ACollider::GetMovementComponent() const
{
	return ColliderMovementComponent;
}