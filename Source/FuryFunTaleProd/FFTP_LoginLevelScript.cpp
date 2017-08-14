// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FFTP_LoginLevelScript.h"


AFFTP_LoginLevelScript::AFFTP_LoginLevelScript()
{
	_controller = nullptr;
	bInGame = false;
}

void AFFTP_LoginLevelScript::BeginPlay()
{
	Super::BeginPlay();
	_controller = Cast<AFFTP_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void AFFTP_LoginLevelScript::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(_controller && bInGame)
	{
		bInGame = _controller->LoginTry("", "");
	}
}