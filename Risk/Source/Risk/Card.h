// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Card.generated.h"

/**
 * 
 */
UCLASS()
class RISK_API UCard : public UObject
{
	GENERATED_BODY()

public:
	FString Territory;
	FString Army;
};
