// Fill out your copyright notice in the Description page of Project Settings.


#include "Territory.h"

// Sets default values
ATerritory::ATerritory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATerritory::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATerritory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int32 ATerritory::GetArmies() const
{
	return Armies;
}

int32 ATerritory::GetInfantry() const
{
	return Infantry;
}

int32 ATerritory::GetCavalry() const
{
	return Cavalry;
}

int32 ATerritory::GetArtillery() const
{
	return Artillery;
}

void ATerritory::AddInfantry(int32 Num, FVector Location)
{
	Armies += Num;
	Infantry += Num;
}

void ATerritory::AddCavalry(int32 Num, FVector Location)
{
	Armies += 5 * Num;
	Cavalry += Num;
}

void ATerritory::AddArtillery(int32 Num, FVector Location)
{
	Armies += 10 * Num;
	Artillery += Num;
}

void ATerritory::InitArmies(FVector Location)
{
	Armies = 1;
	Infantry = 1;
	Cavalry = 0;
	Artillery = 0;
}
