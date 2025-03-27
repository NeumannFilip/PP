// Fill out your copyright notice in the Description page of Project Settings.


#include "PPCamera.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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

	RootCollision = CreateDefaultSubobject<USphereComponent>("RootCollision");
	SetRootComponent(RootCollision);

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
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			//Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	DefaultRotation = GetActorRotation();
	ZoomTarget = SpringArm->TargetArmLength;
}

void APPCamera::AddCameraMovementInput(FVector2D InputVector)
{
	FloatingPawnMovement->MaxSpeed = DirectionalMovementSpeed * GlobalSpeedMultiplier * ZoomSpeedMultiplierCurve->GetFloatValue(GetZoom());
	FloatingPawnMovement->MaxSpeed *= (1 / UGameplayStatics::GetGlobalTimeDilation(GetWorld()));
	AddMovementInput(GetActorForwardVector(), InputVector.Y);
	AddMovementInput(GetActorRightVector(), InputVector.X);
}

void APPCamera::InterpolateToFocusLocation(float DeltaTime)
{
	if (ActorToFocus)
		LocationToFocus = ActorToFocus->GetActorLocation();
	if (FVector2D::DistSquared(FVector2D(GetActorLocation().X, GetActorLocation().Y), FVector2D(LocationToFocus.X, LocationToFocus.Y)) < 10)
	{
		if (bAttachToFocusedActor && ActorToFocus)
		{
			AttachActor = ActorToFocus;
			RootComponent->SetUsingAbsoluteRotation(true);
			AttachToActor(AttachActor, FAttachmentTransformRules::KeepWorldTransform);
		}
		bFocusing = false;
		bAttachToFocusedActor = false;
		ActorToFocus = nullptr;
		OnFocusTargetReached.Broadcast();
		return;
	}
}

void APPCamera::CalculateAndAddEdgeScrollInput()
{
	const FVector2D EdgeScrollVector = CalculateEdgeScrollVector();
	if (EdgeScrollVector.Length() < 0.01)
		return;

	StopFocus();
	AddCameraMovementInput(EdgeScrollVector);
}

void APPCamera::InterpolateToTransformAnchorLocation(float DeltaTime)
{
	if (FVector2D::DistSquared(FVector2D(ArmRoot->GetComponentLocation().X, ArmRoot->GetComponentLocation().Y),
							   FVector2D(TransformTarget->GetComponentLocation().X, TransformTarget->GetComponentLocation().Y)) > 1)
	{
		FVector NewLoc = FMath::VInterpTo(ArmRoot->GetComponentLocation(), TransformTarget->GetComponentLocation(), DeltaTime, TransformAnchorInterpSpeed);
		NewLoc.Z = ArmRoot->GetComponentLocation().Z;
		ArmRoot->SetWorldLocation(NewLoc);
	}
}

void APPCamera::InterpolateToDesiredCameraPitch(float DeltaTime)
{
	if (DirectMouseControlActive)
	{
		if (SpringArm->GetRelativeRotation().Pitch > (CameraPitchClamp.X - PitchDampeningMargin) || SpringArm->GetRelativeRotation().Pitch < (CameraPitchClamp.Y + PitchDampeningMargin))
		{
			if (FMath::Abs(SpringArm->GetRelativeRotation().Pitch - CameraPitchClamp.X) < FMath::Abs(SpringArm->GetRelativeRotation().Pitch - CameraPitchClamp.Y))
				SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), FRotator::MakeFromEuler(FVector(0, CameraPitchClamp.X - PitchDampeningMargin, 0)), DeltaTime, 20));
			else
				SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), FRotator::MakeFromEuler(FVector(0, CameraPitchClamp.Y + PitchDampeningMargin, 0)), DeltaTime, 20));
		}
	}
	else if (!FMath::IsNearlyEqual(SpringArm->GetRelativeRotation().Pitch, GetDesiredCameraPitch(), CameraPitchFreeAngle))
		SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), FRotator::MakeFromEuler(FVector(0, GetDesiredCameraPitch(), 0)), DeltaTime, CameraPitchInterpSpeed));
}

void APPCamera::InterpolateToTransformAnchorRotation(float DeltaTime)
{
	if (ArmRoot->GetComponentRotation().GetManhattanDistance(TransformTarget->GetComponentRotation()) > 1)
		ArmRoot->SetWorldRotation(FMath::RInterpTo(ArmRoot->GetComponentRotation(), TransformTarget->GetComponentRotation(), DeltaTime, TransformAnchorInterpSpeed));
}

float APPCamera::GetDesiredCameraPitch() const
{
	const float CurrentZoomFraction = GetZoomFraction();
	if (CurrentZoomFraction < DefaultCameraPitchZoomRange.X)
		return DefaultCameraArmAngle;
	if (CurrentZoomFraction > DefaultCameraPitchZoomRange.Y)
		return -88;

	return FMath::GetMappedRangeValueClamped(DefaultCameraPitchZoomRange, FVector2D(DefaultCameraArmAngle, -88), CurrentZoomFraction);
}

void APPCamera::InterpolateToZoomTarget(float DeltaTime)
{
	if (FMath::Abs(ZoomTarget - GetZoom()) > 1)
	{
		SpringArm->TargetArmLength = FMath::FInterpTo(GetZoom(), ZoomTarget, DeltaTime, ZoomInterpSpeed);
	}
}

void APPCamera::AddCameraOffsetDirectly(FVector Offset)
{
	FHitResult HitResult = FHitResult();
	FVector LastLocation = GetActorLocation();
	AddActorWorldOffset(Offset, true, &HitResult);
	if (HitResult.bBlockingHit)
	{
		SetActorLocation(LastLocation);
		GetMovementComponent()->SlideAlongSurface(Offset, 1, HitResult.Normal, HitResult, false);;
	}
	ArmRoot->SetWorldLocation(GetActorLocation());
}

const FVector2D& APPCamera::GetZoomRange() const
{
	return ZoomRange;
}

void APPCamera::FocusOnActor(AActor* FocusedActor, bool bInstant)
{
	StopFocus();
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

void APPCamera::PanSwitchStarted()
{
	SetDirectMouseControlMode(true);
}

void APPCamera::PanSwitchCompleted()
{
	SetDirectMouseControlMode(false);
	MoveCursorOutOfEdge();
}

void APPCamera::PanInputTriggered(const FInputActionValue& ActionValue)
{
	StopFocus();
	const FVector2D Input = -1 * ActionValue.Get<FVector2D>() * MousePanDirectionData.GetSigns() * MousePanSpeed;
	const FVector Offset = Input.Y * GetActorForwardVector() + Input.X * GetActorRightVector();
	AddCameraOffsetDirectly(Offset);
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
	/*const FVector2D CursorPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
	const float EdgeThreshold = ViewportSize.X * EdgeScrollVerticalScreenSpacePercent;

	FVector2D ScrollDirection = FVector2D::Zero();

	// Horizontal edges (Left/Right)
	if (CursorPosition.X < EdgeThreshold)
		ScrollDirection.Y = -1.0f;
	else if (CursorPosition.X > ViewportSize.X - EdgeThreshold)
		ScrollDirection.Y = 1.0f;

	// Vertical edges (Forward/Backward)
	if (CursorPosition.Y < EdgeThreshold)
		ScrollDirection.X = 1.0f;
	else if (CursorPosition.Y > ViewportSize.Y - EdgeThreshold)
		ScrollDirection.X = -1.0f;

	return ScrollDirection.GetSafeNormal() * EdgeScrollSpeed;*/

	const FVector2D CursorPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	if (CursorPosition.X < 0 || CursorPosition.Y < 0)
		return FVector2D::Zero();
	const FVector2D ScreenSize = UWidgetLayoutLibrary::GetViewportSize(this) / UWidgetLayoutLibrary::GetViewportScale(this);
	if (CursorPosition.X > ScreenSize.X || CursorPosition.Y > ScreenSize.Y)
		return FVector2D::Zero();

	const float EdgeSize = ScreenSize.X * (EdgeScrollVerticalScreenSpacePercent / 100);
	if (CursorPosition.X > EdgeSize && CursorPosition.Y > EdgeSize && CursorPosition.X < (ScreenSize.X - EdgeSize) && CursorPosition.Y < (ScreenSize.Y - EdgeSize))
		return FVector2D::Zero();

	FVector2D RawScrollDirection = CursorPosition - (ScreenSize / 2);
	RawScrollDirection.Normalize();
	RawScrollDirection.Y *= -1;
	return RawScrollDirection * EdgeScrollSpeed;

	//return FVector2D::Zero();
}

// Called every frame
void APPCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bFocusing)
		InterpolateToFocusLocation(DeltaTime);
	else
	{
		if (bSettingsEdgeScroll && bEdgeScrollEnabled && !DirectMouseControlActive)
			CalculateAndAddEdgeScrollInput();

		InterpolateToTransformAnchorLocation(DeltaTime);
		InterpolateToTransformAnchorRotation(DeltaTime);
	}

	InterpolateToDesiredCameraPitch(DeltaTime);
	InterpolateToZoomTarget(DeltaTime);
	//AdjustToLandscape(DeltaTime);
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
		EnhancedInputComponent->BindAction(PanSwitchAction, ETriggerEvent::Started, this, &ThisClass::PanSwitchStarted);
		EnhancedInputComponent->BindAction(PanSwitchAction, ETriggerEvent::Completed, this, &ThisClass::PanSwitchCompleted);
		EnhancedInputComponent->BindAction(PanAction, ETriggerEvent::Triggered, this, &ThisClass::PanInputTriggered);
	}
}