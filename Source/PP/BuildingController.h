#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "BuildingController.generated.h"

class ABuildableActor;
class UInputHandler;

UCLASS()
class PP_API ABuildingController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetupInputComponent() override;

	void OnLeftClick();

	UPROPERTY(EditAnywhere, Category="Building")
	TSubclassOf<ABuildableActor> BuildingToPlace;

	UPROPERTY()
	UInputHandler* InputHandler;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UFUNCTION(BlueprintCallable, Category = "Building")
	void SpawnBuilding(TSubclassOf<ABuildableActor> BuildingClass);
	
private:

	void BeginPlay() override;
	void SetupInput();

	
	
	//World mouse location
	bool GetMouseWorldLocation(FVector& OutLocation) const;
	
};
