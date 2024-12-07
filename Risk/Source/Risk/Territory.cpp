// Fill out your copyright notice in the Description page of Project Settings.

#include "Territory.h"
#include "Army.h"
#include "Zealot.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATerritory::ATerritory() : NumArmies{ 0 }
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
	return NumArmies;
}

void ATerritory::AddArmy(int32 Num, FVector Location)
{
	NumArmies += Num;

	for (int i = 0; i < Num; ++i)
	{
		UClass* Class = nullptr;
		for (TObjectIterator<AActor> It; It; ++It)
		{
			if (It->GetClass()->GetSuperClass()->GetName() == "Army") {
				Class = It->GetClass();
				break;
			}
		}
		if (Class == nullptr) {
			return;
		}
		AActor* Army = GetWorld()->SpawnActor<AActor>(Class, Location, FRotator::ZeroRotator);
		Army->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void ATerritory::RemoveArmy()
{
	TArray<AActor*> Armies;
	GetAttachedActors(Armies);
	if (!Armies.IsEmpty())
	{
		AActor* Army = Armies.Pop();
		Army->Destroy();
	}
}

void ATerritory::InitArmies(FVector Location)
{
	NumArmies = 1;
	UClass* Class = nullptr;
	for (TObjectIterator<AActor> It; It; ++It)
	{
		if (It->GetClass()->GetSuperClass()->GetName() == "Army") {
			Class = It->GetClass();
			break;
		}
	}
	if (Class == nullptr) {
		return;
	}
	AActor* Army = GetWorld()->SpawnActor<AActor>(Class, Location, FRotator::ZeroRotator);
	Army->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
}
