// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Continent.generated.h"

class URiskPlayer;
class ATerritory;

UCLASS()
class AContinent : public AActor
{
	GENERATED_BODY()

public:
	AContinent();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	TObjectPtr<URiskPlayer> ContinentOwner;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 NewArmies;

private:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
