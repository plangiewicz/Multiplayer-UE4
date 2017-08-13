// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "FFTP_CharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class FURYFUNTALEPROD_API UFFTP_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	virtual float GetMaxSpeed() const override;
	
};
