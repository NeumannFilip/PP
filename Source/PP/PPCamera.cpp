// Fill out your copyright notice in the Description page of Project Settings.


#include "PPCamera.h"

#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

APPCamera::APPCamera()
{

	PrimaryActorTick.bCanEverTick = false;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 1000.0f; 
	SpringArm->bDoCollisionTest = false; 
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}


void APPCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APPCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APPCamera::MoveCamera(const FInputActionValue& Value)
{
	FVector2D Movement = Value.Get<FVector2D>();
	FVector NewLocation = GetActorLocation();
	NewLocation.X += Movement.X * MoveSpeed * GetWorld()->GetDeltaSeconds();
	NewLocation.Y += Movement.Y * MoveSpeed * GetWorld()->GetDeltaSeconds();
	SetActorLocation(NewLocation);
}

void APPCamera::ZoomCamera(const FInputActionValue& Value)
{
	float ZoomDelta = Value.Get<float>();
	float NewArmLength = SpringArm->TargetArmLength + ZoomDelta * ZoomSpeed * GetWorld()->GetDeltaSeconds();
	NewArmLength = FMath::Clamp(NewArmLength, MinZoom, MaxZoom);
	SpringArm->TargetArmLength = NewArmLength;
}

void APPCamera::RotateCamera(const FInputActionValue& Value)
{
	float RotationDelta = Value.Get<float>();
	FRotator NewRotation = SpringArm->GetComponentRotation();
	NewRotation.Yaw += RotationDelta * RotationSpeed * GetWorld()->GetDeltaSeconds();
	SpringArm->SetWorldRotation(NewRotation);
}

// Called to bind functionality to input
void APPCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

