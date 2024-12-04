// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Territory.generated.h"

class URiskPlayer;
class AContinent;

UCLASS()
class RISK_API ATerritory : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerritory();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<ATerritory>> NearTerritories;

	TObjectPtr<URiskPlayer> TerritoryOwner;

	int32 GetArmies() const;
	int32 GetInfantry() const;
	int32 GetCavalry() const;
	int32 GetArtillery() const;

	// Adds infantry and updates number of armies
	void AddInfantry(int32 Num);

	// Adds cavalry and updates number of armies
	void AddCavalry(int32 Num);

	// Adds artillery and updates number of armies
	void AddArtillery(int32 Num);

	// Resets territory to 1 infantry
	void InitArmies();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	int32 Armies;
	int32 Infantry;
	int32 Cavalry;
	int32 Artillery;

};
