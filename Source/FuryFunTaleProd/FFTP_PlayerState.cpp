// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FTTP_GameState.h"
#include "FFTP_PlayerState.h"




AFFTP_PlayerState::AFFTP_PlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* AI will remain in team 0, players are updated to team 1 through the GameMode::InitNewPlayer */
	TeamNumber = 0;
}


void AFFTP_PlayerState::Reset()
{
	Super::Reset();

	NumKills = 0;
	NumDeaths = 0;
	Score = 0;
}

void AFFTP_PlayerState::AddKill()
{
	NumKills++;
}

void AFFTP_PlayerState::AddDeath()
{
	NumDeaths++;
}

void AFFTP_PlayerState::ScorePoints(int32 Points)
{
	Score += Points;

	/* Add the score to the global score count */
	AFTTP_GameState* GS = GetWorld()->GetGameState<AFTTP_GameState>();
	if (GS)
	{
		GS->AddScore(Points);
	}
}


void AFFTP_PlayerState::SetTeamNumber(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
}


int32 AFFTP_PlayerState::GetTeamNumber() const
{
	return TeamNumber;
}

int32 AFFTP_PlayerState::GetKills() const
{
	return NumKills;
}

int32 AFFTP_PlayerState::GetDeaths() const
{
	return NumDeaths;
}


float AFFTP_PlayerState::GetScore() const
{
	return Score;
}


void AFFTP_PlayerState::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFFTP_PlayerState, NumKills);
	DOREPLIFETIME(AFFTP_PlayerState, NumDeaths);
	DOREPLIFETIME(AFFTP_PlayerState, TeamNumber);
}
