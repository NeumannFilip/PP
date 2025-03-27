
#include "SimpleCamera.h"

#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"


ASimpleCamera::ASimpleCamera()
{
	PrimaryActorTick.bCanEverTick = true;
	
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("Movement");
	FloatingPawnMovement->MaxSpeed = 2000.f;
	FloatingPawnMovement->Acceleration = 2000.f;
	FloatingPawnMovement->Deceleration = 2000.f;
	
	FloatingPawnMovement->bConstrainToPlane = true;
	FloatingPawnMovement->SetPlaneConstraintNormal(FVector(0,0,1));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->TargetArmLength = 2000.f;
	SpringArm->bDoCollisionTest = false;
	RootComponent = SpringArm;
	//SpringArm->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0, DefaultCameraArmAngle, 0)));
	SpringArm->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); 
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
}

void ASimpleCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASimpleCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Edge scrolling
	const FVector2D EdgeScrollDir = GetEdgeScrollDirection();
	if (!EdgeScrollDir.IsZero())
	{
		AddActorWorldOffset(GetActorForwardVector() * EdgeScrollDir.X * MoveSpeed * DeltaTime);
		AddActorWorldOffset(GetActorRightVector() * EdgeScrollDir.Y * MoveSpeed * DeltaTime);
	}

}

void ASimpleCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ThisClass::Zoom);
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ThisClass::Rotate);
	}
}

void ASimpleCamera::SetEdgeScrollEnabled(bool bEnabled)
{
	bEdgeScrollEnabled = bEnabled;
}

void ASimpleCamera::Move(const FInputActionValue& Value)
{
	const FVector2D Movement = Value.Get<FVector2D>();
    
	// For top-down view:
	// - Forward = World X-axis (1,0,0)
	// - Right = World Y-axis (0,1,0)
	// - Ignore Z movement
	
	const FRotator ControlRot(0.f, GetControlRotation().Yaw, 0.f);
	const FVector Forward = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(Forward, Movement.Y); 
	AddMovementInput(Right, Movement.X);  
}

void ASimpleCamera::Rotate(const FInputActionValue& Value)
{
	const float RotationDelta = Value.Get<float>() * RotationSpeed * GetWorld()->DeltaTimeSeconds;
	SpringArm->AddWorldRotation(FRotator(0.f, RotationDelta, 0.f));
}

void ASimpleCamera::Zoom(const FInputActionValue& Value)
{
	const float ZoomDelta = Value.Get<float>() * ZoomSpeed * GetWorld()->DeltaTimeSeconds;
	SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength - ZoomDelta, ZoomRange.X, ZoomRange.Y);
}

FVector2D ASimpleCamera::GetEdgeScrollDirection() const
{
	if (!bEdgeScrollEnabled) return FVector2D::Zero();

	const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
	
	const FVector2D NormPos = FVector2D(
		MousePos.X / ViewportSize.X,
		MousePos.Y / ViewportSize.Y
	);

	FVector2D Direction = FVector2D::Zero();
	
	if (NormPos.X < EdgeScrollThreshold / ViewportSize.X) 
		Direction.Y = -1.f; 
	else if (NormPos.X > 1.f - (EdgeScrollThreshold / ViewportSize.X)) 
		Direction.Y = 1.f;  
	
	if (NormPos.Y < EdgeScrollThreshold / ViewportSize.Y) 
		Direction.X = 1.f;  
	else if (NormPos.Y > 1.f - (EdgeScrollThreshold / ViewportSize.Y)) 
		Direction.X = -1.f; 
    
	return Direction;
}

void ASimpleCamera::CalculateAndAddEdgeScrollInput()
{
	const FVector2D EdgeScrollDir = GetEdgeScrollDirection();
    
	if (!EdgeScrollDir.IsZero())
	{
		const FRotator CameraRot(0.f, GetControlRotation().Yaw, 0.f);
		const FVector Forward = FRotationMatrix(CameraRot).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(CameraRot).GetUnitAxis(EAxis::Y);
		
		const float DeltaTime = GetWorld()->GetDeltaSeconds();
		AddMovementInput(Forward, EdgeScrollDir.X * MoveSpeed * DeltaTime);
		AddMovementInput(Right, EdgeScrollDir.Y * MoveSpeed * DeltaTime);
	}
}

