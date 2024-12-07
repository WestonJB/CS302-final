// Fill out your copyright notice in the Description page of Project Settings.


#include "Zealot.h"

AZealot::AZealot()
{
	UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	SetRootComponent(MeshComponent);
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Game/Armies/Zealot.Zealot"));
	MeshComponent->SetStaticMesh(MeshObj.Object);

}
