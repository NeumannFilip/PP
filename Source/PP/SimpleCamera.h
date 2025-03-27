// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "SimpleCamera.generated.h"

class USphereComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UFloatingPawnMovement;

UCLASS()
class PP_API ASimpleCamera : public APawn
{
	GENERATED_BODY()

public:

	ASimpleCamera();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Components)
	UFloatingPawnMovement* FloatingPawnMovement;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	
	UPROPERTY(EditAnywhere, Category="Camera")
	float MoveSpeed = 1500.f;

	UPROPERTY(EditAnywhere, Category="Camera")
	float RotationSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category="Camera")
	float ZoomSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category="Camera")
	FVector2D ZoomRange = FVector2D(500.f, 5000.f);

	UPROPERTY(EditAnywhere, Category="Camera")
	float EdgeScrollThreshold = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Input")
	UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Input")
	UInputAction* ZoomAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Input")
	UInputAction* RotateAction;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category="Camera")
	void SetEdgeScrollEnabled(bool bEnabled);

	void Move(const FInputActionValue& Value);
	void Rotate(const FInputActionValue& Value);
	void Zoom(const FInputActionValue& Value);
	FVector2D GetEdgeScrollDirection() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Transform")
	float DefaultCameraArmAngle{-50};

private:
	UPROPERTY()
	bool bEdgeScrollEnabled = true;
	
	void CalculateAndAddEdgeScrollInput();
};
