// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RiskGameController.generated.h"

class AContinent;
class ATerritory;
class AArmy;
class UCard;
class URiskPlayer;

/**
 * 
 */
UCLASS()
class RISK_API ARiskGameController : public APlayerController
{
	GENERATED_BODY()

public:
	ARiskGameController();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets up controls
	virtual void SetupInputComponent() override;

	// Utilities
	void MoveCamera();
	void HighlightTerritory();
	void SelectTerritory(AActor* Actor);
	TArray<int32> RollDice(int32 NumDice) const;

	// Game Functions

	int32 AddArmy(ATerritory* Territory, FVector HitLocation);
	void MoveArmy();
	void GiveArmies();
	int32 TradeArmies(ATerritory* Territory, FVector HitLocation, TCHAR StartType, TCHAR EndType);

	int32 SetAttack(ATerritory* Start, ATerritory* End);
	int32 Attack(int32 PlayerOneDice, int32 PlayerTwoDice, FVector HitLocation);
	int32 OccupyTerritory(const TArray<AArmy*> Armies);

	int32 SetFortify(ATerritory* Start, ATerritory* End);
	void Fortify(const TArray<AArmy*> Armies);

	void GiveCard();
	int32 TradeCards(const TArray<int32>& CardsInd, FVector Location);

	URiskPlayer* FindContOwner(const AContinent* Continent) const;
	bool AreConnectedTerritories(const ATerritory* Start, const ATerritory* End) const;
	int32 CaptureTerritory(ATerritory* Territory);
	bool IsValidTrade(const TArray<int32>& CardsInd) const;
	ATerritory* FindTerritory(const FString& Name) const;

	// Input
	void LeftPress();
	void LeftRelease();
	void RightPress();
	void RightRelease();
	void UpPress();
	void UpRelease();
	void DownPress();
	void DownRelease();
	void MouseUp();
	void MouseDown();
	void LeftClick();
	void LeftClickRelease();

	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	FVector CameraMovement;

	UPROPERTY(BluePrintReadWrite, EditAnywhere)

	TArray<URiskPlayer*> Players;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AActor* SelectedActor;
	AActor* HighlightedActor;
	AArmy* SelectedArmy;
	bool bArmySelected;

	int32 Turn;
	int32 TerritoriesOccupied;

	TArray<UCard*> DrawPile;
	TArray<UCard*> DiscardPile;

	ATerritory* TerrOne;
	ATerritory* TerrTwo;
	int32 Trades;
	bool bGotTradeBonus;
	int32 AttackArmies;
	bool bCaptured;

	double CameraZoom;

};
