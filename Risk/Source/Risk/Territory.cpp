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

void ATerritory::AddArmy(int32 Num, FVector Location)
{
	Armies += Num;
}

void ATerritory::InitArmies(FVector Location)
{
	Armies = 1;
}
