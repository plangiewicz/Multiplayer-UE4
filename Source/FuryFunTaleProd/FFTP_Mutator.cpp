// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FFTP_Mutator.h"


bool AFFTP_Mutator::CheckRelevance_Implementation(AActor* Other)
{
	if (NextMutator)
	{
		return NextMutator->CheckRelevance(Other);
	}

	return true;
}


void AFFTP_Mutator::InitGame_Implementation(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	if (NextMutator)
	{
		NextMutator->InitGame(MapName, Options, ErrorMessage);
	}
}

