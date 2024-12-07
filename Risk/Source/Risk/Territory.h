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

	// Adds infantry and updates number of armies
	void AddArmy(int32 Num, FVector Location);

	// Resets territory to 1 infantry
	void InitArmies(FVector Location);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	int32 Armies;

};
