// Fill out your copyright notice in the Description page of Project Settings.


#include "RiskPlayer.h"

void URiskPlayer::SetName(const FString& newName)
{
	Name = newName;
}

FString URiskPlayer::GetName() const
{
	return Name;
}

void URiskPlayer::SetColor(EPlayerColor newColor)
{
	PlayerColor = newColor;
}

EPlayerColor URiskPlayer::GetColor() const
{
	return PlayerColor;
}
