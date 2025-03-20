// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "PPCamera.generated.h"

class USphereComponent;
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
	
	//default classes
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void Serialize(FArchive& Ar) override;

protected:
	
	virtual void BeginPlay() override;

	void AddCameraMovementInput(FVector2D InputVector);

	UFUNCTION(BlueprintCallable, meta=(AdvancedDisplay=bInstant))
	void FocusOnActor(AActor* FocusedActor, bool bInstant = false);
	UFUNCTION(BlueprintCallable, meta=(AdvancedDisplay=bInstant))
	void FocusOnLocation(FVector FocusedLocation, bool bInstant = false);
	UFUNCTION(BlueprintCallable)
	void StopFocus();
	UFUNCTION()
	void AddExternalMovementInput(FVector2D InputVector);
	UFUNCTION(BlueprintCallable)
	void ResetRotationToDefault();
	UFUNCTION(BlueprintCallable)
	void ResetZoomToDefault();
	UFUNCTION(BlueprintCallable)
	void TeleportToLocation(FVector Location);
	
	UFUNCTION(BlueprintCallable)
	void DigitalRotateCamera(float Input);
	
	UFUNCTION(BlueprintCallable)
	void ZoomCamera(float Input);

	UFUNCTION(BlueprintPure)
	float GetZoom() const;
	float GetZoomFraction() const;


	UPROPERTY(BlueprintReadOnly, Category=Focus)
	bool bFocusing{false};
	UPROPERTY(BlueprintReadOnly, Category=Focus)
	AActor* ActorToFocus;
	UPROPERTY(BlueprintReadOnly, Category=Focus)
	FVector LocationToFocus;
	UPROPERTY(BlueprintReadOnly, Category=Focus)
	bool bAttachToFocusedActor{false};
	UPROPERTY(BlueprintReadOnly, Category=Focus)
	AActor* AttachActor;

	
	//input
	void SetDirectMouseControlMode(bool bEnabled);

	UFUNCTION(Category="Input")
	void MovementInputTriggered(const FInputActionValue& ActionValue);
	UFUNCTION(Category="Input")
	void DigitalRotateInputTriggered(const FInputActionValue& ActionValue);
	UFUNCTION(Category="Input")
	void MouseRotateSwitchStarted();
	UFUNCTION(Category="Input")
	void MouseRotateSwitchCompleted();
	UFUNCTION(Category="Input")
	void ZoomInputTriggered(const FInputActionValue& ActionValue);
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Input")
	UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Input")
	UInputAction* DigitalRotateAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Input")
	UInputAction* MouseRotateSwitchAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Input")
	UInputAction* ZoomAction;

	
	
private:

	void MoveCursorOutOfEdge();

	FVector2D CalculateEdgeScrollVector();

	UPROPERTY()
	float ZoomTarget;
	UPROPERTY()
	bool DirectMouseControlActive;
	//For temporary disabling edge scrolling during gameplay
	UPROPERTY()
	bool bEdgeScrollEnabled = true;
	UPROPERTY()
	FRotator DefaultRotation;
	UPROPERTY()
	float ZoomCache = -1;


	//Components
	UPROPERTY(VisibleAnywhere, Category="Camera")
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category="Camera")
	UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera")
	USphereComponent* RootCollision;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera")
	USceneComponent* TransformTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera")
	USceneComponent* ArmRoot;


	//setup
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Camera")
	float Fov{70};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Zoom")
	FVector2D ZoomRange{200, 5000};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Zoom", meta = (EditCondition = "false"))
	float DefaultZoom{2900};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Transform")
	FVector2D DefaultCameraPitchZoomRange{0.65, 0.8};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Transform")
	float DefaultCameraArmAngle{-50};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Transform")
	float DefaultCameraAngle{4.75};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Transform")
	FVector2D CameraPitchClamp{-30, -70};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Transform")
	float CameraPitchFreeAngle{2.5};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Transform")
	float DefaultHeight{0};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Speed")
	UCurveFloat* ZoomSpeedMultiplierCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Speed")
	float EdgeScrollSpeed{1.5};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Speed")
	float DirectionalMovementSpeed{1.5};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Speed")
	float MousePanSpeed{50};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Speed")
	float DigitalRotationSpeed{125};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Speed")
	float AnalogRotationSpeed{30};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Speed")
	float ZoomSpeed{5000};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|InterpSpeed")
	float TransformAnchorInterpSpeed{5};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|InterpSpeed")
	float CameraPitchInterpSpeed{2};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|InterpSpeed")
	float ZoomInterpSpeed{5};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|InterpSpeed")
	float FocusSpeed{10};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	float EdgeScrollVerticalScreenSpacePercent{0.15};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	float GroundAdjustTraceLength{10000};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	float GroundAdjustInterpSpeed{1};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	TEnumAsByte<ETraceTypeQuery> GroundAdjustTraceChannel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	float PitchDampeningMargin{5};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup|Audio")
	FVector2D AudioListenerZoomRange{200, 5000};
	
};
