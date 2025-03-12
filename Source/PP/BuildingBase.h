#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingBase.generated.h"

UCLASS()
class PP_API ABuildingBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABuildingBase();

protected:
	
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditDefaultsOnly, Category="Building")
	FString BuildingName;
	UPROPERTY(EditDefaultsOnly, Category="Building")
	int32 ConstructionCost;
	UPROPERTY(EditDefaultsOnly, Category="Building")
	float ConstructionTime;

	virtual void InitializeBuilding();
	
	virtual void Tick(float DeltaTime) override;

};
