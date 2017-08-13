// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FFTP_UsableActor.generated.h"


UENUM(BlueprintType)
enum class EItemType : uint8
{
	NoType,
	Sword,
	Usable,
	Article
};

UCLASS()
class FURYFUNTALEPROD_API AFFTP_UsableActor : public AActor
{
	GENERATED_BODY()
	
	float _widgetOffset;

protected:

	AFFTP_UsableActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		UStaticMeshComponent* MeshComp;

public:

	FVector MeshBottomPoint;
	FVector MeshTopPoint;

	/* Player is looking at */
	virtual void OnBeginFocus();

	/* Player is no longer looking at */
	virtual void OnEndFocus();

	/* Called when player interacts with object */
	virtual void OnUsed(APawn* InstigatorPawn);

	/* Public accessor to the mesh component. With FORCEINLINE we are allowed to define the function in the header, use this only for simple accessors! */
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() const
	{
		return MeshComp;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
		EItemType ItemType;

	virtual FString GetTextAbout();
	FVector GetWidgetPosition();
};
