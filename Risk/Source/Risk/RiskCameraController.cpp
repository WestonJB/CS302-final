// Fill out your copyright notice in the Description page of Project Settings.


#include "RiskCameraController.h"
#include "Kismet/KismetMathLibrary.h"
#include "CameraPawn.h"
#include "GameFramework/SpringArmComponent.h"

typedef UKismetMathLibrary Math;

ARiskCameraController::ARiskCameraController() : CameraZoom{ 1 }
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ARiskCameraController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
}

void ARiskCameraController::Tick(float DeltaTime)
{
	// Move Camera
	double MouseX, MouseY;
	int32 ScreenX, ScreenY;
	GetViewportSize(ScreenX, ScreenY);
	GetMousePosition(MouseX, MouseY);

	double MouseRight = 1 - FMath::Clamp(Math::NormalizeToRange(MouseX, 0, 0.1 * ScreenX), 0, 1);
	double MouseLeft = -FMath::Clamp(Math::NormalizeToRange(MouseX, 0.9 * ScreenX, ScreenX), 0 , 1);
	double MouseUp = 1 - FMath::Clamp(Math::NormalizeToRange(MouseY, 0, 0.1 * ScreenX), 0, 1);
	double MouseDown = -FMath::Clamp(Math::NormalizeToRange(MouseY, 0.9 * ScreenX, ScreenX), 0, 1);

	FVector MouseMovement = FVector(MouseRight + MouseLeft, MouseUp + MouseDown, 0);

	FVector MouseSpeed = (CameraMovement.GetSafeNormal() - MouseMovement).GetClampedToSize(0, 1) * Math::Lerp(7, 30, CameraZoom);

	GetPawn()->SetActorLocation(GetPawn()->GetActorLocation() + MouseSpeed);

	Cast<ACameraPawn>(GetPawn())->SpringArm->TargetArmLength = Math::Lerp(400, 2000, CameraZoom);

}

void ARiskCameraController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Left", IE_Pressed, this, &ARiskCameraController::LeftPress);
	InputComponent->BindAction("Left", IE_Released, this, &ARiskCameraController::LeftRelease);
	InputComponent->BindAction("Right", IE_Pressed, this, &ARiskCameraController::RightPress);
	InputComponent->BindAction("Right", IE_Released, this, &ARiskCameraController::RightRelease);
	InputComponent->BindAction("Up", IE_Pressed, this, &ARiskCameraController::UpPress);
	InputComponent->BindAction("Up", IE_Released, this, &ARiskCameraController::UpRelease);
	InputComponent->BindAction("Down", IE_Pressed, this, &ARiskCameraController::DownPress);
	InputComponent->BindAction("Down", IE_Released, this, &ARiskCameraController::DownRelease);
	InputComponent->BindKey(EKeys::MouseScrollUp, IE_Pressed, this, &ARiskCameraController::MouseUp);
	InputComponent->BindKey(EKeys::MouseScrollDown, IE_Pressed, this, &ARiskCameraController::MouseDown);
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ARiskCameraController::LeftClick);
}

void ARiskCameraController::LeftPress()
{
	CameraMovement += FVector(-1, 0, 0);
}

void ARiskCameraController::LeftRelease()
{
	CameraMovement += FVector(1, 0, 0);
}

void ARiskCameraController::RightPress()
{
	CameraMovement += FVector(1, 0, 0);
}

void ARiskCameraController::RightRelease()
{
	CameraMovement += FVector(-1, 0, 0);
}

void ARiskCameraController::UpPress()
{
	CameraMovement += FVector(0, -1, 0);
}

void ARiskCameraController::UpRelease()
{
	CameraMovement += FVector(0, 1, 0);
}

void ARiskCameraController::DownPress()
{
	CameraMovement += FVector(0, 1, 0);
}

void ARiskCameraController::DownRelease()
{
	CameraMovement += FVector(0, -1, 0);
}

void ARiskCameraController::MouseUp()
{
	GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, FString::Printf(TEXT("%f"), CameraZoom));
	CameraZoom = FMath::Clamp(CameraZoom - 0.1, 0.0, 1.0);
}

void ARiskCameraController::MouseDown()
{
	GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, FString::Printf(TEXT("%f"), CameraZoom));
	CameraZoom = FMath::Clamp(CameraZoom + 0.1, 0.0, 1.0);
}

void ARiskCameraController::LeftClick()
{
	FHitResult OutHitResult;
	GetHitResultUnderCursor(ECC_WorldStatic, false, OutHitResult);
	GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, OutHitResult.GetActor()->GetName());
}