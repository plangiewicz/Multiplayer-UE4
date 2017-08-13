// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FFTP_GameMode.h"
#include "FTTP_GameState.h"
#include "FFTP_PlayerState.h"
#include "FFTP_BaseCharacter.h"
#include "FFTP_Character.h"


AFFTP_GameMode::AFFTP_GameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* Assign the class types used by this gamemode */
	PlayerControllerClass = AFFTP_PlayerController::StaticClass();
	PlayerStateClass = AFFTP_PlayerState::StaticClass();
	GameStateClass = AFTTP_GameState::StaticClass();
	//SpectatorClass = ASSpectatorPawn::StaticClass();

	bAllowFriendlyFireDamage = false;
	bSpawnZombiesAtNight = true;

	/* Start the game at 16:00 */
	TimeOfDayStart = 16 * 60;
	BotSpawnInterval = 5.0f;

	/* Default team is 1 for players and 0 for enemies */
	PlayerTeamNum = 1;
}


void AFFTP_GameMode::InitGameState()
{
	Super::InitGameState();

	AFTTP_GameState* MyGameState = Cast<AFTTP_GameState>(GameState);
	if (MyGameState)
	{
		MyGameState->ElapsedGameMinutes = TimeOfDayStart;
	}
}


void AFFTP_GameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	/* Set timer to run every second */
	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &AFFTP_GameMode::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}


void AFFTP_GameMode::StartMatch()
{
	if (!HasMatchStarted())
	{
		/* Spawn a new bot every 5 seconds (bothandler will opt-out based on his own rules for example to only spawn during night time) */
		//GetWorldTimerManager().SetTimer(TimerHandle_BotSpawns, this, &AFFTP_GameMode::SpawnBotHandler, BotSpawnInterval, true);
	}

	Super::StartMatch();
}


void AFFTP_GameMode::DefaultTimer()
{
	/* Immediately start the match while playing in editor */
	//if (GetWorld()->IsPlayInEditor())
	{
		if (GetMatchState() == MatchState::WaitingToStart)
		{
			StartMatch();
		}
	}

	/* Only increment time of day while game is active */
	if (IsMatchInProgress())
	{
		AFTTP_GameState* MyGameState = Cast<AFTTP_GameState>(GameState);
		if (MyGameState)
		{
			/* Increment our time of day */
			MyGameState->ElapsedGameMinutes += MyGameState->GetTimeOfDayIncrement();

			/* Determine our state */
			MyGameState->GetAndUpdateIsNight();

			/* Trigger events when night starts or ends */
			bool CurrentIsNight = MyGameState->GetIsNight();
			if (CurrentIsNight != LastIsNight)
			{
				EHUDMessage MessageID = CurrentIsNight ? EHUDMessage::Game_SurviveStart : EHUDMessage::Game_SurviveEnded;
				MyGameState->BroadcastGameMessage(MessageID);

				/* The night just ended, respawn all dead players */
				if (!CurrentIsNight)
				{
					OnNightEnded();
				}

				/* Update bot states */
				if (CurrentIsNight)
				{
					//WakeAllBots();
				}
				else
				{
					//PassifyAllBots();
				}
			}

			LastIsNight = MyGameState->bIsNight;
		}
	}
}


bool AFFTP_GameMode::CanDealDamage(class AFFTP_PlayerState* DamageCauser, class AFFTP_PlayerState* DamagedPlayer) const
{
	if (bAllowFriendlyFireDamage)
	{
		return true;
	}

	/* Allow damage to self */
	if (DamagedPlayer == DamageCauser)
	{
		return true;
	}

	// Compare Team Numbers
	return DamageCauser && DamagedPlayer && (DamageCauser->GetTeamNumber() != DamagedPlayer->GetTeamNumber());
}


FString AFFTP_GameMode::InitNewPlayer(class APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	AFFTP_PlayerState* NewPlayerState = Cast<AFFTP_PlayerState>(NewPlayerController->PlayerState);
	if (NewPlayerState)
	{
		NewPlayerState->SetTeamNumber(PlayerTeamNum);
	}

	return Result;
}


float AFFTP_GameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	float ActualDamage = Damage;

	AFFTP_BaseCharacter* DamagedPawn = Cast<AFFTP_BaseCharacter>(DamagedActor);
	if (DamagedPawn && EventInstigator)
	{
		AFFTP_PlayerState* DamagedPlayerState = Cast<AFFTP_PlayerState>(DamagedPawn->PlayerState);
		AFFTP_PlayerState* InstigatorPlayerState = Cast<AFFTP_PlayerState>(EventInstigator->PlayerState);

		// Check for friendly fire
		if (!CanDealDamage(InstigatorPlayerState, DamagedPlayerState))
		{
			ActualDamage = 0.f;
		}
	}

	return ActualDamage;
}


bool AFFTP_GameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	/* Always pick a random location */
	return false;
}


AActor* AFFTP_GameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	/* Get all playerstart objects in level */
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

	/* Split the player starts into two arrays for preferred and fallback spawns */
	for (int32 i = 0; i < PlayerStarts.Num(); i++)
	{
		APlayerStart* TestStart = Cast<APlayerStart>(PlayerStarts[i]);

		if (TestStart && IsSpawnpointAllowed(TestStart, Player))
		{
			if (IsSpawnpointPreferred(TestStart, Player))
			{
				PreferredSpawns.Add(TestStart);
			}
			else
			{
				FallbackSpawns.Add(TestStart);
			}
		}

	}

	/* Pick a random spawnpoint from the filtered spawn points */
	APlayerStart* BestStart = nullptr;
	if (PreferredSpawns.Num() > 0)
	{
		BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
	}
	else if (FallbackSpawns.Num() > 0)
	{
		BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
	}

	/* If we failed to find any (so BestStart is nullptr) fall back to the base code */
	return BestStart ? BestStart : Super::ChoosePlayerStart_Implementation(Player);
}


bool AFFTP_GameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Controller)
{
	if (Controller == nullptr || Controller->PlayerState == nullptr)
		return true;

	///* Check for extended playerstart class */
	//ASPlayerStart* MyPlayerStart = Cast<ASPlayerStart>(SpawnPoint);
	//if (MyPlayerStart)
	//{
	//	return MyPlayerStart->GetIsPlayerOnly() && !Controller->PlayerState->bIsABot;
	//}

	/* Cast failed, Anyone can spawn at the base playerstart class */
	return true;
}


bool AFFTP_GameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Controller)
{
	if (SpawnPoint)
	{
		/* Iterate all pawns to check for collision overlaps with the spawn point */
		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
		{
			ACharacter* OtherPawn = Cast<ACharacter>(*It);
			if (OtherPawn)
			{
				const float CombinedHeight = (SpawnPoint->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedWidth = SpawnPoint->GetCapsuleComponent()->GetScaledCapsuleRadius() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleRadius();
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// Check if player overlaps the playerstart
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedWidth)
				{
					return false;
				}
			}
		}

		///* Check if spawnpoint is exclusive to players */
		//ASPlayerStart* MyPlayerStart = Cast<ASPlayerStart>(SpawnPoint);
		//if (MyPlayerStart)
		//{
		//	return MyPlayerStart->GetIsPlayerOnly() && !Controller->PlayerState->bIsABot;
		//}
	}

	return false;
}


//void AFFTP_GameMode::SpawnNewBot()
//{
	//FActorSpawnParameters SpawnInfo;
	//SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	//ASZombieAIController* AIC = GetWorld()->SpawnActor<ASZombieAIController>(SpawnInfo);
	//RestartPlayer(AIC);
//}

/* Used by RestartPlayer() to determine the pawn to create and possess when a bot or player spawns */
UClass* AFFTP_GameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	//if (Cast<ASZombieAIController>(InController))
	//{
	//	return BotPawnClass;
	//}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}


bool AFFTP_GameMode::CanSpectate_Implementation(APlayerController* Viewer, APlayerState* ViewTarget)
{
	/* Don't allow spectating of other non-player bots */
	return (ViewTarget && !ViewTarget->bIsABot);
}


//void AFFTP_GameMode::PassifyAllBots()
//{
//	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
//	{
//		ASZombieCharacter* AIPawn = Cast<ASZombieCharacter>(*It);
//		if (AIPawn)
//		{
//			AIPawn->SetBotType(EBotBehaviorType::Passive);
//		}
//	}
//}


//void AFFTP_GameMode::WakeAllBots()
//{
//	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
//	{
//		ASZombieCharacter* AIPawn = Cast<ASZombieCharacter>(*It);
//		if (AIPawn)
//		{
//			AIPawn->SetBotType(EBotBehaviorType::Patrolling);
//		}
//	}
//}

//
//void AFFTP_GameMode::SpawnBotHandler()
//{
//	if (!bSpawnZombiesAtNight)
//		return;
//
//	ASGameState* MyGameState = Cast<ASGameState>(GameState);
//	if (MyGameState)
//	{
//		/* Only spawn bots during night time */
//		if (MyGameState->GetIsNight())
//		{
//			/* This could be any dynamic number based on difficulty (eg. increasing after having survived a few nights) */
//			const int32 MaxPawns = 10;
//
//			/* Check number of available pawns (players included) */
//			if (GetWorld()->GetNumPawns() < MaxPawns)
//			{
//				SpawnNewBot();
//			}
//		}
//	}
//}


void AFFTP_GameMode::OnNightEnded()
{
	// Do nothing (can be used to apply score or trigger other time of day events)
}

void AFFTP_GameMode::Killed(AController* Killer, AController* VictimPlayer, APawn* VictimPawn, const UDamageType* DamageType)
{
	// Do nothing (can we used to apply score or keep track of kill count)
}


void AFFTP_GameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	SpawnDefaultInventory(PlayerPawn);
}


void AFFTP_GameMode::SpawnDefaultInventory(APawn* PlayerPawn)
{
	AFFTP_Character* MyPawn = Cast<AFFTP_Character>(PlayerPawn);
	if (MyPawn)
	{
		//for (int32 i = 0; i < DefaultInventoryClasses.Num(); i++)
		//{
		//	if (DefaultInventoryClasses[i])
		//	{
		//		//FActorSpawnParameters SpawnInfo;
		//		//SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//		//ASWeapon* NewWeapon = GetWorld()->SpawnActor<ASWeapon>(DefaultInventoryClasses[i], SpawnInfo);

		//		//MyPawn->AddWeapon(NewWeapon);
		//	}
		//}
	}
}


/************************************************************************/
/* Modding & Mutators                                                   */
/************************************************************************/


void AFFTP_GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	// HACK: workaround to inject CheckRelevance() into the BeginPlay sequence
	UFunction* Func = AActor::GetClass()->FindFunctionByName(FName(TEXT("ReceiveBeginPlay")));
	Func->FunctionFlags |= FUNC_Native;
	Func->SetNativeFunc((Native)&AFFTP_GameMode::BeginPlayMutatorHack);

	/* Spawn all mutators. */
	for (int32 i = 0; i < MutatorClasses.Num(); i++)
	{
		AddMutator(MutatorClasses[i]);
	}

	if (BaseMutator)
	{
		BaseMutator->InitGame(MapName, Options, ErrorMessage);
	}

	Super::InitGame(MapName, Options, ErrorMessage);
}


bool AFFTP_GameMode::CheckRelevance_Implementation(AActor* Other)
{
	/* Execute the first in the mutator chain */
	if (BaseMutator)
	{
		return BaseMutator->CheckRelevance(Other);
	}

	return true;
}


void AFFTP_GameMode::BeginPlayMutatorHack(FFrame& Stack, RESULT_DECL)
{
	P_FINISH;

	// WARNING: This function is called by every Actor in the level during his BeginPlay sequence. Meaning:  'this' is actually an AActor! Only do AActor things!
	if (!IsA(ALevelScriptActor::StaticClass()) && !IsA(AFFTP_Mutator::StaticClass()) &&
		(RootComponent == NULL || RootComponent->Mobility != EComponentMobility::Static || (!IsA(AStaticMeshActor::StaticClass()) && !IsA(ALight::StaticClass()))))
	{
		AFFTP_GameMode* Game = GetWorld()->GetAuthGameMode<AFFTP_GameMode>();
		// a few type checks being AFTER the CheckRelevance() call is intentional; want mutators to be able to modify, but not outright destroy
		if (Game != NULL && Game != this && !Game->CheckRelevance((AActor*)this) && !IsA(APlayerController::StaticClass()))
		{
			/* Actors are destroyed if they fail the relevance checks (which moves through the gamemode specific check AND the chain of mutators) */
			Destroy();
		}
	}
}


void AFFTP_GameMode::AddMutator(TSubclassOf<AFFTP_Mutator> MutClass)
{
	AFFTP_Mutator* NewMut = GetWorld()->SpawnActor<AFFTP_Mutator>(MutClass);
	if (NewMut)
	{
		if (BaseMutator == nullptr)
		{
			BaseMutator = NewMut;
		}
		else
		{
			// Add as child in chain
			BaseMutator->NextMutator = NewMut;
		}
	}
}
