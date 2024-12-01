// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

UCLASS()
class RISK_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

	TArray<AActor> players;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	TArray<AActor> continents;
	int8 turn;
	int8 terrOcc;

	TArray<AActor> drawPile;
	TArray<AActor> discardPile;
	AActor& terrOne;
	AActor& terrTwo;
	int16 trades;
	bool gotTradeBonus;
	int8 attArm;
	bool captured;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
