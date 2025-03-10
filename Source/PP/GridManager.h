// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

UCLASS()
class PP_API AGridManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGridManager();

	//Number of cells along x-axis
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	int32 GridSizeX = 10;

	//Y-Axis
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	int32 GridSizeY = 10;

	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	float CellSize = 100.f;

	//2D array to track cell occupancy (true = occupied, false  = free)
	TArray<TArray<bool>> GridCells;

protected:
	virtual void BeginPlay() override;

public:

	bool GetGridCoordinates(const FVector& WorldLocation, int32& OutGridX, int32& OutGridY) const;

	bool IsCellFree(int32 GridX, int32 GridY) const;

	void OccupyCell(int32 GridX, int32 GridY);
};
