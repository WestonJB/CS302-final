// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Card.h"
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
class AContinent;
class Card;

/**
 * 
 */
UCLASS()
class RISK_API URiskPlayer : public UObject
{
	GENERATED_BODY()

public:
	TArray<TObjectPtr<ATerritory>> Territories;
	TArray<TObjectPtr<AContinent>> Continents;
	TArray<TObjectPtr<UCard>> Cards;
	int32 Armies;

	void SetName(const FString& newName);
	FString GetName() const;
	void SetColor(Color newColor);
	Color GetColor() const;

private:
	FString Name;
	Color PlayerColor;
};
