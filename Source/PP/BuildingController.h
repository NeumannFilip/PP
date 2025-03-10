#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BuildingController.generated.h"

class ABuildableActor;
class AGridManager;

UCLASS()
class PP_API ABuildingController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetupInputComponent() override;

	void OnLeftClick();

	UPROPERTY(EditAnywhere, Category = "Building")
	TSubclassOf<ABuildableActor> BuildingToPlace;

private:
	AGridManager* GetGridManager() const;
	
};
