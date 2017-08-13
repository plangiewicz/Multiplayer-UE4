// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FFTP_UsableActor.h"
#include "FFTP_TakeableActor.generated.h"





class AFFTP_Character;
/**
 * 
 */
UCLASS()
class FURYFUNTALEPROD_API AFFTP_TakeableActor : public AFFTP_UsableActor
{
	GENERATED_BODY()

	void BeginPlay() override;

	UFUNCTION()
		void OnRep_IsActive();

protected:
	AFFTP_TakeableActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsActive)
		bool bIsActive;

	virtual void RespawnActor();

	virtual void OnPickedUp();

	virtual void OnRespawned();

public:

	virtual void OnUsed(APawn* InstigatorPawn) override;
	virtual FString GetTextAbout() override;

	/* Immediately spawn on begin play */
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
		bool bStartActive;

	/* Will this item ever respawn */
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
		bool bAllowRespawn;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
		float RespawnDelay;

	/* Extra delay randomly applied on the respawn timer */
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
		float RespawnDelayRange;
	
	
};
