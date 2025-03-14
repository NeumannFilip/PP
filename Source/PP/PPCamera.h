// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PPCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class PP_API APPCamera : public APawn
{
	GENERATED_BODY()

public:
	APPCamera();
	
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	void MoveCamera(const FInputActionValue& Value);
	void ZoomCamera(const FInputActionValue& Value);
	void RotateCamera(const FInputActionValue& Value);


private:

	UPROPERTY(VisibleAnywhere, Category="Camera")
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category="Camera")
	UCameraComponent* Camera;

	//settings
	UPROPERTY(EditAnywhere, Category = "Camera")
	float MoveSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float ZoomSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float RotationSpeed = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MinZoom = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MaxZoom = 3000.0f;
	//Input
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ZoomAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* RotateAction;


};
