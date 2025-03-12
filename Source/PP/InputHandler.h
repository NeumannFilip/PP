// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputHandler.generated.h"

class UEnhancedInputComponent;
class APlayerController;

/**
 * 
 */
UCLASS()
class PP_API UInputHandler : public UActorComponent
{
	GENERATED_BODY()
	
public:
	void BindInputActions(UEnhancedInputComponent* EnhancedInputComponent, APlayerController* PlayerController);

	//input handlers
	void OnLeftClickTriggered();
	void OnRightClickTriggered();
	void OnCameraMovementTriggered(const FInputActionValue& Value);
	void OnZoomTriggered(const FInputActionValue& Value);

	//Input Actions
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LeftClickAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* RightClickAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* CameraMovementAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* CameraZoomAction;


	
	
};
