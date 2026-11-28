#include "switches.h"
#include "util.h"
#include "physics.h"

// Door lock mechanics, lifts, and cabinet verification without structures

// Lift 1 only for floor 1 and 2
bool canUseLift1(int fromFloor, int toFloor){
    if (fromFloor == toFloor) return false;
    return (fromFloor == 1 && toFloor == 2) || (fromFloor == 2 && toFloor == 1);
}

//lift 2 only for floor 2,4,5 and is to be repaired or emergency trip
bool canUseLift2(int fromFloor, int toFloor, bool lift2Operational, int emergencyTripsLeft){
    if (fromFloor == toFloor){ return false; }
    bool fromStop = fromFloor == 2 || fromFloor == 4 || fromFloor == 5;
    bool toStop = toFloor == 2 || toFloor == 4 || toFloor == 5;
    return fromStop && toStop && (lift2Operational || emergencyTripsLeft > 0);
}

//unlock floor 3 door with masterkey
bool unlockFloor3Door(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount){
    return hasItem(inventory, inventoryCount, ENT_MASTER_KEY);
}

//install fuse at control panel with fuse in inventory
bool installFuseControlPanel(char inventory[MAX_INVENTORY_CAPACITY], int &inventoryCount, bool objectives[NUM_OBJECTIVES], int &score){

    if (hasItem(inventory, inventoryCount, ENT_FUSE)){
        removeItem(inventory, inventoryCount, ENT_FUSE);
        objectives[OBJ_FUSE_INSTALLED] = true;
        applyScoreDelta(score, SCORE_INSTALL_FUSE);
        return true;
    }
    return false;
}

//verify cabinet code with clues found
bool verifyCabinetCode(const char* enteredCode, const bool objectives[NUM_OBJECTIVES]){
    if (objectives[OBJ_CLUE_4] && objectives[OBJ_CLUE_2] && objectives[OBJ_CLUE_7]){
        return stringEquals(enteredCode, "427");
    }
    return false;
}