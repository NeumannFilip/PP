// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"


AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	GridCells.SetNum(GridSizeX);
	for(int32 x = 0; x<GridSizeX; x++)
	{
		GridCells[x].SetNum(GridSizeY);
		for(int32 y = 0; y< GridSizeY; y++)
		{ 
			GridCells[x][y] = false;
		}
	}

	//Visible grid in the editor
#if WITH_EDITOR
	for (int32 x = 0; x < GridSizeX; x++)
	{
		for (int32 y = 0; y < GridSizeY; y++)
		{
			FVector CellCenter = FVector(x * CellSize, y * CellSize, 0);
			DrawDebugBox(GetWorld(), CellCenter, FVector(CellSize / 2), FColor::Green, true);
		}
	}
#endif
}

bool AGridManager::GetGridCoordinates(const FVector& WorldLocation, int32& OutGridX, int32& OutGridY) const
{
	OutGridX = FMath::FloorToInt(WorldLocation.X / CellSize);
	OutGridY = FMath::FloorToInt(WorldLocation.Y/ CellSize);

	return (OutGridX >= 0 && OutGridX < GridSizeX && OutGridY >=0 && OutGridY < GridSizeY);
}

bool AGridManager::IsCellFree(int32 GridX, int32 GridY) const
{
	if(GridX >= 0 && GridX < GridSizeX && GridY >= 0 && GridY < GridSizeY)
	{
		return !GridCells[GridX][GridY];
	}
	return false;
	
}

void AGridManager::OccupyCell(int32 GridX, int32 GridY)
{
	if(GridX >= 0 && GridX < GridSizeX && GridY >= 0 && GridY < GridSizeY)
	{
		GridCells[GridX][GridY] = true;
	}
}
