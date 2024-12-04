// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

class URiskPlayer;
class AContinent;
class ATerritory;
class UCard;
class AArmy;

UCLASS()
class RISK_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();
	TArray<FString> GetNames();

	TArray<int32> RollDice(int32 numDice) const;
	void SetTurn(int32 newTurn);
	int32 GetTurn() const;
	void EndTurn();

	bool SetupFinished() const;
	int32 AddArmy(TObjectPtr<ATerritory> territory);
	void GiveArmies();
	int32 TradeArmies(TObjectPtr<ATerritory> territory, TCHAR startType, TCHAR endType);

	int32 SetAttack(TObjectPtr<ATerritory> start, TObjectPtr<ATerritory> end);
	int32 Attack(int32 playerOneDice, int32 playerTwoDice);
	int32 OccupyTerritory(const TArray<TObjectPtr<AArmy>> Armies);

	int32 SetFortify(TObjectPtr<ATerritory> start, TObjectPtr<ATerritory> end);
	void Fortify(const TArray<TObjectPtr<AArmy>> Armies);

	void GiveCard();
	int32 TradeCards(const TArray<int32>& cardsInd);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<TObjectPtr<URiskPlayer>> Players;

protected:
	TArray<TObjectPtr<AContinent>> Continents;
	int32 Turn;
	int32 TerritoriesOccupied;

	TArray<TObjectPtr<UCard>> DrawPile;
	TArray<TObjectPtr<UCard>> DiscardPile;

	TObjectPtr<ATerritory> TerrOne;
	TObjectPtr<ATerritory> TerrTwo;
	int32 Trades;
	bool bGotTradeBonus;
	int32 AttackArmies;
	bool bCaptured;

	TObjectPtr<URiskPlayer> FindContOwner(const TObjectPtr<AContinent> continent) const;
	bool AreConnectedTerritories(const TObjectPtr<ATerritory> start, const TObjectPtr<ATerritory> end) const;
	int32 CaptureTerritory(TObjectPtr<ATerritory> territory);
	bool IsValidTrade(const TArray<int32>& cardsInd) const;
	TObjectPtr<ATerritory> FindTerritory(const FString& name) const;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

};
