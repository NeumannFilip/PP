// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildableActor.h"
#include "GridManager.h"
#include "GameFramework/GameModeBase.h"

ABuildableActor::ABuildableActor()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	PrimaryActorTick.bCanEverTick = true;

}

void ABuildableActor::OnPlaced(int32 GridX, int32 GridY)
{
	// Logic to snap to grid or update visuals
	// (Optional: Snap to the grid cell's center)
}



