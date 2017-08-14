// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "FFTP_PlayerHUD.h"
#include "FFTP_DataBase.h"
#include "FFTP_PlayerController.generated.h"


UENUM()
enum class EHUDMessage : uint8
{

	Welcome,

	/* Weapons */
	Weapon_SlotTaken,

	/* Character */
	Character_EnergyRestored,

	/* Gamemode */
	Game_SurviveStart,
	Game_SurviveEnded,

	/* No category specified */
	None,
};

/**
 * 
 */
UCLASS()
class FURYFUNTALEPROD_API AFFTP_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
	AFFTP_PlayerController(const FObjectInitializer& ObjectInitializer);
	

	/* Flag to respawn or start spectating upon death */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
		bool bRespawnImmediately;

	/* Respawn or start spectating after dying */
	virtual void UnFreeze() override;

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerSuicide();

	void ServerSuicide_Implementation();

	bool ServerSuicide_Validate();

	FFTP_DataBase _database;

public:

	//void BeginPlay() override;

	//UUserWidget* MyMainMenu;

	bool LoginTry(FString login, FString password);

	UFUNCTION(Reliable, Client)
		void ClientHUDStateChanged(EHUDState NewState);

	void ClientHUDStateChanged_Implementation(EHUDState NewState);

	/* Enum is remapped to localized text before sending it to the HUD */
	UFUNCTION(Reliable, Client)
		void ClientHUDMessage(EHUDMessage MessageID);

	/* Enum is remapped to localized text before sending it to the HUD */
	UFUNCTION(Reliable, Client)
		void ClientHUDMessageAboveItem(const FString& TextMessage, const FVector& worldLocation);

	void ClientHUDMessage_Implementation(EHUDMessage MessageID);

	FText GetText(EHUDMessage MsgID);

	/* Kill the current pawn */
	UFUNCTION(exec)
		virtual void Suicide();

	/* Start spectating. Should be called only on server */
	void StartSpectating();
};
