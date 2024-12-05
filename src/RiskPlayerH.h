// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EPlayerColor : uint8
{
    Red UMETA(DisplayName = "Red"),
    Blue UMETA(DisplayName = "Blue"),
    Green UMETA(DisplayName = "Green"),
    Yellow UMETA(DisplayName = "Yellow")
    Brown UMETA(DisplayName = "Brown")
    Black UMETA(DisplayName = "Black")
}; //6 colors available, as there can be at most 6 players in Risk

UCLASS(Blueprintable)
class RISK UPlayer : public UObject
{
    GENERATED_BODY()

private:

    UPROPERTY()
    FString Name;

    UPROPERTY()
    EPlayerColor Color;

public:
 
    UFUNCTION(BlueprintCallable, Category = "Player")
    void SetName(const FString& NewName);

    UFUNCTION(BlueprintCallable, Category = "Player")
    FString GetName() const;

    UFUNCTION(BlueprintCallable, Category = "Player")
    void SetColor(EPlayerColor NewColor);

    UFUNCTION(BlueprintCallable, Category = "Player")
    EPlayerColor GetColor() const;
};
