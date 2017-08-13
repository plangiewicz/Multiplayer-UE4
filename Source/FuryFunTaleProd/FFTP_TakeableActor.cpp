// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FFTP_TakeableActor.h"
#include "FFTP_Character.h"


AFFTP_TakeableActor::AFFTP_TakeableActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* Ignore Pawn - this is to prevent objects shooting through the level or pawns glitching on top of small items. */
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);


	bIsActive = false;
	bStartActive = true;
	bAllowRespawn = false;
	RespawnDelay = 5.0f;
	RespawnDelayRange = 5.0f;

	SetReplicates(true);

	bReplicateMovement = true;
}

void AFFTP_TakeableActor::BeginPlay()
{
	Super::BeginPlay();
	RespawnActor();
}

void AFFTP_TakeableActor::OnUsed(APawn* InstigatorPawn)
{
	Super::OnUsed(InstigatorPawn);

	//UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());

	bIsActive = false;
	OnPickedUp();

	if (bAllowRespawn)
	{
		FTimerHandle RespawnTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &AFFTP_TakeableActor::RespawnActor, RespawnDelay + FMath::RandHelper(RespawnDelayRange), false);
	}
	else
	{
		/* Delete from level if respawn is not allowed */
		Destroy();
	}
}

FString AFFTP_TakeableActor::GetTextAbout()
{
	Super::GetTextAbout();

	FString retString = "";
	switch (ItemType)
	{
	case EItemType::NoType:
		retString = Name;
		break;
	case EItemType::Sword:
		retString = "[sword] " + Name;
		break;
	case EItemType::Usable:
		retString = "[usable] " + Name;
		break;
	case EItemType::Article:
		retString = "[article] " + Name;
		break;
	default:
		retString = "[error type] " + Name;
	}

	return retString;

}

void AFFTP_TakeableActor::OnPickedUp()
{
	if (MeshComp)
	{
		MeshComp->SetVisibility(false);
		MeshComp->SetSimulatePhysics(false);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AFFTP_TakeableActor::RespawnActor()
{
	bIsActive = true;
	OnRespawned();
}


void AFFTP_TakeableActor::OnRespawned()
{
	if (MeshComp)
	{
		MeshComp->SetVisibility(true);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}


void AFFTP_TakeableActor::OnRep_IsActive()
{
	if (bIsActive)
	{
		OnRespawned();
	}
	else
	{
		OnPickedUp();
	}
}


void AFFTP_TakeableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFFTP_TakeableActor, bIsActive);
}