// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FFTP_UsableActor.h"


// Sets default values
AFFTP_UsableActor::AFFTP_UsableActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MeshComp = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	RootComponent = MeshComp;
	_widgetOffset = 10;
}

void AFFTP_UsableActor::OnBeginFocus()
{
	// Used by custom PostProcess to render outlines
	MeshComp->SetRenderCustomDepth(true);
	MeshComp->GetLocalBounds(MeshBottomPoint, MeshTopPoint);
}

void AFFTP_UsableActor::OnEndFocus()
{
	// Used by custom PostProcess to render outlines
	MeshComp->SetRenderCustomDepth(false);
}

void AFFTP_UsableActor::OnUsed(APawn* InstigatorPawn)
{

}

FString AFFTP_UsableActor::GetTextAbout()
{
	FString retString = Name;
	return retString;
}

FVector AFFTP_UsableActor::GetWidgetPosition()
{
	return FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + MeshTopPoint.Z * GetActorScale().Z + _widgetOffset);
}

