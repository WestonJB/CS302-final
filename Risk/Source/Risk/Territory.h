// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Territory.generated.h"

class URiskPlayer;
class UContinent;

UCLASS()
class RISK_API ATerritory : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerritory();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FString Name;
	TObjectPtr<UContinent> Continent;
	TArray<ATerritory> NearTerritories;
	TObjectPtr<URiskPlayer> Owner;
	int8 Armies;
	int8 Infantry;
	int8 Cavalry;
	int8 Artillery;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
