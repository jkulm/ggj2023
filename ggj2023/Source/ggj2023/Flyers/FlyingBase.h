// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FlyingBase.generated.h"

UENUM(BlueprintType)
enum class EOrbitDirection : uint8
{
	None UMETA(DisplayName = "None"),
	Clockwise UMETA(DisplayName = "Clockwise"),
	CounterClockwise UMETA(DisplayName = "CounterClockwise")
};

UENUM(BlueprintType)
enum class EPivotLocation : uint8
{
	None UMETA(DisplayName = "None"),
	Right UMETA(DisplayName = "Right"),
	Left UMETA(DisplayName = "Left")
};

UCLASS(BlueprintType)
class GGJ2023_API AFlyingBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFlyingBase();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void InitializePositioning();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Orbit(float DeltaTime);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bShouldOrbit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bRandomizeDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EOrbitDirection OrbitDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EPivotLocation PivotLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector PivotPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinTargetOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxTargetOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RPM;

	/** Always use a positive value here for normal banking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BankAngle;

	bool bPositioningInitialized = false;

	// Store upvector to avoid recalculating it every frame
	FVector UpVector;

	float AnglePerSec;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
