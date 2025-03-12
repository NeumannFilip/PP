// Fill out your copyright notice in the Description page of Project Settings.


#include "InputHandler.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"

void UInputHandler::BindInputActions(UEnhancedInputComponent* EnhancedInputComponent,
	APlayerController* PlayerController)
{

	if(!EnhancedInputComponent || !PlayerController) return;

	EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Triggered, this, &ThisClass::OnLeftClickTriggered);
	EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Triggered, this, &ThisClass::OnRightClickTriggered);
	EnhancedInputComponent->BindAction(CameraMovementAction, ETriggerEvent::Triggered, this, &ThisClass::OnCameraMovementTriggered);
	EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &ThisClass::OnZoomTriggered);
}

void UInputHandler::OnLeftClickTriggered()
{
	UE_LOG(LogTemp, Warning, TEXT("Left Click Triggered"));
}

void UInputHandler::OnRightClickTriggered()
{
	UE_LOG(LogTemp, Warning, TEXT("Right Click Triggered"));
}

void UInputHandler::OnCameraMovementTriggered(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("Camera Movement: %s"), *MovementVector.ToString());
}

void UInputHandler::OnZoomTriggered(const FInputActionValue& Value)
{
	float ZoomValue = Value.Get<float>();
	UE_LOG(LogTemp, Warning, TEXT("Zoom: %f"), ZoomValue);
}
