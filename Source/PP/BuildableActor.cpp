// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildableActor.h"
#include "GridManager.h"
#include "GameFramework/GameModeBase.h"

ABuildableActor::ABuildableActor()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_WorldStatic);
}

void ABuildableActor::OnPlaced(FVector& Location)
{
	SetActorLocation(Location);
}