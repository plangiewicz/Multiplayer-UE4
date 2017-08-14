// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FFTP_PlayerHUD.h"
#include "FFTP_Character.h"
#include "FFTP_PlayerController.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"


AFFTP_PlayerHUD::AFFTP_PlayerHUD(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* You can use the FObjectFinder in C++ to reference content directly in code. Although it's advisable to avoid this and instead assign content through Blueprint child classes. */
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDCenterDotObj(TEXT("/Game/UI/HUD/centerdot"));
	CenterDotIcon = UCanvas::MakeIcon(HUDCenterDotObj.Object);

	//AccessToPlay(true);

}
//
//void AFFTP_PlayerHUD::BeginPlay()
//{
//	Super::BeginPlay(); 
//	APlayerController* controller = GetOwningPlayerController();
//	if (controller)
//	{
//		static ConstructorHelpers::FObjectFinder<UClass> WidgetHUDLayer(TEXT("/Game/UI/HUD/Widged_HUDLayout"));
//		if(WidgetHUDLayer.Succeeded())
//		{
//			//CreateWidget(controller, WidgetHUDLayer.Object->GetClass());
//			MyMainMenu = CreateWidget<UUserWidget>(controller, WidgetHUDLayer);
//		}
//	}
//
//}

void AFFTP_PlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawCenterDot();
}


void AFFTP_PlayerHUD::AccessToPlay(bool access)
{
	bInGame = access;
}

void AFFTP_PlayerHUD::DrawCenterDot()
{
	float CenterX = Canvas->ClipX / 2;
	float CenterY = Canvas->ClipY / 2;
	float CenterDotScale = 0.07f;

	AFFTP_Character* Pawn = Cast<AFFTP_Character>(GetOwningPawn());
	if (Pawn/* && Pawn->IsAlive()*/ && bInGame)
		//TODO: IsAlive()
	{
		// Boost size when hovering over a usable object.
		AFFTP_UsableActor* Usable = Pawn->GetUsableInView();
		if (Usable)
		{
			CenterDotScale *= 1.5f;
		}

		Canvas->SetDrawColor(255, 255, 255, 255);
		Canvas->DrawIcon(CenterDotIcon,
			CenterX - CenterDotIcon.UL*CenterDotScale / 2.0f,
			CenterY - CenterDotIcon.VL*CenterDotScale / 2.0f, CenterDotScale);
	}
}

void AFFTP_PlayerHUD::OnStateChanged_Implementation(EHUDState NewState)
{
	CurrentState = NewState;
}

EHUDState AFFTP_PlayerHUD::GetCurrentState()
{
	return CurrentState;
}