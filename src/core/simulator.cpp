#include "simulator.h"
#include "physics.h"
#include "switches.h"
#include "logger.h"
#include "viewer_iface.h"
#include "level.h"
#include "util.h"
#include <iostream>
using namespace std;

//files' paths in the constants and no of emergency trips
const char saveFile[] = "out/savegame.txt";
const char summaryFile[] = "out/game_summary.txt";
const int cardTrips = 3;

// Modular simulation loop using primitive variables

// Rooms and checks applied on the basis of movement through the rooms
bool handleMoveCommand(int targetFloor, int targetRoom, int& currentFloor, int& currentRoom, int& energy, int& time, const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR-1], const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, int& emergencyTripsLeft, bool lift2Operational){
    if(targetFloor < 1 || targetFloor > NUM_FLOORS || targetRoom < 1 || targetRoom > ROOMS_PER_FLOOR){
        cout << "The room doesn't exist" << endl;
        return false;
    }
    if(targetFloor == currentFloor && targetFloor == targetRoom){
        cout << "You are already there." << endl;
        return false;
    }

    char link;
    if(!isDirectlyConnected(currentFloor, currentRoom, targetFloor, targetRoom, horizontalConn, link)){
        cout << "You cannot reach that room from here" << endl;
        return false;
    }

    if(!canTraverse(targetFloor, targetRoom, link, inventory, inventoryCount, emergencyTripsLeft, lift2Operational)){
        if(link == CONN_SOLID_WALL){
            cout << "A solid wall is in the way." << endl;
        }else if(link == CONN_LOCKED_DOOR){
            cout << "The door is locked, master key required.";
        }else{
            cout << "Lift 2 is not working. Install the fuse and get the lift pass." << endl;
            cout << "Or use the Emergency Access Card.";
        }
        return false;
    }

    if(link == '2' && !lift2Operational){
        emergencyTripsLeft--;
        cout << "Emergency Card Used. Trips left: " << emergencyTripsLeft << endl;
    }

    if(link == CONN_WINDOW){
        applyResourceDelta(energy, time, COST_MOVE_WINDOW_ENERGY, COST_MOVE_WINDOW_TIME);
    }else if(link == '1' || link == '2'){
        applyResourceDelta(energy, time, COST_MOVE_LIFT_ENERGY, COST_MOVE_LIFT_TIME);
    }else{
        applyResourceDelta(energy, time, COST_MOVE_ORDINARY_ENERGY, COST_MOVE_ORDINARY_TIME);
    }

    currentFloor = targetFloor;
    currentRoom = targetRoom;
    cout << "You moved to F" << targetFloor << "-R" << targetRoom << endl;
    return true;

}

//it costs 1 energy and 1 time, looking for clue digits, found digit gives 10 points
    bool handleInspectCommand(int currentFloor, int currentRoom, int& energy, int& time, const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCount, bool objectives[NUM_OBJECTIVES], int& score){
        applyResourceDelta(energy, time, COST_INSPECT_ENERGY, COST_INSPECT_TIME);
        renderRoomDetails(currentFloor, currentRoom, roomEntities, entityCount);

        for(int i=0; i<entityCount; i++){
            char symbol = roomEntities[currentFloor-1][currentRoom-1][i];
            int clue = -1;
            if(symbol == ENT_CLUE_4) clue = OBJ_CLUE_4;
            if(symbol == ENT_CLUE_2) clue = OBJ_CLUE_2;
            if(symbol == ENT_CLUE_7) clue = OBJ_CLUE_7;
            
            if(clue != -1 && !objectives[clue]){
                objectives[clue] = true;
                applyScoreDelta(score, SCORE_CLUE_FOUND);
                cout << "Your found a clue digit: " << symbol << " (+" << SCORE_CLUE_FOUND << " points)" << endl;
            }
        }
        return true;
    }


//item present in room, cat/person can notbe picked up and cabinet should be unlocked. Also energy pack, toolkit and token effect immediately and not stored, everything else goes into inventory if space avaliable.
bool handleCollectCommand(char itemSymbol, int currentFloor, int currentRoom, int &energy, int &time, int& score, char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int& entityCount, char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, bool objectives[NUM_OBJECTIVES]){

    bool inRoom = false;
    for (int i = 0; i < entityCount; i++) {
        if (roomEntities[currentFloor - 1][currentRoom - 1][i] == itemSymbol) {
            inRoom = true;
        }
    }

    if (!inRoom) {
        cout << "The item is not present in the room." << endl;
        return false;
    }

    if (itemSymbol == ENT_CLUE_4 || itemSymbol == ENT_CLUE_2 || itemSymbol == ENT_CLUE_7) {
        cout << "Clues are written on the wall." << endl;
        return false;
    }

    if (itemSymbol == ENT_PASSENGER || itemSymbol == ENT_CAT || itemSymbol == ENT_CONTROL_PANEL){
        cout << "You cannot pick up this item." << endl;
        return false;
    }

    if (itemSymbol == ENT_CABINET_TRAIN_KEY && !objectives[OBJ_CABINET_UNLOCKED]) {
        cout << "The cabinet is locked. You need to unlock it with code first." << endl;
        return false;
    }

    bool usedAtOnce = itemSymbol == ENT_ENERGY_PACK || itemSymbol == ENT_EMERGENCY_TOOLKIT || itemSymbol == ENT_OBSERVATION_TOKEN;

    if(!usedAtOnce && inventoryCount >= MAX_INVENTORY_CAPACITY) {
        cout << "Your inventory is full. Cannot collect more items." << endl;
        return false;
    }

    applyResourceDelta(energy, time, COST_ITEM_PICKUP_ENERGY, COST_ITEM_PICKUP_TIME);

    for (int i = 0; i < entityCount; i++) {
        if (roomEntities[currentFloor - 1][currentRoom - 1][i] == itemSymbol) {
            for (int j = i; j < entityCount - 1; j++) {
                roomEntities[currentFloor - 1][currentRoom - 1][j] = roomEntities[currentFloor - 1][currentRoom - 1][j + 1];
            }
            entityCount--;
            break;
        }
    }

    if (usedAtOnce) {

        if (itemSymbol == ENT_ENERGY_PACK) applyResourceDelta(energy, time, BONUS_ENERGY_PACK, 0);

        if (itemSymbol == ENT_EMERGENCY_TOOLKIT) applyResourceDelta(energy, time, BONUS_EMERGENCY_TOOLKIT, 0);

        if (itemSymbol == ENT_OBSERVATION_TOKEN) applyResourceDelta(energy, time, 0, BONUS_OBSERVATION_TOKEN);

        cout << "You used the " << getEntityName(itemSymbol) << ".\n" ;
        return true;

        }

        addItem(inventory, inventoryCount, itemSymbol);

        if (itemSymbol == ENT_MASTER_KEY) {
            objectives[OBJ_MASTER_KEY] = true;
            applyScoreDelta(score, SCORE_KEY_COLLECT);
        }

        else if (itemSymbol == ENT_FUSE){
            objectives[OBJ_FUSE_COLLECTED] = true;
        }

        else if (itemSymbol == ENT_CAT_CARRIER){
            objectives[OBJ_CAT_CARRIER] = true;
        }

        else if (itemSymbol == ENT_CABINET_TRAIN_KEY){
            objectives[OBJ_TRAIN_KEY] = true;
            applyScoreDelta(score, SCORE_TRAIN_KEY);
        }

        if (objectives[OBJ_FUSE_INSTALLED] && objectives[OBJ_LIFT_PASS]){
            objectives[OBJ_LIFT2_ACTIVE] = true;
        }
        
        if (itemSymbol == ENT_CABINET_TRAIN_KEY) cout << "You took the Train keyyy!\n";

        else cout << "You picked up the " << getEntityName(itemSymbol) << ".\n";

        return true;
}


bool handleUseCommand(char itemSymbol, int currentFloor, int currentRoom, int& energy, int& time, int& score, char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, bool objectives[NUM_OBJECTIVES]){

    if (hasItem(inventory, inventoryCount,  itemSymbol)){
        cout << "You do not hav that item in your inventory." << endl;
        return false;
    }

    if (itemSymbol == ENT_MEDICAL_KIT && currentFloor == 4 && currentRoom == 1 && !objectives[OBJ_PASSENGER_RESCUED]){
        applyResourceDelta(energy, time, COST_ITEM_USE_ENERGY, COST_ITEM_USE_TIME);
        objectives[OBJ_PASSENGER_RESCUED] = true;
        applyScoreDelta(score, SCORE_RESCUE_PASSENGER);
        cout << "You treated the passenger. Passenger rescued...!\n";
        return true;
    }

    if (itemSymbol == ENT_FUSE && currentFloor == 3 && currentRoom == 4){
        applyResourceDelta(energy, time, COST_ITEM_USE_ENERGY, COST_ITEM_USE_TIME);
        removeItem(inventory, inventoryCount, ENT_CAT_CARRIER);
        objectives[OBJ_CAT_SECURED] = true;
        cout << "You have the cat in the carrir. Cat secured...!\n";
        return true;
    }

    if (itemSymbol == ENT_FUSE && currentFloor == 3 && currentRoom == 4){
        applyResourceDelta(energy, time, COST_ITEM_USE_ENERGY, COST_ITEM_USE_TIME);
        installFuseControlPanel(inventory, inventoryCount, objectives, score);
        cout << "Fuse installed in the control Panel.\n";
        if (objectives[OBJ_LIFT_PASS]){
            objectives[OBJ_LIFT2_ACTIVE] = true;
            cout << "Lift 2 is now working!\n";
        }
        else {
            cout << "Collect the Lift Pass to use lift 2.\n";
        }
        return true;
    }

    if (itemSymbol == ENT_CABINET_TRAIN_KEY){
        if (currentFloor != 5 || currentRoom != 2){
            cout << "The train is at the boarding platform(F5 R2).\n";
            return false;
        }
        
        if (!objectives[OBJ_PASSENGER_RESCUED] || !objectives[OBJ_CAT_SECURED]){
            cout << "Rescue the passenger and secure the cat first.\n";
            return false;
        }

        applyResourceDelta(energy, time , COST_ITEM_USE_ENERGY, COST_ITEM_USE_TIME);
        objectives[OBJ_TRAIN_PREPARED] = true;
        objectives[OBJ_MISSION_WON] = true;
        applyScoreDelta(score, SCORE_PREPARE_TRAIN + SCORE_MISSION_VICTORY);
        cout << "The train is ready, Everyone on board!!.\n";
        return true;
    }

    cout << "Nothing happens, try using it in the correct room.\n";
    return false;

}

//check if cabinet in the room and look for ^ item, that has not been opened and all 3 clues are found and the code is correct
bool handleUnlockCabinetCommand(const char* code, int currentFloor, int currentRoom, bool objectives[NUM_OBJECTIVES], int& score, char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int&entityCount){

    bool cabinetHere = false;
    for (int i =0; i < entityCount; i++){
        if (roomEntities[currentFloor - 1][currentRoom -1][i] == ENT_CABINET_TRAIN_KEY){
            cabinetHere = true;
        }
    }
    if (objectives[OBJ_CABINET_UNLOCKED]){
        cout << "The cabinet is already openn.\n";
        return false;
   }
    
    if (!cabinetHere){
        cout << "The cabinet is in here in F5 R1.\n";
        return false;
   }

   if (!objectives[OBJ_CLUE_4] || !objectives[OBJ_CLUE_2] || !objectives[OBJ_CLUE_7]){
        cout << "You have not found all 3 clue digits yet.\n";
        return false;
   }

   if (!verifyCabinetCode(code, objectives)){
        cout << "Wrong code.\n";
        return false;
   }

   objectives[OBJ_CABINET_UNLOCKED] = true;
   applyScoreDelta(score, SCORE_CABINET_UNLOCK);
   cout << "CAbinet Unlocked! Collect the train key( ^ ).\n";
   return true;
}


//prints victory or failure message and returns true if game over
bool checkEndConditions(int energy,int time, const bool objectives[NUM_OBJECTIVES]){
    if (objectives[OBJ_MISSION_WON]){
        printHeader("Victory!!! The last train leaves safely...");
        return true;
    }

    if (energy <= 0){
        printHeader("Mission Failed!!! You ran out of energy...");
        return true;
    }

    if (time <= 0){
        printHeader("Mission Failed!!! You ran out of time...");
        return true;
    }

    return false;
}

void runSimulation(int& playerFloor, int& playerRoom, int& energy, int& time, int& score, char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft, char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR-1], char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]){
    char startChoice = '1';
    cout << "1. New game  2. Load saved game\n";
    cout << "Choice: ";
    cin >> startChoice;
    if(cin.fail()) return;

    bool loaded = false;
    if(startChoice == '2'){
        loaded = loadGameStateFromFile(saveFile, playerFloor, playerRoom, energy, time, score, inventory, inventoryCount, objectives, emergencyTripsLeft, roomEntities, entityCounts);
        if (!loaded) cout << "No saved game found. Starting a new game." << endl;
    }

    while(!loaded){
        char floorChar = '0';
        char roomChar = '0';
        cout << "Start floor (1-5): ";
        cin >> floorChar;
        cout << "Start room (1-4): ";
        cin >> roomChar;
        if(cin.fail()) return;
        playerFloor = floorChar - '0';
        playerRoom = roomChar -'0';
        if(validateStartingRoom(playerFloor, playerRoom, horizontalConn)) break;
        cout << "You cannot start there. Pick another room." << endl;
    }

    while(true) {
        renderAsciiBuilding(playerFloor, playerRoom);
        renderDashboard(playerFloor, playerRoom, energy, time, score, inventory, inventoryCount, emergencyTripsLeft);
        cout << "\n1.Move  2.Inspect  3.Collect  4.Use Item  5.Unlock cabinet\n6.Objectives  7.Save  0.Save & Quit" << endl;
        cout << "Choice: ";
        char choice = '0';
        cin >> choice;
        if(cin.fail()) choice = '0';

        int f = playerFloor - 1;
        int r = playerRoom - 1;

        switch (choice) {
            case '0':
            saveGameStateToFile(saveFile, playerFloor, playerRoom, energy, time, score, inventory, inventoryCount, objectives, emergencyTripsLeft, roomEntities, entityCounts);
            cout << "Game saved. GoodBye!\n";
            return;
            
            case '1': {
                char floorChar = '0';
                char roomChar = '0';
                cout << "Target floor: ";
                cin >> floorChar;
                cout << "Target Room: ";
                cin >> roomChar;
                if(cin.fail()) break;
                handleMoveCommand(floorChar - '0', roomChar - '0', playerFloor, playerRoom, energy, time, horizontalConn, inventory, inventoryCount, emergencyTripsLeft, objectives[OBJ_LIFT2_ACTIVE]);
                break;
            }
            case '2':
                handleInspectCommand(playerFloor, playerRoom, energy, time, roomEntities, entityCounts[f][r], objectives, score);
                break;
            case '3': {
                char symbol = ' ';
                cout << "Item symbol to collect: ";
                cin >> symbol;
                if(cin.fail()) break;
                bool picked = handleCollectCommand(symbol, playerFloor, playerRoom, energy, time, score, roomEntities, entityCounts[f][r], inventory, inventoryCount, objectives);
                if(picked && symbol == ENT_EMERGENCY_CARD) emergencyTripsLeft = cardTrips;
                break;
            }

            case '4': {
                char symbol = ' ';
                cout << "Item symbol to use: ";
                cin >> symbol;
                if (cin.fail()) break;
                handleUseCommand(symbol, playerFloor, playerRoom, energy, time, score, inventory, inventoryCount, objectives);
                if(objectives[OBJ_PASSENGER_RESCUED]) {
                    // the rescued passenger leaves F4-R1
                    for(int i=0; i<entityCounts[3][0]; i++){
                        if(roomEntities[3][0][i] == ENT_PASSENGER) {
                            for(int j=i; j<entityCounts[3][0]-1; j++){
                                roomEntities[3][0][j] = roomEntities[3][0][j+1];
                            }
                            entityCounts[3][0]--;
                            break;
                        }
                    }
                }
                break;
            }

            case '5': {
                
            } 
            
        }
    }
}
