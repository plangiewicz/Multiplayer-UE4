// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FFTP_BaseCharacter.h"
#include "FFTP_CharacterMovementComponent.h"
#include <functional>


// Sets default values
AFFTP_BaseCharacter::AFFTP_BaseCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UFFTP_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	Health = 100;

	TargetingSpeedModifier = 0.5f;
	SprintingSpeedModifier = 2.0f;

	/* Noise emitter for both players and enemies. This keeps track of MakeNoise data and is used by the pawnsensing component in our SZombieCharacter class */
	//NoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitterComp"));

	/* Don't collide with camera checks to keep 3rd person camera at position when zombies or other players are standing behind us */
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	_widgetOffset = 10.0f;
}


void AFFTP_BaseCharacter::OnBeginFocus()
{
	// Used by custom PostProcess to render outlines
	GetMesh()->SetRenderCustomDepth(true);
	MeshTopPoint = GetMesh()->SkeletalMesh->GetImportedBounds().BoxExtent;
	MeshBottomPoint = GetMesh()->SkeletalMesh->GetImportedBounds().BoxExtent;
	//GetMesh()->GetLocalBounds(MeshBottomPoint, MeshTopPoint);
}

FString AFFTP_BaseCharacter::GetTextAbout()
{
	FString retString = "Character";
	return retString;
}

FVector AFFTP_BaseCharacter::GetWidgetPosition()
{
	return FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + MeshTopPoint.Z * GetActorScale().Z/* + _widgetOffset*/);	
}

void AFFTP_BaseCharacter::OnEndFocus()
{
	// Used by custom PostProcess to render outlines
	GetMesh()->SetRenderCustomDepth(false);
}

void AFFTP_BaseCharacter::SetTargeting(bool NewTargeting)
{
	bIsTargeting = NewTargeting;

	if (Role < ROLE_Authority)
	{
		ServerSetTargeting(NewTargeting);
	}
}

void AFFTP_BaseCharacter::ServerSetTargeting_Implementation(bool NewTargeting)
{
	SetTargeting(NewTargeting);
}

bool AFFTP_BaseCharacter::ServerSetTargeting_Validate(bool NewTargeting)
{
	return true;
}

bool AFFTP_BaseCharacter::IsTargeting() const
{
	return bIsTargeting;
}

float AFFTP_BaseCharacter::GetTargetingSpeedModifier() const
{
	return TargetingSpeedModifier;
}

FRotator AFFTP_BaseCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}


// MOVEMENT


void AFFTP_BaseCharacter::SetSprinting(bool NewSprinting)
{
	bWantsToRun = NewSprinting;

	if (bIsCrouched)
	{
		UnCrouch();
	}

	if (Role < ROLE_Authority)
	{
		ServerSetSprinting(NewSprinting);
	}
}

bool AFFTP_BaseCharacter::IsSprinting() const
{
	if (!GetCharacterMovement())
	{
		return false;
	}

	return bWantsToRun && !IsTargeting() && !GetVelocity().IsZero()
		// Don't allow sprint while strafing sideways or standing still (1.0 is straight forward, -1.0 is backward while near 0 is sideways or standing still)
		&& (FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) > 0.8); // Changing this value to 0.1 allows for diagonal sprinting. (holding W+A or W+D keys)
}


float AFFTP_BaseCharacter::GetSprintingSpeedModifier() const
{
	return SprintingSpeedModifier;
}

void AFFTP_BaseCharacter::ServerSetSprinting_Implementation(bool NewSprinting)
{
	SetSprinting(NewSprinting);
}


bool AFFTP_BaseCharacter::ServerSetSprinting_Validate(bool NewSprinting)
{
	return true;
}


void AFFTP_BaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Value is already updated locally, skip in replication step
	DOREPLIFETIME_CONDITION(AFFTP_BaseCharacter, bWantsToRun, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AFFTP_BaseCharacter, bIsTargeting, COND_SkipOwner);

	// Replicate to every client, no special condition required
	DOREPLIFETIME(AFFTP_BaseCharacter, Health);
	//DOREPLIFETIME(AFFTP_BaseCharacter, LastTakeHitInfo);
}