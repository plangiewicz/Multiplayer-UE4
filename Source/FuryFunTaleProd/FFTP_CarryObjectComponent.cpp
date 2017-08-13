// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FFTP_CarryObjectComponent.h"


UFFTP_CarryObjectComponent::UFFTP_CarryObjectComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxPickupDistance = 600;
	RotateSpeed = 10.0f;

	bUsePawnControlRotation = true;
	bDoCollisionTest = false;

	SetIsReplicated(true);
}


void UFFTP_CarryObjectComponent::TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
	{
		if (OwningPawn->IsLocallyControlled())
		{
			Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);
		}
		else
		{
			/* NOTE: Slightly changed code from base implementation (USpringArmComponent) to use RemoteViewPitch instead of non-replicated ControlRotation */
			if (bUsePawnControlRotation)
			{
				{
					/* Re-map uint8 to 360 degrees */
					const float PawnViewPitch = (OwningPawn->RemoteViewPitch / 255.f)*360.f;
					if (PawnViewPitch != GetComponentRotation().Pitch)
					{
						FRotator NewRotation = GetComponentRotation();
						NewRotation.Pitch = PawnViewPitch;
						SetWorldRotation(NewRotation);
					}
				}
			}

			UpdateDesiredArmLocation(bDoCollisionTest, bEnableCameraLag, bEnableCameraRotationLag, DeltaSeconds);
		}
	}
}


void UFFTP_CarryObjectComponent::Pickup()
{
	/* Drop if we are already carrying an Actor */
	if (GetIsCarryingActor())
	{
		Drop();
		return;
	}

	if (GetOwner()->Role < ROLE_Authority)
	{
		ServerPickup();
		return;
	}

	AActor* FocusActor = GetActorInView();
	OnPickupMulticast(FocusActor);
}


void UFFTP_CarryObjectComponent::Drop()
{
	if (GetOwner()->Role < ROLE_Authority)
	{
		ServerDrop();
	}

	OnDropMulticast();
}


AActor* UFFTP_CarryObjectComponent::GetActorInView()
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	AController* Controller = PawnOwner->Controller;
	if (Controller == nullptr)
	{
		return nullptr;
	}

	FVector CamLoc;
	FRotator CamRot;
	Controller->GetPlayerViewPoint(CamLoc, CamRot);

	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * MaxPickupDistance);

	FCollisionQueryParams TraceParams(TEXT("TraceActor"), true, PawnOwner);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = false;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

	/* Check to see if we hit a staticmesh component that has physics simulation enabled */
	UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Hit.GetComponent());
	if (MeshComp && MeshComp->IsSimulatingPhysics())
	{
		return Hit.GetActor();
	}

	return nullptr;
}


AActor* UFFTP_CarryObjectComponent::GetCarriedActor()
{
	USceneComponent* ChildComp = GetChildComponent(0);
	if (ChildComp)
	{
		return ChildComp->GetOwner();
	}

	return nullptr;
}


UStaticMeshComponent* UFFTP_CarryObjectComponent::GetCarriedMeshComp()
{
	USceneComponent* ChildComp = GetChildComponent(0);
	if (ChildComp)
	{
		AActor* OwningActor = ChildComp->GetOwner();
		if (OwningActor)
		{
			return Cast<UStaticMeshComponent>(OwningActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		}
	}

	return nullptr;
}


void UFFTP_CarryObjectComponent::Throw()
{
	if (!GetIsCarryingActor())
		return;

	if (GetOwner()->Role < ROLE_Authority)
	{
		ServerThrow();
		return;
	}

	/* Grab a reference to the MeshComp before dropping the object */
	UStaticMeshComponent* MeshComp = GetCarriedMeshComp();
	if (MeshComp)
	{
		/* Detach and re-enable collision */
		OnDropMulticast();

		APawn* OwningPawn = Cast<APawn>(GetOwner());
		if (OwningPawn)
		{
			/* Re-map uint8 to 360 degrees */
			const float PawnViewPitch = (OwningPawn->RemoteViewPitch / 255.f)*360.f;

			FRotator NewRotation = GetComponentRotation();
			NewRotation.Pitch = PawnViewPitch;

			/* Apply physics impulse, ignores mass */
			MeshComp->AddImpulse(NewRotation.Vector() * 1000, NAME_None, true);
		}
	}
}


bool UFFTP_CarryObjectComponent::GetIsCarryingActor()
{
	return GetChildComponent(0) != nullptr;
}


void UFFTP_CarryObjectComponent::Rotate(float DirectionYaw, float DirectionRoll)
{
	if (GetOwner()->Role < ROLE_Authority)
	{
		ServerRotate(DirectionYaw, DirectionRoll);
	}

	OnRotateMulticast(DirectionYaw, DirectionRoll);
}


void UFFTP_CarryObjectComponent::RotateActorAroundPoint(AActor* RotateActor, FVector RotationPoint, FRotator AddRotation)
{
	FVector Loc = RotateActor->GetActorLocation() - RotationPoint;
	FVector RotatedLoc = AddRotation.RotateVector(Loc);

	FVector NewLoc = RotationPoint + RotatedLoc;

	/* Compose the rotators, use Quats to avoid gimbal lock */
	FQuat AQuat = FQuat(RotateActor->GetActorRotation());
	FQuat BQuat = FQuat(AddRotation);

	FRotator NewRot = FRotator(BQuat*AQuat);

	RotateActor->SetActorLocationAndRotation(NewLoc, NewRot);
}


void UFFTP_CarryObjectComponent::OnPickupMulticast_Implementation(AActor* FocusActor)
{
	if (FocusActor && FocusActor->IsRootComponentMovable())
	{
		/* Find the static mesh (if any) to disable physics simulation while carried
		Filter by objects that are physically simulated and can therefor be picked up */
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(FocusActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (MeshComp && MeshComp->IsSimulatingPhysics())
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			MeshComp->SetSimulatePhysics(false);
		}

		FocusActor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	}
}


void UFFTP_CarryObjectComponent::OnDropMulticast_Implementation()
{
	AActor* CarriedActor = GetCarriedActor();
	if (CarriedActor)
	{
		/* Find the static mesh (if any) to re-enable physics simulation */
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(CarriedActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (MeshComp)
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComp->SetSimulatePhysics(true);
		}

		CarriedActor->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
}


void UFFTP_CarryObjectComponent::OnRotateMulticast_Implementation(float DirectionYaw, float DirectionRoll)
{
	AActor* CarriedActor = GetCarriedActor();
	if (CarriedActor)
	{
		/* Retrieve the object center */
		FVector RootOrigin = GetCarriedActor()->GetRootComponent()->Bounds.Origin;
		FRotator DeltaRot = FRotator(0, DirectionYaw * RotateSpeed, DirectionRoll * RotateSpeed);

		RotateActorAroundPoint(CarriedActor, RootOrigin, DeltaRot);
	}
}


void UFFTP_CarryObjectComponent::ServerDrop_Implementation()
{
	Drop();
}


bool UFFTP_CarryObjectComponent::ServerDrop_Validate()
{
	return true;
}


void UFFTP_CarryObjectComponent::ServerThrow_Implementation()
{
	Throw();
}


bool UFFTP_CarryObjectComponent::ServerThrow_Validate()
{
	return true;
}


void UFFTP_CarryObjectComponent::ServerPickup_Implementation()
{
	Pickup();
}


bool UFFTP_CarryObjectComponent::ServerPickup_Validate()
{
	return true;
}


void UFFTP_CarryObjectComponent::ServerRotate_Implementation(float DirectionYaw, float DirectionRoll)
{
	Rotate(DirectionYaw, DirectionRoll);
}


bool UFFTP_CarryObjectComponent::ServerRotate_Validate(float DirectionYaw, float DirectionRoll)
{
	return true;
}


