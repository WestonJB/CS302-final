// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RiskCameraController.generated.h"

/**
 * 
 */
UCLASS()
class RISK_API ARiskCameraController : public APlayerController
{
	GENERATED_BODY()

public:
	ARiskCameraController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupInputComponent() override;

	// Keyboard
	void LeftPress();
	void LeftRelease();
	void RightPress();
	void RightRelease();
	void UpPress();
	void UpRelease();
	void DownPress();
	void DownRelease();

	// Mouse
	void MouseUp();
	void MouseDown();
	void LeftClick();

	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	APawn* CameraPawn;

	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	FVector CameraMovement;

	UPROPERTY(BluePrintReadWrite, EditAnywhere)
	double CameraZoom;

};
