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

void URiskPlayer::SetColor(Color newColor)
{
	PlayerColor = newColor;
}

Color URiskPlayer::GetColor() const
{
	return PlayerColor;
}
