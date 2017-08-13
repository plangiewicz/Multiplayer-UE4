// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FFTP_PlayerController.h"
#include "FFTP_PlayerCameraManager.h"
#include "FTTP_GameState.h"
#include "FFTP_Character.h"


/* Define a log category for error messages */
DEFINE_LOG_CATEGORY_STATIC(LogGame, Log, All);


AFFTP_PlayerController::AFFTP_PlayerController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* Assign the class types we wish to use */
	PlayerCameraManagerClass = AFFTP_PlayerCameraManager::StaticClass();

	/* Example - Can be set to true for debugging, generally a value like this would exist in the GameMode instead */
	bRespawnImmediately = false;
}


void AFFTP_PlayerController::UnFreeze()
{
	Super::UnFreeze();

	// Check if match is ending or has ended.
	AFTTP_GameState* MyGameState = GetWorld()->GetGameState<AFTTP_GameState>();
	if (MyGameState && MyGameState->HasMatchEnded())
	{
		/* Don't allow spectating or respawns */
		return;
	}

	/* Respawn or spectate */
	if (bRespawnImmediately)
	{
		ServerRestartPlayer();
	}
	else
	{
		StartSpectating();
	}
}


void AFFTP_PlayerController::StartSpectating()
{
	/* Update the state on server */
	PlayerState->bIsSpectator = true;
	/* Waiting to respawn */
	bPlayerIsWaiting = true;
	ChangeState(NAME_Spectating);
	/* Push the state update to the client */
	ClientGotoState(NAME_Spectating);

	/* Focus on the remaining alive player */
	ViewAPlayer(1);

	/* Update the HUD to show the spectator screen */
	ClientHUDStateChanged(EHUDState::Spectating);
}


void AFFTP_PlayerController::Suicide()
{
	if (IsInState(NAME_Playing))
	{
		ServerSuicide();
	}
}

void AFFTP_PlayerController::ServerSuicide_Implementation()
{
	AFFTP_Character* MyPawn = Cast<AFFTP_Character>(GetPawn());
	if (MyPawn && ((GetWorld()->TimeSeconds - MyPawn->CreationTime > 1) || (GetNetMode() == NM_Standalone)))
	{
		//MyPawn->Suicide();
		// TODO: Player suicide
	}
}


bool AFFTP_PlayerController::ServerSuicide_Validate()
{
	return true;
}


void AFFTP_PlayerController::ClientHUDStateChanged_Implementation(EHUDState NewState)
{
	AFFTP_PlayerHUD* HUD = Cast<AFFTP_PlayerHUD>(GetHUD());
	if (HUD)
	{
		HUD->OnStateChanged(NewState);
	}
}


void AFFTP_PlayerController::ClientHUDMessage_Implementation(EHUDMessage MessageID)
{
	/* Turn the ID into a message for the HUD to display */
	FText TextMessage = GetText(MessageID);

	AFFTP_PlayerHUD* HUD = Cast<AFFTP_PlayerHUD>(GetHUD());
	if (HUD)
	{
		/* Implemented in SurvivalHUD Blueprint */
		HUD->MessageReceived(TextMessage);
	}
}


void AFFTP_PlayerController::ClientHUDMessageAboveItem_Implementation(const FString& TextMessage, const FVector& worldLocation)
{
	/* Turn the ID into a message for the HUD to display */
	//FText TextMessage = GetText(MessageID);

	AFFTP_PlayerHUD* HUD = Cast<AFFTP_PlayerHUD>(GetHUD());
	if (HUD)
	{
		FVector2D screenLocation;
		ProjectWorldLocationToScreen(worldLocation, screenLocation);
		HUD->MessageAboveItem(TextMessage, screenLocation);
	}
}

/* Temporarily set the namespace. If this was omitted, we should call NSLOCTEXT(Namespace, x, y) instead */
#define LOCTEXT_NAMESPACE "HUDMESSAGES"

FText AFFTP_PlayerController::GetText(EHUDMessage MsgID)
{
	switch (MsgID)
	{
	case EHUDMessage::Welcome:
		return LOCTEXT("WelcomeMessage", "Elo znowu w grze.");
	case EHUDMessage::Weapon_SlotTaken:
		return LOCTEXT("WeaponSlotTaken", "Weapon slot already taken.");
	case EHUDMessage::Character_EnergyRestored:
		return LOCTEXT("CharacterEnergyRestored", "Energy Restored");
	case EHUDMessage::Game_SurviveStart:
		return LOCTEXT("GameNightStart", "SURVIVE THE NIGHT");
	case EHUDMessage::Game_SurviveEnded:
		return LOCTEXT("GameNightEnd", "Night survived! Prepare for the coming night.");
	default:
		UE_LOG(LogGame, Warning, TEXT("No Message set for enum value in SPlayerContoller::GetText(). "))
			return FText::FromString("No Message Set");
	}
}

/* Remove the namespace definition so it doesn't exist in other files compiled after this one. */
#undef LOCTEXT_NAMESPACE