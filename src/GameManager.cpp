// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "RiskPlayer.h"
#include "Continent.h"
#include "Territory.h"
#include "Army.h"
#include "Card.h"

// Sets default values
AGameManager::AGameManager() : Turn{ -1 }, TerritoriesOccupied{ 0 }, TerrOne{nullptr}, TerrTwo { nullptr }, Trades{ 0 }, bGotTradeBonus{ false }, AttackArmies{ 0 }, bCaptured{ true }
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // initialize the players
    TArray<FString> Names = GetNames();
    Players.SetNum(Names.Num());
    for (int32 i = 0; i < Names.Num(); ++i) {
        Players[i] = NewObject<URiskPlayer>();
        Players[i]->SetName(Names[i]);
        Players[i]->Armies = 50 - Names.Num() * 5;
    }

    // initialize the board
    // allocate and name the continents
    TArray<TArray<FString>> TerritoryNames =
    { { "Iceland", "Ireland", "Northern Ireland", "United Kingdom",  "France", "Belgium", "Netherlands" },
      { "Portugal", "Spain", "Morocco", "Algeria", "Tunisia", "Italy", "Switzerland"},
      { "Austria", "Hungary", "Slovakia", "Slovenia", "Croatia", "Bosnia and Herzegovina", "Belgrade" },
      { "Romania", "Bulgaria", "Turkey", "Greece", "Moldova", "Albnia", "Macedonia" },
      { "Norway", "Sweden", "Denmark", "Germany", "Poland", "Czech Republic", "Kaliningrad"},
      { "Russia", "Finland", "Latvia", "Lithuania", "Belarus", "Ukraine", "Estonia" } };

    // we need to name the 6 continents [HERE]
    // allocate and name the territories

    // Old initialization code
    /*Continents.SetNum(6); // -------
    for (int32 i = 0; i < Continents.Num(); ++i) {
        Continents[i] = NewObject<AContinent>();
        Continents[i]->NewArmies = 4;
        Continents[i]->Territories.SetNum(7);
        for (int32 j = 0; j < Continents[i]->Territories.Num(); ++j) {
            TObjectPtr<ATerritory> Territory = Continents[i]->Territories[j];
            Territory = NewObject<ATerritory>();
            Territory->Continent = Continents[i];
            Territory->Name = TerritoryNames[i][j];
        }
    }*/


    // we need to set adjeceny lists HERE
    // initialize the DrawPile here
}

TArray<FString> AGameManager::GetNames() {
    return TArray<FString> {"Alice", "Bob", "Charlie", "David"};
}

TArray<int32> AGameManager::RollDice(int32 NumDice) const {
    TArray<int32> Rolls;
    Rolls.SetNum(NumDice);
    for (int32 i = 0; i < NumDice; i++)
        Rolls[i] = std::rand() % 6 + 1;
    return Rolls;
}

void AGameManager::EndTurn() {
    TerrOne = nullptr;
    TerrTwo = nullptr;
    bGotTradeBonus = false;
    AttackArmies = 0;
    if (bCaptured) {
        GiveCard();
        bCaptured = false;
    }
    Turn = (Turn + 1) % Players.Num();
}

bool AGameManager::SetupFinished() const {
    for (TObjectPtr<URiskPlayer> Player : Players) {
        if (Player->Armies > 0) return false;
    }
    return true;
}

// note: this is used for the setup of the game and the mid-game
int32 AGameManager::AddArmy(TObjectPtr<ATerritory> Territory) {
    /* Return Key:
     * 0: added a piece to the territory
     * 1: error, must add piece to unoccupied territory (start of game)
     * 2: error, must add piece to one's own territory */
    TObjectPtr<URiskPlayer> Player = Players[Turn];
    if (TerritoriesOccupied < 42) { // +++++++
        if (Territory->TerritoryOwner != nullptr) return 1;
        Territory->TerritoryOwner = Player;
        Player->Territories.Add(Territory);
        Territory->InitArmies();
        --(Player->Armies);
        TObjectPtr<AContinent> Continent = dynamic_cast<AContinent*, AActor>(Territory->GetAttachParentActor());
        if (FindContOwner(Continent) == Player) {
            Continent->ContinentOwner = Player;
            Player->Continents.Add(Continent);
        }
        ++TerritoriesOccupied;
    }
    else {
        if (Territory->TerritoryOwner != Player) return 2;
        Territory->AddInfantry(1);
        --Player->Armies; // unnecessary after the setup of the game
    }
    return 0;
}

void AGameManager::GiveArmies() {
    TObjectPtr<URiskPlayer> player = Players[Turn];
    int32 newArmies = player->Territories.Num() / 3;
    for (TObjectPtr<AContinent> i : player->Continents) newArmies += 4;
    if (3 < newArmies) player->Armies += newArmies;
    else player->Armies += 3;
}

int32 AGameManager::TradeArmies(TObjectPtr<ATerritory> Territory, TCHAR startType, TCHAR endType) {
    /* Return Key:
     * 0: traded armies
     * 1: error, do not own territory
     * 2: error, trading pieces for the same pieces (startType == endType)
     * 3: error, do not own enough pieces to trade */
    TObjectPtr<URiskPlayer> Player = Players[Turn];
    if (Territory->TerritoryOwner != Player) return 1;
    if (startType == endType) return 2;
    switch (startType) {
    case 'i':
        switch (endType) {
        case 'c':
            if (Territory->GetInfantry() < 5) return 3;
            Territory->AddInfantry(-5);
            Territory->AddCavalry(1);
            break;
        case 'a':
            if (Territory->GetInfantry() < 10) return 3;
            Territory->AddInfantry(-10);
            Territory->AddArtillery(1);
            break;
        }
        break;
    case 'c':
        switch (endType) {
        case 'i':
            Territory->AddCavalry(-1);
            Territory->AddInfantry(5);
            break;
        case 'a':
            if (Territory->GetCavalry() < 2) return 3;
            Territory->AddCavalry(2);
            Territory->AddArtillery(1);
            break;
        }
        break;
    case 'a':
        switch (endType) {
            Territory->AddArtillery(-1);
        case 'i':
            Territory->AddInfantry(10);
            break;
        case 'c':
            Territory->AddCavalry(2);
            break;
        }
        break;
    }
    return 0;
}

int32 AGameManager::SetAttack(TObjectPtr<ATerritory> Start, TObjectPtr<ATerritory> End) {
    /* Return Key:
     * 0: set start and end territories for attacking
     * 1: error, do not own starting territory
     * 2: error, player owns ending territory
     * 3: error, territories are not connected
     * 4: error, do not have enough armies on starting territory */
    TObjectPtr<URiskPlayer> player = Players[Turn];
    if (Start->TerritoryOwner != player) return 1;
    if (End->TerritoryOwner == player) return 2;
    if (!AreConnectedTerritories(Start, End)) return 3;
    if (Start->GetArmies() < 2) return 4;
    TerrOne = Start;
    TerrTwo = End;
    return 0;
}

int32 AGameManager::Attack(int32 PlayerOneDice, int32 PlayerTwoDice) {
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
    if (TerrOne->GetInfantry() < PlayerOneDice + 1) return 5;
    if (PlayerOneDice < 1 || PlayerTwoDice > 2) return 6;
    if (TerrTwo->GetInfantry() < PlayerTwoDice) return 7;

    // attack
    TArray<int32> PlayerOneRoll = RollDice(PlayerOneDice);
    TArray<int32> PlayerTwoRoll = RollDice(PlayerTwoDice);

    // sort the rolls
    PlayerOneRoll.Sort();
    PlayerTwoRoll.Sort();

    // compare the two highest rolls
    if (PlayerOneRoll[PlayerOneDice - 1] > PlayerTwoRoll[PlayerTwoDice - 1]) {
        // player one wins the battle
        TerrTwo->AddInfantry(-1);
    }
    else {
        // player two wins the battle
        TerrOne->AddInfantry(-1);
    }
    // see if there is another battle to compare and compare the second highest
    if (PlayerOneDice > 1 && PlayerTwoDice > 1) {
        if (PlayerOneRoll[PlayerOneDice - 2]
            > PlayerTwoRoll[PlayerTwoDice - 2]) {
            // player one wins the battle
            TerrTwo->AddInfantry(-1);
        }
        else {
            // player two wins the battle
            TerrOne->AddInfantry(-1);
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

int32 AGameManager::OccupyTerritory(const TArray<TObjectPtr<AArmy>> Armies) {
    /* Return Key:
     * 0: occupied the territory
     * 1: error, need as many armies as dice rolled to capture (AttackArmies) */
    int32 MovedArmies = 0;
    for (TObjectPtr<AArmy> Army : Armies) {
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
    Fortify(Armies);
    AttackArmies = 0;
    return 0;
}

int32 AGameManager::SetFortify(TObjectPtr<ATerritory> Start, TObjectPtr<ATerritory> End) {
    /* Return Key:
     * 0: set start and end territories for fortify
     * 1: error, do not own starting territory
     * 2: error, do not own ending territory
     * 3: error, territories are not connected */
    TObjectPtr<URiskPlayer> player = Players[Turn];
    if (Start->TerritoryOwner != player) return 1;
    if (End->TerritoryOwner != player) return 2;
    if (!AreConnectedTerritories(Start, End)) return 3;
    TerrOne = Start;
    TerrTwo = End;
    return 0;
}

void AGameManager::Fortify(const TArray<TObjectPtr<AArmy>> Armies) {
    int32 Value;
    for (TObjectPtr<AArmy> Army : Armies) {
        // find the value of the piece and change the armies on territories
        FString ArmyName = Army->GetClass()->GetName();
        if (ArmyName == "BP_Infantry_C") {
            Value = 1;
            TerrOne->AddInfantry(-1);
            TerrTwo->AddInfantry(1);
        }
        else if (ArmyName == "BP_Cavalry_C") {
            Value = 5;
            TerrOne->AddCavalry(-1);
            TerrTwo->AddCavalry(1);
        }
        else if (ArmyName == "BP_Artillery_C") {
            Value = 10;
            TerrOne->AddArtillery(-1);
            TerrTwo->AddArtillery(1);
        } else {
            Value = 1;
        }
    }
}

void AGameManager::GiveCard() {
    // give the player a card and remove it from the DrawPile
    Players[Turn]->Cards.Add(DrawPile[DrawPile.Num() - 1]);
    DrawPile.Pop();
    // if the deck is empty, make the DiscardPile become the DrawPile
    if (DrawPile.Num() == 0) {
        DrawPile = DiscardPile;
        DiscardPile.Empty();
        // shuffle
        int32 Temp;
        TObjectPtr<UCard> Temp2;
        for (int32 i = 0; i < DrawPile.Num() - 1; i++) {
            Temp = std::rand() % (DrawPile.Num() - i) + i;
            Temp2 = DrawPile[i];
            DrawPile[i] = DrawPile[Temp];
            DrawPile[Temp] = Temp2;
        }
    }
}

int32 AGameManager::TradeCards(const TArray<int32>& CardsInd) {
    /* Return Key:
     * 0: traded in cards
     * 1: error, cards do not form a set that can be traded */
    TObjectPtr<URiskPlayer> Player = Players[Turn];
    if (!IsValidTrade(CardsInd)) return 1;
    // if they own the territory, then 2 extra armies are placed there
    // however, this happens once per turn
    if (!bGotTradeBonus) {
        for (int32 i = 0; i < 3; i++) {
            TObjectPtr<ATerritory> CardTerr = FindTerritory(Player->Cards[CardsInd[i]]
                ->Territory);
            if (CardTerr == nullptr) continue; // wild card; has no territory
            if (CardTerr->TerritoryOwner == Player) {
                bGotTradeBonus = true;
                CardTerr->AddInfantry(2);
                break;
            }
        }
    }
    // give armies for the trade
    switch (Trades) {
    case 0:
        Player->Armies += 4;
        break;
    case 1:
        Player->Armies += 6;
        break;
    case 2:
        Player->Armies += 8;
        break;
    case 3:
        Player->Armies += 10;
        break;
    case 4:
        Player->Armies += 12;
        break;
    case 5:
        Player->Armies += 15;
        break;
    default:
        Player->Armies += 5 * Trades - 10;
        break;
    }
    // put the cards back in DiscardPile and remove them from the player's hand
    DiscardPile.Add(Player->Cards[CardsInd[0]]);
    DiscardPile.Add(Player->Cards[CardsInd[1]]);
    DiscardPile.Add(Player->Cards[CardsInd[2]]);
    Player->Cards.RemoveAt(CardsInd[0]);
    Player->Cards.RemoveAt(CardsInd[1]);
    Player->Cards.RemoveAt(CardsInd[2]);
    return 0;
}

// returns nullptr if there is no owner
TObjectPtr<URiskPlayer> AGameManager::FindContOwner(const TObjectPtr<AContinent> continent) const {
    TArray<AActor *> Territories;
    continent->GetAttachedActors(Territories);
    TObjectPtr<URiskPlayer> OwningPlayer = Cast<ATerritory>(Territories[0])->TerritoryOwner;
    for (AActor* Territory : Territories) {
        if (Cast<ATerritory>(Territory)->TerritoryOwner != OwningPlayer) return nullptr;
    }
    return OwningPlayer;
}

bool AGameManager::AreConnectedTerritories(const TObjectPtr<ATerritory> Start, const TObjectPtr<ATerritory> End) const {
    return Start->NearTerritories.Contains(End);
}

int32 AGameManager::CaptureTerritory(TObjectPtr<ATerritory> Territory) {
    /* Return Key:
     * 0: normal capture
     * 1: captured continent
     * 2: captured world (game over)
     * 10: normal capture and eliminated a player
     * 11: captured continent and eliminated a player */
    bCaptured = true;
    int32 returnType = 0;
    TObjectPtr<URiskPlayer> player = Players[Turn];
    TObjectPtr<URiskPlayer> prevOwner = Territory->TerritoryOwner;

    // give the new player the territory and erase the old player's ownership
    Territory->TerritoryOwner = player;
    player->Territories.Add(Territory);
    prevOwner->Territories.Remove(Territory);

    // if the old player owns the continent, remove their ownership there
    TObjectPtr<AContinent> Continent = Cast<AContinent>(Territory->GetAttachParentActor());
    if (Continent->ContinentOwner == prevOwner) {
        Continent->ContinentOwner = nullptr;
        prevOwner->Continents.Remove(Continent);
    }

    // if the player now owns the continent, add ownership
    if (FindContOwner(Continent) == player) {
        Continent->ContinentOwner = player;
        player->Continents.Add(Continent);
        // check if they have every continent
        if (player->Continents.Num() == 6) return 2; // -------
        returnType = 1;
    }
    else {
        returnType = 0;
    }

    // if the old player is dead, erase him from the game and give a card
    if (prevOwner->Territories.Num() > 0) {
        return returnType;
    }
    else {
        // remove the player from the player list
        delete prevOwner;
        Players.Remove(prevOwner);
        if (Players[Turn] != player) --Turn;
        return returnType + 10;
    }
}

bool AGameManager::IsValidTrade(const TArray<int32>& CardsInd) const {
    // check for a wild card, matching armies, or disjoint armies
    TObjectPtr<URiskPlayer> player = Players[Turn];
    FString Card1 = player->Cards[CardsInd[0]]->Army;
    FString Card2 = player->Cards[CardsInd[1]]->Army;
    FString Card3 = player->Cards[CardsInd[2]]->Army;
    if ((Card1 != "wild" && Card2 != "wild" && Card3 != "wild") &&
        (Card1 != Card2 || Card1 != Card3 || Card2 != Card3) &&
        (Card1 == Card2 || Card1 == Card3 || Card2 == Card3)) {
        return false;
    }
    return true;
}

TObjectPtr<ATerritory> AGameManager::FindTerritory(const FString& name) const {
    for (TObjectPtr<AContinent> Continent : Continents) {
        TArray<AActor*> Territories;
        Continent->GetAttachedActors(Territories);
        for (AActor* Territory : Territories) {
            if (Territory->GetName() == name) {
                return Cast<ATerritory>(Territory);
            }
        }
    }
    // should only happen for wild cards
    return nullptr;
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();	
}
