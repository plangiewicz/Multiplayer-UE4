// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "FFTP_PlayerController.h"
#include "FTTP_GameState.generated.h"

/**
 * 
 */
UCLASS()
class FURYFUNTALEPROD_API AFTTP_GameState : public AGameState
{
	GENERATED_BODY()
	

		/* Total accumulated score from all players  */
		UPROPERTY(Replicated)
		int32 TotalScore;

public:

	UFUNCTION(BlueprintCallable, Category = "Score")
		int32 GetTotalScore();

	void AddScore(int32 Score);

	AFTTP_GameState(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(Replicated)
		bool bIsNight;

	/* Time in wallclock hours the day begins */
	float SunriseTimeMark;

	/* Time in wallclock hours the night begins */
	float SunsetTimeMark;

	bool GetIsNight();

	bool GetAndUpdateIsNight();

	/* Current time of day in the gamemode represented in full minutes */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "TimeOfDay")
		int32 ElapsedGameMinutes;

	/* Conversion of 1 second real time to X seconds gametime of the day/night cycle */
	UPROPERTY(EditDefaultsOnly, Category = "TimeOfDay")
		float TimeScale;

	/* Returns the time of day increment every real second (converted to accelerated game time, eg. 1 real second is 1 minute in time of day increment) */
	float GetTimeOfDayIncrement();

	UFUNCTION(BlueprintCallable, Category = "TimeOfDay")
		int32 GetElapsedDays();

	/* Returns whole days elapsed, represented in minutes */
	UFUNCTION(BlueprintCallable, Category = "TimeOfDay")
		int32 GetElapsedFullDaysInMinutes();

	/* Return the time in real seconds till next sunrise */
	UFUNCTION(BlueprintCallable, Category = "TimeOfDay")
		int32 GetRealSecondsTillSunrise();

	int32 GetElapsedMinutesCurrentDay();

	/* By passing in "exec" we expose it as a command line (press ~ to open) */
	UFUNCTION(exec)
		void SetTimeOfDay(float NewTimeOfDay);

	/* NetMulticast will send this event to all clients that know about this object, in the case of GameState that means every client. */
	UFUNCTION(Reliable, NetMulticast)
		void BroadcastGameMessage(EHUDMessage NewMessage);

	void BroadcastGameMessage_Implementation(EHUDMessage MessageID);
	
	
};
