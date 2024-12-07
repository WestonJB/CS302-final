// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RiskGameController.generated.h"

class AContinent;
class ATerritory;
class AArmy;
class URiskPlayer;

enum class EGameState : int8
{
	Neutral,
	PlaceArmies,
	SelectOne,
	SelectTwo,
	Occupy,
	ArmySelected,
};

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

	int32 SetAttack(ATerritory* Start, ATerritory* End);
	int32 Attack(int32 PlayerOneDice, int32 PlayerTwoDice, FVector HitLocation);
	int32 OccupyTerritory(const TArray<AArmy*> Armies);

	URiskPlayer* FindContOwner(const AContinent* Continent) const;
	bool AreConnectedTerritories(const ATerritory* Start, const ATerritory* End) const;
	int32 CaptureTerritory(ATerritory* Territory);
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
	URiskPlayer* CurrentPlayer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AActor* SelectedActor;
	AActor* HighlightedActor;
	AArmy* SelectedArmy;
	EGameState GameState;
	EGameState PreviousGameState;

	int32 Turn;
	int32 TerritoriesOccupied;

	ATerritory* TerrOne;
	ATerritory* TerrTwo;
	int32 AttackArmies;
	bool bCaptured;

	double CameraZoom;

};
