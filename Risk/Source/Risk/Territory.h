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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UContinent> Continent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<ATerritory>> NearTerritories;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<URiskPlayer> TerritoryOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Armies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Infantry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cavalry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Artillery;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
