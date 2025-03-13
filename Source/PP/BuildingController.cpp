#include "BuildingController.h"
#include "BuildableActor.h"
#include "BuildingBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputHandler.h"


void ABuildingController::SetupInputComponent()
{
	Super::SetupInputComponent();

	///InputComponent->BindAction("LeftClick", IE_Pressed, this, ABuildingController::OnLeftClick());
}

void ABuildingController::OnLeftClick()
{
	if (TownhallBP)
	{
		// Update the ghost mesh for the Townhall
		UpdateGhostMesh(TownhallBP);

		// Spawn the Townhall
		SpawnBuilding(TownhallBP);
	}
}


void ABuildingController::SpawnBuilding(TSubclassOf<ABuildableActor> BuildingClass)
{
	if(!BuildingToPlace) return;

	//World Location under mouse cursor
	FVector MouseLocation;
	if(GetMouseWorldLocation(MouseLocation))
	{
		if(ABuildableActor* NewBuilding = GetWorld()->SpawnActor<ABuildableActor>(BuildingToPlace, MouseLocation, FRotator::ZeroRotator))
		{
			NewBuilding->OnPlaced(MouseLocation);
		}
	}
}

void ABuildingController::UpdateGhostMesh(TSubclassOf<ABuildingBase> BuildingClass)
{
	if(!BuildingClass) return;

	ABuildingBase* DefaultBuilding = BuildingClass.GetDefaultObject();
	if(DefaultBuilding && DefaultBuilding->MeshComponent)
	{
		//Creating mesh if it doesn't exist
		if(!GhostMesh)
		{
			GhostMesh = NewObject<UStaticMeshComponent>(this);
			GhostMesh->RegisterComponent();
		}

		GhostMesh->SetStaticMesh(DefaultBuilding->MeshComponent->GetStaticMesh());
		FVector MouseLocation;
		if(GetMouseWorldLocation(MouseLocation))
			GhostMesh->SetWorldLocation(MouseLocation);
		
		GhostMesh->SetVisibility(true);
	}
}

void ABuildingController::BeginPlay()
{
	Super::BeginPlay();
	if (!InputHandler)
	{
		InputHandler = NewObject<UInputHandler>(this);
	}

	// Set up input
	SetupInput();

	GhostMesh = NewObject<UStaticMeshComponent>(this);
	GhostMesh->RegisterComponent();
	GhostMesh->SetVisibility(false);
}

void ABuildingController::SetupInput()
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Bind input actions
		InputHandler->BindInputActions(EnhancedInputComponent, this);

		// Add input mapping context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		
	}
}

bool ABuildingController::GetMouseWorldLocation(FVector& OutLocation) const
{
	FHitResult HitResult;
	if(GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		OutLocation = HitResult.Location;
		return true;
	}
	return false;
}
