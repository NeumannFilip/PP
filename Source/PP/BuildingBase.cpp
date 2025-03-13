

#include "BuildingBase.h"

ABuildingBase::ABuildingBase()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

void ABuildingBase::BeginPlay()
{
	Super::BeginPlay();
	InitializeBuilding();
}

void ABuildingBase::InitializeBuilding()
{
	//TODO:: Logic
	UE_LOG(LogTemp, Warning, TEXT("Building Initialized: %s"), *BuildingName);
}


void ABuildingBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

