// Fill out your copyright notice in the Description page of Project Settings.


#include "RiskCameraController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Territory.h"
#include "CameraPawn.h"
#include "GameFramework/SpringArmComponent.h"

typedef UKismetMathLibrary Math;

ARiskCameraController::ARiskCameraController() : CameraZoom{ 1 }, SelectedActor{ nullptr }
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
	MoveCamera();
	HighlightTerritory();
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

void ARiskCameraController::MoveCamera()
{
	double MouseX, MouseY;
	int32 ScreenX, ScreenY;
	GetViewportSize(ScreenX, ScreenY);
	GetMousePosition(MouseX, MouseY);

	// Get speed from each side of viewport
	double MouseRight = 1 - FMath::Clamp(Math::NormalizeToRange(MouseX, 0, 0.2 * ScreenX), 0, 1);
	double MouseLeft = -FMath::Clamp(Math::NormalizeToRange(MouseX, 0.8 * ScreenX, ScreenX), 0, 1);
	double MouseUp = 1 - FMath::Clamp(Math::NormalizeToRange(MouseY, 0, 0.2 * ScreenX), 0, 1);
	double MouseDown = -FMath::Clamp(Math::NormalizeToRange(MouseY, 0.8 * ScreenX, ScreenX), 0, 1);

	// Create movement vector
	FVector MouseMovement = FVector(MouseRight + MouseLeft, MouseUp + MouseDown, 0);

	// Turn into speed using lerp
	FVector MouseSpeed = (CameraMovement.GetSafeNormal() - MouseMovement).GetClampedToSize(0, 1) * Math::Lerp(10, 60, CameraZoom);

	// Clamp coordinates
	double XPosition = FMath::Clamp(MouseSpeed.X + GetPawn()->GetActorLocation().X, -3750, 3750);
	double YPosition = FMath::Clamp(MouseSpeed.Y + GetPawn()->GetActorLocation().Y, -3375, 3375);

	// Set new location
	GetPawn()->SetActorLocation(FVector(XPosition, YPosition, 0));

	Cast<ACameraPawn>(GetPawn())->SpringArm->TargetArmLength = Math::Lerp(1000, 4000, CameraZoom);
}

void ARiskCameraController::HighlightTerritory()
{
	FHitResult OutHitResult;
	if (GetHitResultUnderCursor(ECC_Visibility, true, OutHitResult))
	{
		AActor* Actor = OutHitResult.GetActor();

		if (HighlightedActor == nullptr || Actor == HighlightedActor)
		{
			HighlightedActor = Actor;
			return;
		}
		if (HighlightedActor->GetClass()->GetSuperClass()->GetName() == "Territory")
		{
			if (HighlightedActor == SelectedActor) {
				Cast<UStaticMeshComponent>(HighlightedActor->GetDefaultAttachComponent())->SetCustomDepthStencilValue(2);
			}
			else
			{
				Cast<UStaticMeshComponent>(HighlightedActor->GetDefaultAttachComponent())->SetCustomDepthStencilValue(0);
			}
		}
		if (Actor->GetClass()->GetSuperClass()->GetName() == "Territory")
		{
			Cast<UStaticMeshComponent>(Actor->GetDefaultAttachComponent())->SetCustomDepthStencilValue(1);
		}
		HighlightedActor = Actor;
	}
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
	CameraZoom = FMath::Clamp(CameraZoom - 0.1, 0.0, 1.0);
}

void ARiskCameraController::MouseDown()
{
	CameraZoom = FMath::Clamp(CameraZoom + 0.1, 0.0, 1.0);
}

void ARiskCameraController::LeftClick()
{
	FHitResult OutHitResult;
	if (GetHitResultUnderCursor(ECC_Visibility, true, OutHitResult))
	{
		AActor* Actor = OutHitResult.GetActor();
		if (SelectedActor == nullptr || Actor == SelectedActor)
		{
			SelectedActor = Actor;
			return;
		}
		if (SelectedActor->GetClass()->GetSuperClass()->GetName() == "Territory")
		{
			Cast<UStaticMeshComponent>(SelectedActor->GetDefaultAttachComponent())->SetCustomDepthStencilValue(0);
		}
		if (Actor->GetClass()->GetSuperClass()->GetName() == "Territory")
		{
			Cast<UStaticMeshComponent>(Actor->GetDefaultAttachComponent())->SetCustomDepthStencilValue(2);
		}
		SelectedActor = Actor;
	}
}