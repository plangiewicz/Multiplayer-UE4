// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FFTP_Character.h"
#include "FFTP_CarryObjectComponent.h"
#include "FFTP_UsableActor.h"
#include "FFTP_TakeableActor.h"
#include "FFTP_PlayerController.h"
#include <string>


// Sets default values
AFFTP_Character::AFFTP_Character(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	// Adjust jump to make it less floaty
	MoveComp->GravityScale = 1.5f;
	MoveComp->JumpZVelocity = 620;
	MoveComp->bCanWalkOffLedgesWhenCrouching = true;
	MoveComp->MaxWalkSpeedCrouched = 200;

	/* Ignore this channel or it will absorb the trace impacts instead of the skeletal mesh */
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	// Enable crouching
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;

	CameraBoomComp = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoomComp->SocketOffset = FVector(0, 35, 0);
	CameraBoomComp->TargetOffset = FVector(0, 0, 55);
	CameraBoomComp->bUsePawnControlRotation = true;
	CameraBoomComp->SetupAttachment(GetRootComponent());

	CameraComp = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	CameraComp->SetupAttachment(CameraBoomComp);

	CarriedObjectComp = ObjectInitializer.CreateDefaultSubobject<UFFTP_CarryObjectComponent>(this, TEXT("CarriedObjectComp"));
	CarriedObjectComp->SetupAttachment(GetRootComponent());

	MaxUseDistance = 500;
	DropWeaponMaxDistance = 100;
	bHasNewFocus = true;
	TargetingSpeedModifier = 0.5f;
	SprintingSpeedModifier = 2.5f;

	Health = 100;

	ItemWidgetOffset = 10.0f;

	//IncrementHungerAmount = 5.0f;
	//IncrementHungerInterval = 5.0f;
	//CriticalHungerThreshold = 90;
	//HungerDamagePerInterval = 1.0f;
	//MaxHunger = 100;
	//Hunger = 0;

	/* Names as specified in the character skeleton */
	WeaponAttachPoint = TEXT("WeaponSocket");
	PelvisAttachPoint = TEXT("PelvisSocket");
	SpineAttachPoint = TEXT("SpineSocket");
}


//void AFFTP_Character::OnBeginFocus()
//	:Super()
//{
//	// Used by custom PostProcess to render outlines
//	GetMesh()->SetRenderCustomDepth(true);
//	//GetMesh()->boun
//	//GetMesh()->GetLocalBounds(MeshBottomPoint, MeshTopPoint);
//}
//
//void AFFTP_Character::OnEndFocus()
//{
//	// Used by custom PostProcess to render outlines
//	GetMesh()->SetRenderCustomDepth(false);
//}

void AFFTP_Character::ShowPlayerName()
{
	AFFTP_PlayerController* PC = Cast<AFFTP_PlayerController>(Controller);
	if (PC)
	{
		PC->ClientHUDMessage(EHUDMessage::Welcome);
	}
}

void AFFTP_Character::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		// Set a timer to increment hunger every interval
		//FTimerHandle Handle;
		//GetWorldTimerManager().SetTimer(Handle, this, &AFFTP_Character::IncrementHunger, IncrementHungerInterval, true);
		ShowPlayerName();
	}
}


void AFFTP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AFFTP_PlayerController* PC = Cast<AFFTP_PlayerController>(Controller);
	if (PC)
	{
		PC->ClientHUDMessageAboveItem(FString("NONE"), FVector(0,0,0));
	}

	if (bWantsToRun && !IsSprinting())
	{
		SetSprinting(true);
	}

	if (Controller && Controller->IsLocalController())
	{
		//Find usable item in viewport
		AFFTP_UsableActor* Usable = GetUsableInView();

		//Find character/player/npc... in viewport
		AFFTP_BaseCharacter* Character = GetCharacterInView();


		if (FocusedUsableActor != Usable)
		{
			if (FocusedUsableActor)
			{
				FocusedUsableActor->OnEndFocus();
			}

			bHasNewFocus = true;
		}


		if (FocusedCharacterBase != Character)
		{
			if (FocusedCharacterBase)
			{
				FocusedCharacterBase->OnEndFocus();
			}

			bHasNewFocus = true;
		}

		FocusedUsableActor = Usable;
		FocusedCharacterBase = Character;

		// Start Focus.
		if (Usable)
		{

			//AFFTP_PlayerController* PC = Cast<AFFTP_PlayerController>(Controller);
			if (PC)
			{
				PC->ClientHUDMessageAboveItem(Usable->GetTextAbout(), Usable->GetWidgetPosition());
			}

			if (bHasNewFocus)
			{
				Usable->OnBeginFocus();
				bHasNewFocus = false;
			}
		}

		if (Character)
		{
			if (PC)
			{
				PC->ClientHUDMessageAboveItem(Character->GetTextAbout(), Character->GetWidgetPosition());
			}
			if (bHasNewFocus)
			{
				Character->OnBeginFocus();
				bHasNewFocus = false;
			}
		}
	}

}

void AFFTP_Character::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//DestroyInventory();
}


// Called to bind functionality to input
void AFFTP_Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movement
	PlayerInputComponent->BindAxis("MoveForward", this, &AFFTP_Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFFTP_Character::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("SprintHold", IE_Pressed, this, &AFFTP_Character::OnStartSprinting);
	PlayerInputComponent->BindAction("SprintHold", IE_Released, this, &AFFTP_Character::OnStopSprinting);

	PlayerInputComponent->BindAction("CrouchToggle", IE_Released, this, &AFFTP_Character::OnCrouchToggle);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFFTP_Character::OnJump);

	// Interaction
	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &AFFTP_Character::Use);
	//PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &AFFTP_Character::DropWeapon);

	//// Weapons
	//PlayerInputComponent->BindAction("Targeting", IE_Pressed, this, &AFFTP_Character::OnStartTargeting);
	//PlayerInputComponent->BindAction("Targeting", IE_Released, this, &AFFTP_Character::OnEndTargeting);

	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFFTP_Character::OnStartFire);
	//PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFFTP_Character::OnStopFire);

	//PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFFTP_Character::OnReload);

	//PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &AFFTP_Character::OnNextWeapon);
	//PlayerInputComponent->BindAction("PrevWeapon", IE_Pressed, this, &AFFTP_Character::OnPrevWeapon);

	//PlayerInputComponent->BindAction("EquipPrimaryWeapon", IE_Pressed, this, &AFFTP_Character::OnEquipPrimaryWeapon);
	//PlayerInputComponent->BindAction("EquipSecondaryWeapon", IE_Pressed, this, &AFFTP_Character::OnEquipSecondaryWeapon);

	/* Input binding for the carry object component */
	//PlayerInputComponent->BindAction("PickupObject", IE_Pressed, this, &AFFTP_Character::OnToggleCarryActor);
}


void AFFTP_Character::MoveForward(float Val)
{
	if (Controller && Val != 0.f)
	{
		// Limit pitch when walking or falling
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

		AddMovementInput(Direction, Val);
	}
}


void AFFTP_Character::MoveRight(float Val)
{
	if (Val != 0.f)
	{
		const FRotator Rotation = GetActorRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Val);
	}
}

void AFFTP_Character::OnCrouchToggle()
{
	if (IsSprinting())
	{
		SetSprinting(false);
	}

	// If we are crouching then CanCrouch will return false. If we cannot crouch then calling Crouch() wont do anything
	if (CanCrouch())
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void AFFTP_Character::OnJump()
{
	SetIsJumping(true);
}


bool AFFTP_Character::IsInitiatedJump() const
{
	return bIsJumping;
}


void AFFTP_Character::SetIsJumping(bool NewJumping)
{
	// Go to standing pose if trying to jump while crouched
	if (bIsCrouched && NewJumping)
	{
		UnCrouch();
	}
	else if (NewJumping != bIsJumping)
	{
		bIsJumping = NewJumping;

		if (bIsJumping)
		{
			/* Perform the built-in Jump on the character */
			Jump();
		}
	}

	if (Role < ROLE_Authority)
	{
		ServerSetIsJumping(NewJumping);
	}
}


void AFFTP_Character::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	/* Check if we are no longer falling/jumping */
	if (PrevMovementMode == EMovementMode::MOVE_Falling &&
		GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Falling)
	{
		SetIsJumping(false);
	}
}



void AFFTP_Character::ServerSetIsJumping_Implementation(bool NewJumping)
{
	SetIsJumping(NewJumping);
}


bool AFFTP_Character::ServerSetIsJumping_Validate(bool NewJumping)
{
	return true;
}



void AFFTP_Character::OnStartSprinting()
{
	if (CarriedObjectComp->GetIsCarryingActor())
	{
		CarriedObjectComp->Drop();
	}

	SetSprinting(true);
}

void AFFTP_Character::OnStopSprinting()
{
	SetSprinting(false);
}


void AFFTP_Character::SetSprinting(bool NewSprinting)
{
	if (bWantsToRun)
	{
		//StopWeaponFire();
	}

	Super::SetSprinting(NewSprinting);
}


void AFFTP_Character::OnToggleCarryActor()
{
	CarriedObjectComp->Pickup();
}


void AFFTP_Character::Use()
{
	// Only allow on server. If called on client push this request to the server
	if (Role == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Use"));
		AFFTP_UsableActor* Usable = GetUsableInView();
		if (Usable)
		{
			Usable->OnUsed(this);
		}
	}
	else
	{
		ServerUse();
	}
}


void AFFTP_Character::ServerUse_Implementation()
{
	Use();
}


bool AFFTP_Character::ServerUse_Validate()
{
	return true;
}

/*
Performs ray-trace to find closest looked-at UsableActor.
*/
AFFTP_UsableActor* AFFTP_Character::GetUsableInView()
{
	FVector CamLoc;
	FRotator CamRot;

	if (Controller == nullptr)
		return nullptr;

	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * MaxUseDistance);

	FCollisionQueryParams TraceParams(TEXT("TraceUsableActor"), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;

	/* Not tracing complex uses the rough collision instead making tiny objects easier to select. */
	TraceParams.bTraceComplex = false;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f);
	
	return Cast<AFFTP_UsableActor>(Hit.GetActor());
}

AFFTP_BaseCharacter* AFFTP_Character::GetCharacterInView()
{
	FVector CamLoc;
	FRotator CamRot;

	if (Controller == nullptr)
		return nullptr;

	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * MaxUseDistance);

	FCollisionQueryParams TraceParams(TEXT("TraceCharacterBase"), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;

	/* Not tracing complex uses the rough collision instead making tiny objects easier to select. */
	TraceParams.bTraceComplex = false;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, TraceParams);

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f);
	
	return Cast<AFFTP_BaseCharacter>(Hit.GetActor());
}


void AFFTP_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Value is already updated locally, skip in replication step
	DOREPLIFETIME_CONDITION(AFFTP_Character, bIsJumping, COND_SkipOwner);

	// Replicate to every client, no special condition required
	//DOREPLIFETIME(AFFTP_UsableActor, Hunger);

	//DOREPLIFETIME(AFFTP_UsableActor, LastTakeHitInfo);

	//DOREPLIFETIME(AFFTP_UsableActor, CurrentWeapon);
	//DOREPLIFETIME(AFFTP_UsableActor, Inventory);
	/* If we did not display the current inventory on the player mesh we could optimize replication by using this replication condition. */
	/* DOREPLIFETIME_CONDITION(ASCharacter, Inventory, COND_OwnerOnly);*/
}