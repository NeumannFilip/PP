// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildableActor.generated.h"

UCLASS()
class PP_API ABuildableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ABuildableActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components")
	UStaticMeshComponent* MeshComponent;

	//on place building
	void OnPlaced(FVector& Location);
	
};
