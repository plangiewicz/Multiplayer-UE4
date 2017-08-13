// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FTTP_GameState.h"



AFTTP_GameState::AFTTP_GameState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* 1 SECOND real time is 1*TimeScale MINUTES game time */
	TimeScale = 8.0f;
	bIsNight = false;

	SunriseTimeMark = 6.0f;
	SunsetTimeMark = 18.0f;
}


void AFTTP_GameState::SetTimeOfDay(float NewTimeOfDay)
{
	ElapsedGameMinutes = NewTimeOfDay;
}


bool AFTTP_GameState::GetIsNight()
{
	return bIsNight;
}


float AFTTP_GameState::GetTimeOfDayIncrement()
{
	return (GetWorldSettings()->GetEffectiveTimeDilation() * TimeScale);
}


int32 AFTTP_GameState::GetElapsedDays()
{
	const float MinutesInDay = 24 * 60;
	const float ElapsedDays = ElapsedGameMinutes / MinutesInDay;
	return FMath::FloorToInt(ElapsedDays);
}


int32 AFTTP_GameState::GetElapsedFullDaysInMinutes()
{
	const int32 MinutesInDay = 24 * 60;
	return GetElapsedDays() * MinutesInDay;
}


bool AFTTP_GameState::GetAndUpdateIsNight()
{
	const float TimeOfDay = ElapsedGameMinutes - GetElapsedFullDaysInMinutes();
	if (TimeOfDay > (SunriseTimeMark * 60) && TimeOfDay < (SunsetTimeMark * 60))
	{
		bIsNight = false;
	}
	else
	{
		bIsNight = true;
	}

	return bIsNight;
}


int32 AFTTP_GameState::GetRealSecondsTillSunrise()
{
	float SunRiseMinutes = (SunriseTimeMark * 60);
	const int32 MinutesInDay = 24 * 60;

	float ElapsedTimeToday = GetElapsedMinutesCurrentDay();
	if (ElapsedTimeToday < SunRiseMinutes)
	{
		/* Still early in day cycle, so easy to get remaining time */
		return (SunRiseMinutes - ElapsedTimeToday) / TimeScale;
	}
	else
	{
		/* Sunrise will happen "tomorrow" so we need to add another full day to get remaining time */
		float MaxTimeTillNextSunrise = MinutesInDay + SunRiseMinutes;
		return (MaxTimeTillNextSunrise - ElapsedTimeToday) / TimeScale;
	}
}


int32 AFTTP_GameState::GetElapsedMinutesCurrentDay()
{
	return ElapsedGameMinutes - GetElapsedFullDaysInMinutes();
}


/* As with Server side functions, NetMulticast functions have a _Implementation body */
void AFTTP_GameState::BroadcastGameMessage_Implementation(EHUDMessage MessageID)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AFFTP_PlayerController* MyController = Cast<AFFTP_PlayerController>(*It);
		if (MyController && MyController->IsLocalController())
		{
			MyController->ClientHUDMessage(MessageID);
			if (MessageID == EHUDMessage::Game_SurviveStart)
				UE_LOG(LogTemp, Warning, TEXT("Game_SurviveStart"));
			if (MessageID == EHUDMessage::Game_SurviveEnded)
				UE_LOG(LogTemp, Warning, TEXT("Game_SurviveEnded"));
			if (MessageID == EHUDMessage::Character_EnergyRestored)
				UE_LOG(LogTemp, Warning, TEXT("Character_EnergyRestored"));
			if (MessageID == EHUDMessage::Weapon_SlotTaken)
				UE_LOG(LogTemp, Warning, TEXT("Weapon_SlotTaken"));
			if (MessageID == EHUDMessage::None)
				UE_LOG(LogTemp, Warning, TEXT("None"));
		}
	}
}


int32 AFTTP_GameState::GetTotalScore()
{
	return TotalScore;
}


void AFTTP_GameState::AddScore(int32 Score)
{
	TotalScore += Score;
}


void AFTTP_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFTTP_GameState, ElapsedGameMinutes);
	DOREPLIFETIME(AFTTP_GameState, bIsNight);
	DOREPLIFETIME(AFTTP_GameState, TotalScore);
}

