// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

void UPlayer::SetName(const FString& NewName)
{
    Name = NewName;
}

FString UPlayer::GetName() const
{
    return Name;
}

void UPlayer::SetColor(EPlayerColor NewColor)
{
    Color = NewColor;
}

EPlayerColor UPlayer::GetColor() const
{
    return Color;
}