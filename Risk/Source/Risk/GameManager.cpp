// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "RiskPlayer.h"
#include "Continent.h"
#include "Territory.h"
#include "Card.h"

// Sets default values
AGameManager::AGameManager(const TArray<FString>& names) : Turn{ -1 }, TerritoriesOccupied{ 0 }, TerrOne{nullptr}, TerrTwo { nullptr }, Trades{ 0 }, bGotTradeBonus{ false }, AttackArmies{ 0 }, bCaptured{ true }
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // initialize the players
    Players.SetNum(names.Num());
    for (int8 i = 0; i < names.Num(); ++i) {
        Players[i] = NewObject<URiskPlayer>();
        Players[i]->SetName(names[i]);
        Players[i]->Armies = 50 - names.Num() * 5;
    }

    // initialize the board
    // allocate and name the continents
    Continents.SetNum(6); // -------
    for (TObjectPtr<UContinent> Continent : Continents) {
        Continent = NewObject<UContinent>();
        Continent->NewArmies = 4;
    }

    // create array of names
    TArray<TArray<FString>> Names =
    { { "Iceland", "Ireland", "Northern Ireland", "United Kingdom",  "France", "Belgium", "Netherlands" },
      { "Portugal", "Spain", "Morocco", "Algeria", "Tunisia", "Italy", "Switzerland"},
      { "Austria", "Hungary", "Slovakia", "Slovenia", "Croatia", "Bosnia and Herzegovina", "Belgrade" },
      { "Romania", "Bulgaria", "Turkey", "Greece", "Moldova", "Albnia", "Macedonia" },
      { "Norway", "Sweden", "Denmark", "Germany", "Poland", "Czech Republic", "Kaliningrad"},
      { "Russia", "Finland", "Latvia", "Lithuania", "Belarus", "Ukraine", "Estonia" } };

    // we need to name the 6 continents [HERE]
    // allocate and name the territories
    for (int8 i = 0; i < Continents.Num(); ++i) {
        auto Continent = Continents[i];
        Continent->Territories.SetNum(7);
        for (int8 j = 0; j < Continent->Territories.Num(); ++j) {
            auto Territory = Continent->Territories[j];
            Territory = NewObject<ATerritory>();
            Territory->Continent = Continent;
        }
    }

    // we need to set adjeceny lists HERE
    // initialize the DrawPile here
}

TArray<int8> AGameManager::RollDice(int8 NumDice) const {
    TArray<int8> Rolls;
    Rolls.SetNum(NumDice);
    for (int8 i = 0; i < NumDice; i++)
        Rolls[i] = std::rand() % 6 + 1;
    return Rolls;
}

void AGameManager::SetTurn(int16 NewTurn) { Turn = NewTurn; }

int16 AGameManager::GetTurn() const { return Turn; }

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
int8 AGameManager::AddArmy(TObjectPtr<ATerritory> Territory) {
    /* Return Key:
     * 0: added a piece to the territory
     * 1: error, must add piece to unoccupied territory (start of game)
     * 2: error, must add piece to one's own territory */
    TObjectPtr<URiskPlayer> Player = Players[Turn];
    if (TerritoriesOccupied < 42) { // +++++++
        if (Territory->Owner != nullptr) return 1;
        Territory->Owner = Player;
        Player->Territories.Add(Territory);
        Territory->Armies = 1;
        Territory->Infantry = 1;
        --(Player->Armies);
        if (FindContOwner(Territory->Continent) == Player) {
            Territory->Continent->Owner = Player;
            Player->Continents.Add(Territory->Continent);
        }
        ++TerritoriesOccupied;
    }
    else {
        if (Territory->Owner != Player) return 2;
        ++Territory->Armies;
        ++Territory->Infantry;
        --Player->Armies; // unnecessary after the setup of the game
    }
    return 0;
}

void AGameManager::GiveArmies() {
    TObjectPtr<URiskPlayer> player = Players[Turn];
    int8 newArmies = player->Territories.Num() / 3;
    for (TObjectPtr<UContinent> i : player->Continents) newArmies += i->NewArmies;
    if (3 < newArmies) player->Armies += newArmies;
    else player->Armies += 3;
}

int8 AGameManager::TradeArmies(TObjectPtr<ATerritory> Territory, TCHAR startType, TCHAR endType) {
    /* Return Key:
     * 0: traded armies
     * 1: error, do not own territory
     * 2: error, trading pieces for the same pieces (startType == endType)
     * 3: error, do not own enough pieces to trade */
    TObjectPtr<URiskPlayer> player = Players[Turn];
    if (Territory->Owner != player) return 1;
    if (startType == endType) return 2;
    switch (startType) {
    case 'i':
        switch (endType) {
        case 'c':
            if (Territory->Infantry < 5) return 3;
            Territory->Infantry -= 5;
            ++Territory->Cavalry;
            break;
        case 'a':
            if (Territory->Infantry < 10) return 3;
            Territory->Infantry -= 10;
            ++Territory->Artillery;
            break;
        }
        break;
    case 'c':
        switch (endType) {
        case 'i':
            --Territory->Cavalry;
            Territory->Infantry += 5;
            break;
        case 'a':
            if (Territory->Cavalry < 2) return 3;
            Territory->Cavalry -= 2;
            ++Territory->Artillery;
            break;
        }
        break;
    case 'a':
        switch (endType) {
            --Territory->Artillery;
        case 'i':
            Territory->Infantry += 10;
            break;
        case 'c':
            Territory->Cavalry += 2;
            break;
        }
        break;
    }
    return 0;
}

int8 AGameManager::SetAttack(TObjectPtr<ATerritory> Start, TObjectPtr<ATerritory> End) {
    /* Return Key:
     * 0: set start and end territories for attacking
     * 1: error, do not own starting territory
     * 2: error, player owns ending territory
     * 3: error, territories are not connected
     * 4: error, do not have enough armies on starting territory */
    TObjectPtr<URiskPlayer> player = Players[Turn];
    if (Start->Owner != player) return 1;
    if (End->Owner == player) return 2;
    if (!AreConnectedTerritories(Start, End)) return 3;
    if (Start->Armies < 2) return 4;
    TerrOne = Start;
    TerrTwo = End;
    return 0;
}

int8 AGameManager::Attack(int8 playerOneDice, int8 playerTwoDice) {
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
    if (playerOneDice < 1 || playerOneDice > 3) return 4;
    if (TerrOne->Infantry < playerOneDice + 1) return 5;
    if (playerOneDice < 1 || playerTwoDice > 2) return 6;
    if (TerrTwo->Infantry < playerTwoDice) return 7;
    // attack
    TArray<int8> playerOneRoll = RollDice(playerOneDice);
    TArray<int8> playerTwoRoll = RollDice(playerTwoDice);
    // sort the rolls
    playerOneRoll.Sort();
    playerTwoRoll.Sort();
    // compare the two highest rolls
    if (playerOneRoll[playerOneDice - 1] > playerTwoRoll[playerTwoDice - 1]) {
        // player one wins the battle
        --TerrTwo->Infantry;
        --TerrTwo->Armies;
    }
    else {
        // player two wins the battle
        --TerrOne->Infantry;
        --TerrOne->Armies;
    }
    // see if there is another battle to compare and compare the second highest
    if (playerOneDice > 1 && playerTwoDice > 1) {
        if (playerOneRoll[playerOneDice - 2]
            > playerTwoRoll[playerTwoDice - 2]) {
            // player one wins the battle
            --TerrTwo->Infantry;
            --TerrTwo->Armies;
        }
        else {
            // player two wins the battle
            --TerrOne->Infantry;
            --TerrOne->Armies;
        }
    }
    // if the opposing player has no more armies, then capture his territory
    if (TerrTwo->Armies == 0) {
        int8 captureVal = CaptureTerritory(TerrTwo);
        if (captureVal == 2) return 3;
        // note: the player needs to move a piece to the captured territory
        AttackArmies = playerOneDice;
        return captureVal + 1;
    }
    return 0;
}

int8 AGameManager::OccupyTerritory(const TArray<TCHAR>& Armies) {
    /* Return Key:
     * 0: occupied the territory
     * 1: error, need as many armies as dice rolled to capture (AttackArmies) */
    int8 movedArmies = 0;
    for (TCHAR piece : Armies) {
        switch (piece) {
        case 'i':
            movedArmies += 1;
            break;
        case 'c':
            movedArmies += 5;
            break;
        case 'a':
            movedArmies += 10;
            break;
        }
    }
    if (movedArmies < AttackArmies) return 1;
    Fortify(Armies);
    AttackArmies = 0;
    return 0;
}

int8 AGameManager::SetFortify(TObjectPtr<ATerritory> Start, TObjectPtr<ATerritory> End) {
    /* Return Key:
     * 0: set start and end territories for fortify
     * 1: error, do not own starting territory
     * 2: error, do not own ending territory
     * 3: error, territories are not connected */
    TObjectPtr<URiskPlayer> player = Players[Turn];
    if (Start->Owner != player) return 1;
    if (End->Owner != player) return 2;
    if (!AreConnectedTerritories(Start, End)) return 3;
    TerrOne = Start;
    TerrTwo = End;
    return 0;
}

void AGameManager::Fortify(const TArray<TCHAR>& Pieces) {
    int8 value;
    for (int8 i : Pieces) {
        // find the value of the piece and change the armies on territories
        switch (i) {
        case 'i':
            value = 1;
            TerrOne->Infantry -= 1;
            TerrTwo->Infantry += 1;
            break;
        case 'c':
            value = 5;
            TerrOne->Cavalry -= 1;
            TerrTwo->Cavalry += 1;
            break;
        case 'a':
            value = 10;
            TerrOne->Artillery -= 1;
            TerrTwo->Artillery += 1;
            break;
            // this should never run
        default:
            value = 1;
            break;
        }
        TerrOne->Armies -= value;
        TerrTwo->Armies += value;
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
        int8 temp;
        Card* temp2;
        for (int8 i = 0; i < DrawPile.Num() - 1; i++) {
            temp = std::rand() % (DrawPile.Num() - i) + i;
            temp2 = DrawPile[i];
            DrawPile[i] = DrawPile[temp];
            DrawPile[temp] = temp2;
        }
    }
}

int8 AGameManager::TradeCards(const TArray<int8>& CardsInd) {
    /* Return Key:
     * 0: traded in cards
     * 1: error, cards do not form a set that can be traded */
    TObjectPtr<URiskPlayer> Player = Players[Turn];
    if (!IsValidTrade(CardsInd)) return 1;
    // if they own the territory, then 2 extra armies are placed there
    // however, this happens once per turn
    if (!bGotTradeBonus) {
        for (int8 i = 0; i < 3; i++) {
            TObjectPtr<ATerritory> CardTerr = FindTerritory(Player->Cards[CardsInd[i]]
                ->Territory);
            if (CardTerr == nullptr) continue; // wild card; has no territory
            if (CardTerr->Owner == Player) {
                bGotTradeBonus = true;
                CardTerr->Armies += 2;
                CardTerr->Infantry += 2;
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
TObjectPtr<URiskPlayer> AGameManager::FindContOwner(const TObjectPtr<UContinent> continent) const {
    TObjectPtr<URiskPlayer> owner = continent->Territories[0]->Owner;
    for (TObjectPtr<ATerritory> i : continent->Territories) {
        if (i->Owner != owner) return nullptr;
    }
    return owner;
}

bool AGameManager::AreConnectedTerritories(const TObjectPtr<ATerritory> start, const TObjectPtr<ATerritory> end) const {
    for (TObjectPtr<ATerritory> terr : start->NearTerritories) {
        if (terr == end) return true;
    }
    return false;
}

int8 AGameManager::CaptureTerritory(TObjectPtr<ATerritory> Territory) {
    /* Return Key:
     * 0: normal capture
     * 1: captured continent
     * 2: captured world (game over)
     * 10: normal capture and eliminated a player
     * 11: captured continent and eliminated a player */
    bCaptured = true;
    int8 returnType = 0;
    TObjectPtr<URiskPlayer> player = Players[Turn];
    TObjectPtr<URiskPlayer> prevOwner = Territory->Owner;

    // give the new player the territory and erase the old player's ownership
    Territory->Owner = player;
    player->Territories.Add(Territory);
    prevOwner->Territories.Remove(Territory);

    // if the old player owns the continent, remove their ownership there
    if (Territory->Continent->Owner == prevOwner) {
        Territory->Continent->Owner = nullptr;
        prevOwner->Continents.Remove(Territory->Continent);
    }

    // if the player now owns the continent, add ownership
    if (FindContOwner(Territory->Continent) == player) {
        Territory->Continent->Owner = player;
        player->Continents.Add(Territory->Continent);
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

bool AGameManager::IsValidTrade(const TArray<int8>& CardsInd) const {
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
    for (TObjectPtr<UContinent> Cont : Continents) {
        for (TObjectPtr<ATerritory> Terr : Cont->Territories) {
            if (Terr->Name == name) return Terr;
        }
    }
    // should only happen for wild cards
    return nullptr;
}

AGameManager::~AGameManager()
{
	Super::~AActor();
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