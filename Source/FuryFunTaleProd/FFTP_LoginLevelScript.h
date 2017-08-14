// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "FFTP_PlayerController.h"
#include "FFTP_LoginLevelScript.generated.h"

/**
 * 
 */
UCLASS()
class FURYFUNTALEPROD_API AFFTP_LoginLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()
public:
	AFFTP_LoginLevelScript();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	AFFTP_PlayerController* _controller;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InGameHUD)
		bool bInGame;
};
