// Fill out your copyright notice in the Description page of Project Settings.


#include "PPCamera.h"

#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"

APPCamera::APPCamera()
{

	PrimaryActorTick.bCanEverTick = false;

	//RootCollision = CreateDefaultSubobject<USphereComponent>("RootCollision");
	//SetRootComponent(RootCollision);

	TransformTarget = CreateDefaultSubobject<USceneComponent>("TransformTarget");
	TransformTarget->SetupAttachment(RootCollision);

	ArmRoot = CreateDefaultSubobject<USceneComponent>("ArmRoot");
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(ArmRoot);
	SpringArm->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0, DefaultCameraArmAngle, 0)));
	SpringArm->TargetArmLength = DefaultZoom;
	SpringArm->bDoCollisionTest = false;
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
	Camera->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0, DefaultCameraAngle, 0)));
	Camera->FieldOfView = Fov;
}


void APPCamera::BeginPlay()
{
	Super::BeginPlay();
	//if (USaveGameSubsystem::Get(this) && !USaveGameSubsystem::Get(this)->IsLoadingGame())
	//{
	//	DefaultRotation = GetActorRotation();
	//}

}

void APPCamera::AddCameraMovementInput(FVector2D InputVector)
{
}

void APPCamera::FocusOnActor(AActor* FocusedActor, bool bInstant)
{	StopFocus();
	if (bInstant)
	{
		//SetCameraLocationDirectly(FocusedActor->GetActorLocation());
		return;
	}
	ActorToFocus = FocusedActor;
	bFocusing = true;
}

void APPCamera::FocusOnLocation(FVector FocusedLocation, bool bInstant)
{
	StopFocus();
	if (bInstant)
	{
		//SetCameraLocationDirectly(FocusedLocation);
		return;
	}
	LocationToFocus = FocusedLocation;
	bFocusing = true;
}

void APPCamera::StopFocus()
{
	bFocusing = false;
	bAttachToFocusedActor = false;
	ActorToFocus = nullptr;
	if (AttachActor)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		AttachActor = nullptr;
	}
}

void APPCamera::AddExternalMovementInput(FVector2D InputVector)
{
	AddCameraMovementInput(InputVector);
}

void APPCamera::ResetRotationToDefault()
{
}

void APPCamera::ResetZoomToDefault()
{
}

void APPCamera::TeleportToLocation(FVector Location)
{
}

void APPCamera::DigitalRotateCamera(float Input)
{
}

void APPCamera::ZoomCamera(float Input)
{
}

float APPCamera::GetZoom() const
{
}

float APPCamera::GetZoomFraction() const
{
}

void APPCamera::SetDirectMouseControlMode(bool bEnabled)
{
}

void APPCamera::MovementInputTriggered(const FInputActionValue& ActionValue)
{
}

void APPCamera::DigitalRotateInputTriggered(const FInputActionValue& ActionValue)
{
}

void APPCamera::MouseRotateSwitchStarted()
{
}

void APPCamera::MouseRotateSwitchCompleted()
{
}

void APPCamera::ZoomInputTriggered(const FInputActionValue& ActionValue)
{
}

void APPCamera::MoveCursorOutOfEdge()
{
}

FVector2D APPCamera::CalculateEdgeScrollVector()
{
	const FVector2D EdgeScrollVector = CalculateEdgeScrollVector();

	StopFocus();
	AddCameraMovementInput(EdgeScrollVector);
}

// Called every frame
void APPCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APPCamera::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (!Ar.IsSaveGame())
	{
		return;
	}
	FTransform Transform = GetActorTransform();
	Ar << Transform;

	Ar << ZoomTarget;
	Ar << DefaultRotation;
	Ar << SpringArm->TargetArmLength;
}

// Called to bind functionality to input
void APPCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}