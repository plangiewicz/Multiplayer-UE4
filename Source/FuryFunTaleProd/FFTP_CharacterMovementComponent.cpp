// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FFTP_CharacterMovementComponent.h"
#include "FFTP_BaseCharacter.h"



float UFFTP_CharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const AFFTP_BaseCharacter* CharOwner = Cast<AFFTP_BaseCharacter>(PawnOwner);
	if (CharOwner)
	{
		// Slow down during targeting or crouching
		if (CharOwner->IsTargeting() && !IsCrouching())
		{
			MaxSpeed *= CharOwner->GetTargetingSpeedModifier();
		}
		else if (CharOwner->IsSprinting())
		{
			MaxSpeed *= CharOwner->GetSprintingSpeedModifier();
		}
	}

	return MaxSpeed;
}

