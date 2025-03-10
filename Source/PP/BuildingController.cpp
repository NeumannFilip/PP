


#include "BuildingController.h"

#include "BuildableActor.h"
#include "GridManager.h"
#include "Kismet/GameplayStatics.h"

void ABuildingController::SetupInputComponent()
{
	Super::SetupInputComponent();

	///InputComponent->BindAction("LeftClick", IE_Pressed, this, ABuildingController::OnLeftClick());
}

void ABuildingController::OnLeftClick()
{
	AGridManager* GridManager = GetGridManager();
	if (!GridManager || !BuildingToPlace) return;

	// Get mouse position in the world
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	// Convert world location to grid coordinates
	int32 GridX, GridY;
	if (GridManager->GetGridCoordinates(HitResult.Location, GridX, GridY))
	{
		if (GridManager->IsCellFree(GridX, GridY))
		{
			// Spawn the building
			FVector SpawnLocation = FVector(GridX * GridManager->CellSize, GridY * GridManager->CellSize, 0);
			ABuildableActor* NewBuilding = GetWorld()->SpawnActor<ABuildableActor>(BuildingToPlace, SpawnLocation, FRotator::ZeroRotator);
			NewBuilding->OnPlaced(GridX, GridY);

			// Mark the cell as occupied
			GridManager->OccupyCell(GridX, GridY);
		}
	}
	
}

AGridManager* ABuildingController::GetGridManager() const
{
	return Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
}
