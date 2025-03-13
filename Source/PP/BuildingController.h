#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "BuildingController.generated.h"

class ABuildingBase;
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

	
	
	void UpdateGhostMesh(TSubclassOf<ABuildingBase> BuildingClass);

	template <class T>
	T* GetController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value, "'T' template parameter to GetController must be derived from AController");
		return Cast<T>(GetOwner());
	}

	
private:

	void BeginPlay() override;
	void SetupInput();

	
	
	//World mouse location
	bool GetMouseWorldLocation(FVector& OutLocation) const;


	//Buildings
	UPROPERTY(EditDefaultsOnly, Category="Buildings")
	TSubclassOf<ABuildableActor> TownhallBP;
	UPROPERTY(EditDefaultsOnly, Category="Buildings")
	TSubclassOf<ABuildingBase> HouseBP;
	
	//Pre-view aka GhostMesh
	UPROPERTY()
	UStaticMeshComponent* GhostMesh;
};
