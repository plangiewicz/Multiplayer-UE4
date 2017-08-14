// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "Engine/Canvas.h"
#include "FFTP_PlayerHUD.generated.h"


/* Expose it to Blueprint using this tag */
UENUM(BlueprintType)
enum class EHUDState : uint8
{
	Playing,
	Spectating,
	MatchEnd
};

UCLASS()
class FURYFUNTALEPROD_API AFFTP_PlayerHUD : public AHUD
{
	GENERATED_BODY()
	
	AFFTP_PlayerHUD(const FObjectInitializer& ObjectInitializer);

	FCanvasIcon CenterDotIcon;

	/* Current HUD state */
	EHUDState CurrentState;

	/** Main HUD update loop. */
	virtual void DrawHUD() override;

	void DrawCenterDot();


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InGameHUD)
	bool bInGame;

	//void BeginPlay() override;

	void AccessToPlay(bool access);

	UFUNCTION(BlueprintCallable, Category = "HUD")
		EHUDState GetCurrentState();

	/* An event hook to call HUD text events to display in the HUD. Blueprint HUD class must implement how to deal with this event. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUDEvents")
		void MessageReceived(const FText& TextMessage);

	/* Event hook to update HUD state (eg. to determine visibility of widgets) */
	UFUNCTION(BlueprintNativeEvent, Category = "HUDEvents")
		void OnStateChanged(EHUDState NewState);


	UFUNCTION(BlueprintImplementableEvent, Category = "HUDEvents")
		void MessageAboveItem(const FString& TextMessage, const FVector2D& screenLocation);
};
