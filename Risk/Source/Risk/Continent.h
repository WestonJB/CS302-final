// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Continent.generated.h"

class URiskPlayer;
class ATerritory;

UCLASS()
class UContinent : public UObject
{
	GENERATED_BODY()

public:
	UContinent();
	
	FString Name;
	TObjectPtr<URiskPlayer> Owner;
	TArray<TObjectPtr<ATerritory>> Territories;
	int32 NewArmies;
};
