// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RiskPlayer.generated.h"

enum Color {
	red,
	orange,
	yellow,
	green,
	blue,
	purple,
	pink,
	white,
	black,
	grey
};

class ATerritory;
class UContinent;
class Card;

/**
 * 
 */
UCLASS()
class RISK_API URiskPlayer : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<TObjectPtr<ATerritory>> Territories;

	UPROPERTY()
	TArray<TObjectPtr<UContinent>> Continents;

	UPROPERTY()
	TArray<TObjectPtr<Card>> Cards;

	int16 Armies;

	void SetName(const FString& newName);
	FString GetName() const;
	void SetColor(Color newColor);
	Color GetColor() const;

private:
	FString Name;
	Color PlayerColor;
};
