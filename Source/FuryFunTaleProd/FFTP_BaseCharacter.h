// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "FFTP_BaseCharacter.generated.h"

UCLASS(ABSTRACT)
class FURYFUNTALEPROD_API AFFTP_BaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFFTP_BaseCharacter(const class FObjectInitializer& ObjectInitializer);


	FVector MeshBottomPoint;
	FVector MeshTopPoint;
	float _widgetOffset;

	virtual FString GetTextAbout();
	FVector GetWidgetPosition();

	/* Player is looking at */
	virtual void OnBeginFocus();

	/* Player is no longer looking at */
	virtual void OnEndFocus();

	/************************************************************************/
	/* Targeting                                                            */
	/************************************************************************/

	void SetTargeting(bool NewTargeting);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetTargeting(bool NewTargeting);

	void ServerSetTargeting_Implementation(bool NewTargeting);

	bool ServerSetTargeting_Validate(bool NewTargeting);

	UPROPERTY(Transient, Replicated)
		bool bIsTargeting;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
		float TargetingSpeedModifier;


	/* Is player aiming down sights */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
		bool IsTargeting() const;

	float GetTargetingSpeedModifier() const;

	/* Retrieve Pitch/Yaw from current camera */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
		FRotator GetAimOffsets() const;

	/**********************************************************************/
	/* Movement                                                           */
	/**********************************************************************/

	UFUNCTION(BlueprintCallable, Category = "Movement")
		virtual bool IsSprinting() const;

	/* Client/local call to update sprint state  */
	virtual void SetSprinting(bool NewSprinting);

	float GetSprintingSpeedModifier() const;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float SprintingSpeedModifier;

	/* Character wants to run, checked during Tick to see if allowed */
	UPROPERTY(Transient, Replicated)
		bool bWantsToRun;

	/* Server side call to update actual sprint state */
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetSprinting(bool NewSprinting);

	void ServerSetSprinting_Implementation(bool NewSprinting);

	bool ServerSetSprinting_Validate(bool NewSprinting);

	/************************************************************************/
	/* Damage & Death                                                       */
	/************************************************************************/

protected:

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition", Replicated)
		float Health;
};
