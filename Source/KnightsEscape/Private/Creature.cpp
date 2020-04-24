// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

// Sets default values
ACreature::ACreature()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateEditorOnlyDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetRelativeLocation(FVector(-300.f, 0.f, 300.f));
	CameraComponent->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

	// AutoPossessPlayer = EAutoReceiveInput::Player0;

	CurrentVelocity = FVector(0.f);
	MaxSpeed = 100.f;
}

// Called when the game starts or when spawned
void ACreature::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
	SetActorLocation(NewLocation);
}

// Called to bind functionality to input
void ACreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACreature::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACreature::MoveRight);

}


void ACreature::MoveForward(float MoveRate)
{
	CurrentVelocity.X = FMath::Clamp(MoveRate, -1.f, 1.f) * MaxSpeed;
}


void ACreature::MoveRight(float MoveRate)
{
	CurrentVelocity.Y = FMath::Clamp(MoveRate, -1.f, 1.f) * MaxSpeed;
}


void ACreature::AttackPrimary()
{

} // TODO


void ACreature::AttackSecondary()
{

} // TODO


void ACreature::AttackSpecial()
{

} // TODO


void ACreature::Dodge()
{

} // TODO


void ACreature::Interact()
{

} // TODO


void ACreature::UseConsumable()
{

} // TODO


void ACreature::CycleTargetRight()
{

} // TODO


void ACreature::CycleTargetLeft()
{

} // TODO


void ACreature::OpenInventory()
{

} // TODO