// Fill out your copyright notice in the Description page of Project Settings.

#include "FlyingBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"

AFlyingBase::AFlyingBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set defaults
	bShouldOrbit = true;
	bRandomizeDirection = true;
	OrbitDirection = EOrbitDirection::None;
	PivotLocation = EPivotLocation::None;
	BankAngle = 20.0f;
}

void AFlyingBase::BeginPlay()
{
	Super::BeginPlay();

	if (!bShouldOrbit)
	{
		return;
	}
	InitializePositioning();
}

void AFlyingBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bShouldOrbit || !bPositioningInitialized)
	{
		return;
	}
	if (bRandomizeDirection)
	{
		FRotator CurrentRotation = GetActorRotation();
		CurrentRotation.Yaw = FMath::RandRange(0.0f, 360.0f);
	}
	if (OrbitDirection == EOrbitDirection::None)
	{
		int RandomOrbitDirection = FMath::RandRange(0, 1);
		if (RandomOrbitDirection == 0 || !bRandomizeDirection)
		{
			OrbitDirection = EOrbitDirection::Clockwise;
		}
		else
		{
			OrbitDirection = EOrbitDirection::CounterClockwise;
		}
	}
	if (PivotLocation == EPivotLocation::None)
	{
		int RandomPivotLocation = FMath::RandRange(0, 1);
		if (RandomPivotLocation == 0 || !bRandomizeDirection)
		{
			PivotLocation = EPivotLocation::Right;
		}
		else
		{
			PivotLocation = EPivotLocation::Left;
		}
	}
	Orbit(DeltaTime);
}

void AFlyingBase::InitializePositioning()
{
	float AverageOffset = (MaxTargetOffset - MinTargetOffset) / 2;
	float Offset = FMath::RandRange(MinTargetOffset, MaxTargetOffset);

	// Bank with a steeper angle if offset is small
	float BankAngleModifier = FMath::Clamp(AverageOffset / Offset, 0.5, 2);
	BankAngle *= BankAngleModifier;

	FVector ActorLocation = GetActorLocation();
	FVector ActorForwardVector = GetActorForwardVector();
	FVector ActorRotatedVector = ActorForwardVector;
	if (PivotLocation == EPivotLocation::Right)
	{
		ActorRotatedVector = ActorForwardVector.RotateAngleAxis(90.0f, FVector::UpVector);
	}
	else
	{
		ActorRotatedVector = ActorForwardVector.RotateAngleAxis(-90.0f, FVector::UpVector);
	}
	PivotPoint = ActorLocation + (ActorRotatedVector * Offset);
	Radius = (PivotPoint - ActorLocation).Size();
	bPositioningInitialized = true;
}

void AFlyingBase::Orbit(float DeltaTime)
{
	float RPM = 0.1;
	FVector radius = GetActorLocation() - PivotPoint; // vector from PivotPoint to current location
	float angle = (RPM / 60.f) * 360.f * DeltaTime;	  // the angle to rotate by, in degrees

	if (OrbitDirection == EOrbitDirection::CounterClockwise)
	{
		angle *= -1;
	}

	// Rotate the radius vector around the Z-axis by angle degrees
	FVector newRadius(radius.X * FMath::Cos(angle) - radius.Y * FMath::Sin(angle),
					  radius.X * FMath::Sin(angle) + radius.Y * FMath::Cos(angle),
					  radius.Z);

	// Update the actor's location
	SetActorLocation(PivotPoint + newRadius);

	// Lock rotation to the orbit
	FVector TidalLock;
	FRotator BankedRotation;
	if (OrbitDirection == EOrbitDirection::Clockwise)
	{
		FVector Forward = (PivotPoint - GetActorLocation()).GetSafeNormal();
		TidalLock = FVector::CrossProduct(Forward, FVector::UpVector).GetSafeNormal();
		BankedRotation = TidalLock.Rotation();
		BankedRotation.Roll = 10.0f;
	}
	else
	{
		FVector Forward = (PivotPoint - GetActorLocation()).GetSafeNormal();
		TidalLock = FVector::CrossProduct(FVector::UpVector, Forward).GetSafeNormal();
		BankedRotation = TidalLock.Rotation();
		BankedRotation.Roll = -10.0f;
	}
	SetActorRotation(BankedRotation);
}