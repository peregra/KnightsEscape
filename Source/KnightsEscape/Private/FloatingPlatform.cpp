// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"


// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);

	InterpolateSpeed = 4.0f;
	InterpolateTime = 1.0f;

	bInterpolating = false;
}


// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint = GetActorLocation();
	EndPoint += StartPoint;

	GetWorldTimerManager().SetTimer(InterpolateTimer, this, &AFloatingPlatform::ToggleInterpolating, InterpolateTime);

	Distance = (EndPoint - StartPoint).Size();
}


// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	if (bInterpolating)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Interpolate = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpolateSpeed);
		SetActorLocation(Interpolate);

		float DistanceTraveled = (GetActorLocation() - StartPoint).Size();
		if (Distance - DistanceTraveled <= 1.f)
		{
			ToggleInterpolating();
			GetWorldTimerManager().SetTimer(InterpolateTimer, this, &AFloatingPlatform::ToggleInterpolating, InterpolateTime);
			SwapVectors(StartPoint, EndPoint);
		}
	}
}


void AFloatingPlatform::ToggleInterpolating()
{
	bInterpolating = !bInterpolating;
}


void AFloatingPlatform::SwapVectors(FVector& Vector1, FVector& Vector2)
{
	FVector Temp = Vector1;
	Vector1 = Vector2;
	Vector2 = Temp;
}