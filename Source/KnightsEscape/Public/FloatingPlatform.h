// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class KNIGHTSESCAPE_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	/** Mesh for platform */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
	class UStaticMeshComponent* Mesh;

	/** For moving platforms, set start and end points */
	UPROPERTY(EditAnywhere)
	FVector StartPoint;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector EndPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpolateSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpolateTime;

	FTimerHandle InterpolateTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	bool bInterpolating;

	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ToggleInterpolating();

	void SwapVectors(FVector& Vector1, FVector& Vector2);

};
