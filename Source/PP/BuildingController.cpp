#include "BuildingController.h"
#include "BuildableActor.h"
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

void ABuildingController::BeginPlay()
{
	Super::BeginPlay();
	if (!InputHandler)
	{
		InputHandler = NewObject<UInputHandler>(this);
	}

	// Set up input
	SetupInput();
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
