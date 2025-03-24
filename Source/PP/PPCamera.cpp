// Fill out your copyright notice in the Description page of Project Settings.


#include "PPCamera.h"

#include "EnhancedInputComponent.h"

#include "InputActionValue.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

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
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("FloatingPawnMovement");
	FloatingPawnMovement->MaxSpeed = 2000.f;
	FloatingPawnMovement->Acceleration = 2000.f;
	FloatingPawnMovement->Deceleration = 2000.f;
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
	FloatingPawnMovement->MaxSpeed = DirectionalMovementSpeed * GlobalSpeedMultiplier * ZoomSpeedMultiplierCurve->GetFloatValue(GetZoom());
	FloatingPawnMovement->MaxSpeed *= (1 / UGameplayStatics::GetGlobalTimeDilation(GetWorld()));
	AddMovementInput(GetActorForwardVector(), InputVector.Y);
	AddMovementInput(GetActorRightVector(), InputVector.X);
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
	SetActorRotation(DefaultRotation);
}

void APPCamera::ResetZoomToDefault()
{
	ZoomTarget = DefaultZoom;
}

void APPCamera::TeleportToLocation(FVector Location)
{
	const FVector Offset = GetActorUpVector() * GroundAdjustTraceLength;
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	if(UKismetSystemLibrary::LineTraceSingle(this, Location + Offset, Location - Offset, GroundAdjustTraceChannel, false, ActorsToIgnore,EDrawDebugTrace::None, HitResult,true))
	{
		
	}
}

void APPCamera::DigitalRotateCamera(float Input)
{
	float InputValue = Input * DigitalRotationDirection.GetSign() * DigitalRotationSpeed;
	InputValue *= GetWorld()->DeltaRealTimeSeconds;
	AddActorWorldRotation(FRotator::MakeFromEuler(FVector(0, 0, InputValue)));
}

void APPCamera::ZoomCamera(float Input)
{
	const float InputValue = -1 * Input * ZoomDirection.GetSign() * ZoomSpeed;
	ZoomTarget = FMath::Clamp(ZoomTarget + (InputValue * GetWorld()->DeltaRealTimeSeconds), ZoomRange.X, ZoomRange.Y);
}

float APPCamera::GetZoom() const
{
	return SpringArm->TargetArmLength;
}

float APPCamera::GetZoomFraction() const
{
	return FMath::GetMappedRangeValueClamped(ZoomRange, FVector2D(0, 1), GetZoom());
}

void APPCamera::SetDefaultRotation(float ZRotation)
{
	DefaultRotation = FRotator(0, ZRotation, 0);
}

void APPCamera::SetEdgeScrollEnabled(bool bEnabled)
{
	bEdgeScrollEnabled = bEnabled;
}

void APPCamera::SetDirectMouseControlMode(bool bEnabled)
{
	DirectMouseControlActive = bEnabled;
	
}

void APPCamera::MovementInputTriggered(const FInputActionValue& ActionValue)
{
	StopFocus();
	AddCameraMovementInput(ActionValue.Get<FVector2D>() * MovementDirectionData.GetSigns() * DirectionalMovementSpeed);
}

void APPCamera::DigitalRotateInputTriggered(const FInputActionValue& ActionValue)
{
	DigitalRotateCamera(ActionValue.Get<float>());
}

void APPCamera::MouseRotateSwitchStarted()
{
	SetDirectMouseControlMode(true);
}

void APPCamera::MouseRotateSwitchCompleted()
{
	SetDirectMouseControlMode(false);
	MoveCursorOutOfEdge();
}

void APPCamera::ZoomInputTriggered(const FInputActionValue& ActionValue)
{
	float Input = ActionValue.Get<float>();
	ZoomCamera(Input);
}

void APPCamera::MoveCursorOutOfEdge()
{
	FVector2D CursorPosition;
	GetController<APlayerController>()->GetMousePosition(CursorPosition.X, CursorPosition.Y);
	FInt32Vector2 ViewportSize;
	GetController<APlayerController>()->GetViewportSize(ViewportSize.X, ViewportSize.Y);
	const float EdgeSize = ViewportSize.X * (EdgeScrollVerticalScreenSpacePercent / 100) + 10;

	CursorPosition.X = FMath::Clamp(CursorPosition.X, EdgeSize, ViewportSize.X - EdgeSize);
	CursorPosition.Y = FMath::Clamp(CursorPosition.Y, EdgeSize, ViewportSize.Y - EdgeSize);
	GetController<APlayerController>()->SetMouseLocation(CursorPosition.X, CursorPosition.Y);
}

FVector2D APPCamera::CalculateEdgeScrollVector()
{
	//const FVector2D EdgeScrollVector = CalculateEdgeScrollVector();
	//StopFocus();
	//AddCameraMovementInput(EdgeScrollVector);

	
	const FVector2D CursorPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);

	const float EdgeThreshold = ViewportSize.X * EdgeScrollVerticalScreenSpacePercent;
	FVector2D ScrollDirection = FVector2D::Zero();

	// Horizontal edges
	if (CursorPosition.X < EdgeThreshold)
		ScrollDirection.Y = -1.0f; // Left
	else if (CursorPosition.X > ViewportSize.X - EdgeThreshold)
		ScrollDirection.Y = 1.0f;  // Right

	// Vertical edges
	if (CursorPosition.Y < EdgeThreshold)
		ScrollDirection.X = 1.0f;  // Forward
	else if (CursorPosition.Y > ViewportSize.Y - EdgeThreshold)
		ScrollDirection.X = -1.0f; // Backward

	//return FVector2D::Zero();
	return ScrollDirection * EdgeScrollSpeed;
}

// Called every frame
void APPCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Smooth zoom interpolation
	if (!FMath::IsNearlyEqual(SpringArm->TargetArmLength, ZoomTarget, 1.0f))
	{
		SpringArm->TargetArmLength = FMath::FInterpTo(
			SpringArm->TargetArmLength,
			ZoomTarget,
			DeltaTime,
			ZoomInterpSpeed
		);
	}
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


	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::MovementInputTriggered);
		EnhancedInputComponent->BindAction(DigitalRotateAction, ETriggerEvent::Triggered, this, &ThisClass::DigitalRotateInputTriggered);
		EnhancedInputComponent->BindAction(MouseRotateSwitchAction, ETriggerEvent::Started, this, &ThisClass::MouseRotateSwitchStarted);
		EnhancedInputComponent->BindAction(MouseRotateSwitchAction, ETriggerEvent::Completed, this, &ThisClass::MouseRotateSwitchCompleted);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ThisClass::ZoomInputTriggered);
		//Anything else?
	}
}