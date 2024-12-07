// Fill out your copyright notice in the Description page of Project Settings.

#include "RiskGameController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Continent.h"
#include "Territory.h"
#include "Army.h"
#include "CameraPawn.h"
#include "RiskPlayer.h"
#include "GameFramework/SpringArmComponent.h"

typedef UKismetMathLibrary Math;

// Internal

ARiskGameController::ARiskGameController() : CameraZoom{ 1 }, SelectedActor{ nullptr }, GameState{ 0 }, bGameStateUpdate{ false }
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	for (int i = 0; i < 4; ++i) {
		Players.Add(NewObject<URiskPlayer>());
		Players[i]->Armies = 5;
	}
	CurrentPlayer = Players[0];
}

void ARiskGameController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
}

void ARiskGameController::Tick(float DeltaTime)
{
	MoveCamera();
	switch (GameState)
	{
	case EGameState::Neutral:
		break;
	case EGameState::PlaceArmies:
	case EGameState::SelectOne:
	case EGameState::SelectTwo:
	case EGameState::Occupy:
		HighlightTerritory();
		break;
	case EGameState::ArmySelected:
		MoveArmy();
		break;
	default:
		break;
	}
}

void ARiskGameController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Left", IE_Pressed, this, &ARiskGameController::LeftPress);
	InputComponent->BindAction("Left", IE_Released, this, &ARiskGameController::LeftRelease);
	InputComponent->BindAction("Right", IE_Pressed, this, &ARiskGameController::RightPress);
	InputComponent->BindAction("Right", IE_Released, this, &ARiskGameController::RightRelease);
	InputComponent->BindAction("Up", IE_Pressed, this, &ARiskGameController::UpPress);
	InputComponent->BindAction("Up", IE_Released, this, &ARiskGameController::UpRelease);
	InputComponent->BindAction("Down", IE_Pressed, this, &ARiskGameController::DownPress);
	InputComponent->BindAction("Down", IE_Released, this, &ARiskGameController::DownRelease);
	InputComponent->BindKey(EKeys::MouseScrollUp, IE_Pressed, this, &ARiskGameController::MouseUp);
	InputComponent->BindKey(EKeys::MouseScrollDown, IE_Pressed, this, &ARiskGameController::MouseDown);
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ARiskGameController::LeftClick);
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &ARiskGameController::LeftClickRelease);
}

// Interface Functions

void ARiskGameController::MoveCamera()
{
	// Turn movement into speed using lerp
	FVector MouseSpeed = (CameraMovement.GetSafeNormal()).GetClampedToSize(0, 1) * Math::Lerp(10, 60, CameraZoom);

	// Clamp coordinates
	double XPosition = FMath::Clamp(MouseSpeed.X + GetPawn()->GetActorLocation().X, -3750, 3750);
	double YPosition = FMath::Clamp(MouseSpeed.Y + GetPawn()->GetActorLocation().Y, -3375, 3375);

	// Set new location
	GetPawn()->SetActorLocation(FVector(XPosition, YPosition, 0));

	Cast<ACameraPawn>(GetPawn())->SpringArm->TargetArmLength = Math::Lerp(1000, 4000, CameraZoom);
}

void ARiskGameController::HighlightTerritory()
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

void ARiskGameController::SelectTerritory(AActor* Actor)
{
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

TArray<int32> ARiskGameController::RollDice(int32 NumDice) const
{
	TArray<int32> Rolls;
	Rolls.SetNum(NumDice);
	for (int32 i = 0; i < NumDice; i++)
		Rolls[i] = FMath::RandRange(1,6);
	return Rolls;
}

// Game Functions

int32 ARiskGameController::AddArmy(ATerritory* Territory, FVector HitLocation)
{
	/* Return Key:
	 * 0: added a piece to the territory
	 * 1: error, must add piece to unoccupied territory (start of game)
	 * 2: error, must add piece to one's own territory */
	URiskPlayer* GamePlayer = Players[Turn];
	if (TerritoriesOccupied < 42)
	{ // +++++++
		if (Territory->TerritoryOwner != nullptr)
		{
			return 1;
		}
		Territory->TerritoryOwner = GamePlayer;
		GamePlayer->Territories.Add(Territory);
		Territory->InitArmies(HitLocation);
		--(GamePlayer->Armies);
		AContinent* Continent = Cast<AContinent>(Territory->GetAttachParentActor());
		if (FindContOwner(Continent) == GamePlayer) {
			Continent->ContinentOwner = GamePlayer;
			GamePlayer->Continents.Add(Continent);
		}
		++TerritoriesOccupied;
	}
	else {
		if (Territory->TerritoryOwner != GamePlayer) return 2;
		Territory->AddArmy(1, HitLocation);
		--GamePlayer->Armies; // unnecessary after the setup of the game
	}
	return 0;
}

void ARiskGameController::MoveArmy()
{
	FHitResult OutHitResult;
	if (GetHitResultUnderCursor(ECC_GameTraceChannel1, true, OutHitResult))
	{
		AActor* Actor = OutHitResult.GetActor();
		if (Actor->GetName() == SelectedArmy->GetAttachParentActor()->GetName()) {
			SelectedArmy->SetActorLocation(OutHitResult.Location);
		}
	}
}

void ARiskGameController::GiveArmies()
{
	URiskPlayer* GamePlayer = Players[Turn];
	int32 newArmies = GamePlayer->Territories.Num() / 3;
	for (AContinent* i : GamePlayer->Continents) newArmies += 4;
	if (3 < newArmies) GamePlayer->Armies += newArmies;
	else GamePlayer->Armies += 3;
}

int32 ARiskGameController::SetAttack(ATerritory* Start, ATerritory* End)
{
	/* Return Key:
	 * 0: set start and end territories for attacking
	 * 1: error, do not own starting territory
	 * 2: error, player owns ending territory
	 * 3: error, territories are not connected
	 * 4: error, do not have enough armies on starting territory */
	URiskPlayer* GamePlayer = Players[Turn];
	if (Start->TerritoryOwner != GamePlayer) return 1;
	if (End->TerritoryOwner == GamePlayer) return 2;
	if (!AreConnectedTerritories(Start, End)) return 3;
	if (Start->GetArmies() < 2) return 4;
	TerrOne = Start;
	TerrTwo = End;
	return 0;
}

int32 ARiskGameController::Attack(int32 PlayerOneDice, int32 PlayerTwoDice, FVector HitLocation)
{
	/* Return Key:
	 * 0: battle happened
	 * 1: capture
	 * 2: captured continent
	 * 3: captured world (game over)
	 * 11: capture and eliminated a player
	 * 12: captured continent and eliminated a player
	 * 4: error, player one must roll 1-3 dice
	 * 5: error, player one must have at least one more infantry than dice
	 * 6: error, player two must roll 1-2 dice
	 * 7: error, player two must have at least one infantry per die */
	 // some error checking
	if (PlayerOneDice < 1 || PlayerOneDice > 3) return 4;
	if (TerrOne->GetArmies() < PlayerOneDice + 1) return 5;
	if (PlayerOneDice < 1 || PlayerTwoDice > 2) return 6;
	if (TerrTwo->GetArmies() < PlayerTwoDice) return 7;

	// attack
	TArray<int32> PlayerOneRoll = RollDice(PlayerOneDice);
	TArray<int32> PlayerTwoRoll = RollDice(PlayerTwoDice);

	// sort the rolls
	PlayerOneRoll.Sort();
	PlayerTwoRoll.Sort();

	// compare the two highest rolls
	if (PlayerOneRoll[PlayerOneDice - 1] > PlayerTwoRoll[PlayerTwoDice - 1]) {
		// player one wins the battle
		TerrTwo->AddArmy(-1, HitLocation);
	}
	else {
		// player two wins the battle
		TerrOne->AddArmy(-1, HitLocation);
	}
	// see if there is another battle to compare and compare the second highest
	if (PlayerOneDice > 1 && PlayerTwoDice > 1) {
		if (PlayerOneRoll[PlayerOneDice - 2]
			> PlayerTwoRoll[PlayerTwoDice - 2]) {
			// player one wins the battle
			TerrTwo->AddArmy(-1, HitLocation);
		}
		else {
			// player two wins the battle
			TerrOne->AddArmy(-1, HitLocation);
		}
	}
	// if the opposing player has no more armies, then capture his territory
	if (TerrTwo->GetArmies() == 0) {
		int32 CaptureVal = CaptureTerritory(TerrTwo);
		if (CaptureVal == 2) return 3;
		// note: the player needs to move a piece to the captured territory
		AttackArmies = PlayerOneDice;
		return CaptureVal + 1;
	}
	return 0;
}

int32 ARiskGameController::OccupyTerritory(const TArray<AArmy*> Armies)
{
	/* Return Key:
	 * 0: occupied the territory
	 * 1: error, need as many armies as dice rolled to capture (AttackArmies) */
	int32 MovedArmies = 0;
	for (AArmy* Army : Armies) {
		FString ArmyName = Army->GetClass()->GetName();
		if (ArmyName == "BP_Infantry_C") {
			MovedArmies += 1;
		}
		else if (ArmyName == "BP_Cavalry_C") {
			MovedArmies += 5;
		}
		else if (ArmyName == "BP_Artillery_C") {
			MovedArmies += 10;
		}
	}
	if (MovedArmies < AttackArmies) return 1;
	AttackArmies = 0;
	return 0;
}

int32 ARiskGameController::GetNumArmies()
{
	return CurrentPlayer->Armies;
}

// Game Utilities

URiskPlayer* ARiskGameController::FindContOwner(const AContinent* Continent) const
{
	TArray<AActor*> Territories;
	Continent->GetAttachedActors(Territories);
	URiskPlayer* OwningPlayer = Cast<ATerritory>(Territories[0])->TerritoryOwner;
	for (AActor* Territory : Territories) {
		if (Cast<ATerritory>(Territory)->TerritoryOwner != OwningPlayer) return nullptr;
	}
	return OwningPlayer;
}

bool ARiskGameController::AreConnectedTerritories(const ATerritory* Start, const ATerritory* End) const
{
	return Start->NearTerritories.Contains(End);
}

int32 ARiskGameController::CaptureTerritory(ATerritory* Territory)
{
	/* Return Key:
	 * 0: normal capture
	 * 1: captured continent
	 * 2: captured world (game over)
	 * 10: normal capture and eliminated a player
	 * 11: captured continent and eliminated a player */
	bCaptured = true;
	int32 ReturnType = 0;
	TObjectPtr<URiskPlayer> GamePlayer = Players[Turn];
	TObjectPtr<URiskPlayer> PrevOwner = Territory->TerritoryOwner;

	// give the new player the territory and erase the old player's ownership
	Territory->TerritoryOwner = GamePlayer;
	GamePlayer->Territories.Add(Territory);
	PrevOwner->Territories.Remove(Territory);

	// if the old player owns the continent, remove their ownership there
	TObjectPtr<AContinent> Continent = Cast<AContinent>(Territory->GetAttachParentActor());
	if (Continent->ContinentOwner == PrevOwner) {
		Continent->ContinentOwner = nullptr;
		PrevOwner->Continents.Remove(Continent);
	}

	// if the player now owns the continent, add ownership
	if (FindContOwner(Continent) == GamePlayer) {
		Continent->ContinentOwner = GamePlayer;
		GamePlayer->Continents.Add(Continent);
		// check if they have every continent
		if (GamePlayer->Continents.Num() == 6) return 2; // -------
		ReturnType = 1;
	}
	else {
		ReturnType = 0;
	}

	// if the old player is dead, erase him from the game and give a card
	if (PrevOwner->Territories.Num() > 0) {
		return ReturnType;
	}
	else {
		// remove the player from the player list
		delete PrevOwner;
		Players.Remove(PrevOwner);
		if (Players[Turn] != GamePlayer) --Turn;
		return ReturnType + 10;
	}
}

ATerritory* ARiskGameController::FindTerritory(const FString& Name) const
{
	return nullptr;
}

// Input

void ARiskGameController::LeftPress()
{
	CameraMovement += FVector(-1, 0, 0);
}

void ARiskGameController::LeftRelease()
{
	CameraMovement += FVector(1, 0, 0);
}

void ARiskGameController::RightPress()
{
	CameraMovement += FVector(1, 0, 0);
}

void ARiskGameController::RightRelease()
{
	CameraMovement += FVector(-1, 0, 0);
}

void ARiskGameController::UpPress()
{
	CameraMovement += FVector(0, -1, 0);
}

void ARiskGameController::UpRelease()
{
	CameraMovement += FVector(0, 1, 0);
}

void ARiskGameController::DownPress()
{
	CameraMovement += FVector(0, 1, 0);
}

void ARiskGameController::DownRelease()
{
	CameraMovement += FVector(0, -1, 0);
}

void ARiskGameController::MouseUp()
{
	// Rotate army if selected
	if (GameState == EGameState::ArmySelected) {
		SelectedArmy->AddActorWorldRotation(FRotator(0, -30, 0).Quaternion());
	}
	// Zoom if otherwise
	else
	{
		CameraZoom = FMath::Clamp(CameraZoom - 0.1, 0.0, 1.0);
	}
}

void ARiskGameController::MouseDown()
{
	// Rotate army if selected
	if (GameState == EGameState::ArmySelected)
	{
		SelectedArmy->AddActorWorldRotation(FRotator(0, 30, 0).Quaternion());
	}
	// Zoom if otherwise
	else
	{
		CameraZoom = FMath::Clamp(CameraZoom + 0.1, 0.0, 1.0);
	}
}

void ARiskGameController::LeftClick()
{
	if (GameState == EGameState::Neutral) {
		return;
	}

	FHitResult OutHitResult;
	if (!GetHitResultUnderCursor(ECC_Visibility, true, OutHitResult))
	{
		return;
	}
	AActor* Actor = OutHitResult.GetActor();

	if (Actor->GetClass()->GetSuperClass()->GetName() == "Army")
	{
		PreviousGameState = GameState;
		GameState = EGameState::ArmySelected; // Army Selected
		SelectedArmy = Cast<AArmy>(Actor);
		SelectTerritory(Actor->GetAttachParentActor());
	}
	else
	{
		SelectTerritory(Actor);
		switch (GameState)
		{
		case EGameState::PlaceArmies:
			AddArmy(Cast<ATerritory>(Actor), OutHitResult.Location);
			break;
		case EGameState::SelectOne:
			TerrOne = Cast<ATerritory>(Actor);
			break;
		case EGameState::SelectTwo:
			TerrTwo = Cast<ATerritory>(Actor);
			break;
		case EGameState::Occupy:
			if (OutHitResult.GetActor() == TerrTwo)
			{
				TerrOne->RemoveArmy();
				TerrTwo->AddArmy(1, OutHitResult.Location);
			}
			break;
		default:
			break;
		}
		bGameStateUpdate = true;
	}
}

void ARiskGameController::LeftClickRelease()
{
	if (GameState == EGameState::ArmySelected) {
		GameState = PreviousGameState;
	}
}
