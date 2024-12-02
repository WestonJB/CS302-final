// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

class URiskPlayer;
class UContinent;
class ATerritory;
class UCard;

UCLASS()
class RISK_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();
	TArray<FString> GetNames();

	TArray<int8> RollDice(int8 numDice) const;
	void SetTurn(int16 newTurn);
	int16 GetTurn() const;
	void EndTurn();

	bool SetupFinished() const;
	int8 AddArmy(TObjectPtr<ATerritory> territory);
	void GiveArmies();
	int8 TradeArmies(TObjectPtr<ATerritory> territory, TCHAR startType, TCHAR endType);

	int8 SetAttack(TObjectPtr<ATerritory> start, TObjectPtr<ATerritory> end);
	int8 Attack(int8 playerOneDice, int8 playerTwoDice);
	int8 OccupyTerritory(const TArray<TCHAR>& armies);

	int8 SetFortify(TObjectPtr<ATerritory> start, TObjectPtr<ATerritory> end);
	void Fortify(const TArray<TCHAR>& armies);

	void GiveCard();
	int8 TradeCards(const TArray<int8>& cardsInd);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<TObjectPtr<URiskPlayer>> Players;

protected:
	TArray<TObjectPtr<UContinent>> Continents;
	int8 Turn;
	int8 TerritoriesOccupied;

	TArray<TObjectPtr<UCard>> DrawPile;
	TArray<TObjectPtr<UCard>> DiscardPile;

	TObjectPtr<ATerritory> TerrOne;
	TObjectPtr<ATerritory> TerrTwo;
	int16 Trades;
	bool bGotTradeBonus;
	int8 AttackArmies;
	bool bCaptured;

	TObjectPtr<URiskPlayer> FindContOwner(const TObjectPtr<UContinent> continent) const;
	bool AreConnectedTerritories(const TObjectPtr<ATerritory> start, const TObjectPtr<ATerritory> end) const;
	void SelectionSort(TArray<int16>& list) const;
	int8 CaptureTerritory(TObjectPtr<ATerritory> territory);
	bool IsValidTrade(const TArray<int8>& cardsInd) const;
	TObjectPtr<ATerritory> FindTerritory(const FString& name) const;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

};
