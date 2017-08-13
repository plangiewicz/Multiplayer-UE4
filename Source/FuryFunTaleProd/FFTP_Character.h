// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FFTP_BaseCharacter.h"
#include "FFTP_Character.generated.h"

/**
 * 
 */
UCLASS()
class FURYFUNTALEPROD_API AFFTP_Character : public AFFTP_BaseCharacter
{
	GENERATED_BODY()
		
	AFFTP_Character(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	/* Called every frame */
	virtual void Tick(float DeltaSeconds) override;

	/* Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	//virtual void PawnClientRestart() override;
	
private:

	/* Boom to handle distance to player mesh. */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
		USpringArmComponent* CameraBoomComp;

	/* Primary camera of the player*/
	UPROPERTY(VisibleAnywhere, Category = "Camera")
		UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
		class UFFTP_CarryObjectComponent* CarriedObjectComp;
public:

	float ItemWidgetOffset;

	FORCEINLINE UCameraComponent* GetCameraComponent()
	{
		return CameraComp;
	}


	///* Player is looking at */
	//virtual void OnBeginFocus() override;

	///* Player is no longer looking at */
	//virtual void OnEndFocus() override;

	/************************************************************************/
	/* Movement                                                             */
	/************************************************************************/

	virtual void MoveForward(float Val);

	virtual void MoveRight(float Val);

	/* Client mapped to Input */
	void OnCrouchToggle();

	/* Client mapped to Input */
	void OnJump();

	/* Client mapped to Input */
	void OnStartSprinting();

	/* Client mapped to Input */
	void OnStopSprinting();

	virtual void SetSprinting(bool NewSprinting) override;

	/* Is character currently performing a jump action. Resets on landed.  */
	UPROPERTY(Transient, Replicated)
		bool bIsJumping;

	UFUNCTION(BlueprintCallable, Category = "Movement")
		bool IsInitiatedJump() const;

	void SetIsJumping(bool NewJumping);

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerSetIsJumping(bool NewJumping);

	void ServerSetIsJumping_Implementation(bool NewJumping);

	bool ServerSetIsJumping_Validate(bool NewJumping);

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;


	/************************************************************************/
	/* Object Interaction                                                   */
	/************************************************************************/

	/* Input mapped function for carry object component */
	void OnToggleCarryActor();

	/* Use the usable actor currently in focus, if any */
	virtual void Use();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerUse();

	void ServerUse_Implementation();

	bool ServerUse_Validate();

	class AFFTP_UsableActor* GetUsableInView();
	class AFFTP_BaseCharacter* GetCharacterInView();

	/*Max distance to use/focus on actors. */
	UPROPERTY(EditDefaultsOnly, Category = "ObjectInteraction")
		float MaxUseDistance;

	/* True only in first frame when focused on a new usable actor. */
	bool bHasNewFocus;

	class AFFTP_UsableActor* FocusedUsableActor;
	class AFFTP_BaseCharacter* FocusedCharacterBase;


	/************************************************************************/
	/* Weapons & Inventory                                                  */
	/************************************************************************/

private:

	/* Attachpoint for active weapon/item in hands */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName WeaponAttachPoint;

	/* Attachpoint for items carried on the belt/pelvis. */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName PelvisAttachPoint;

	/* Attachpoint for primary weapons */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName SpineAttachPoint;

	bool bWantsToFire;

	/* Distance away from character when dropping inventory items. */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		float DropWeaponMaxDistance;

	/****************************************************************************
	 *  HUD
	 ******************************************************************************/

	void ShowPlayerName();
};
