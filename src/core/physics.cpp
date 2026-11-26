#include "physics.h"
#include "util.h"
#include "switches.h"

// Movement validation and resource adjustments without structures

//check how rooms are connected and lift 2 usabel or no
bool isDirectlyConnected(int fromF, int fromR, int toF, int toR,const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],char& outConnType){
    outConnType = CONN_NONE;
    if (fromF == toF){
        if (toR == fromR + 1) outConnType = horizontalConn[fromF - 1][fromR - 1];
        else if (toR == fromR - 1) outConnType = horizontalConn[fromF - 1][toR - 1];
    }
    else if(fromR == 1 && toR == 1){
        int low = fromF;
        int high = toF;
        if(low > high){
            low = toF;
            high = fromF;
        }
        if(low == 1 && high == 2) outConnType = 'S';
        else if(low == 2 && high == 3) outConnType = 'S';
        else if(low == 4 && high == 5) outConnType = 'S';
    }
    else if((fromR == 2 || fromR == 3) && (toR == 2 || toR == 3) && canUseLift1(fromF, toF)) outConnType = '1';

    else if(fromR == 4 && toR == 4 && canUseLift2(fromF,toF,true,0)) outConnType = '2';

    return outConnType != CONN_NONE;
}

//can player pass and target exists, wall blocks and lock door needs key and lift 2 needs repair or emergency trip
bool canTraverse(int targetFloor, int targetRoom, char connType,const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,int emergencyTripsLeft,bool lift2Operational){
    if (targetFloor < 1 || targetFloor > NUM_FLOORS) return false;

    else if (targetRoom < 1 || targetRoom > ROOMS_PER_FLOOR) return false;
    
    else if (connType == CONN_NONE || connType == CONN_SOLID_WALL) return false;
    
    else if (connType == CONN_LOCKED_DOOR) return unlockFloor3Door(inventory, inventoryCount);
    
    else if (connType == CONN_LOCKED_DOOR) return unlockFloor3Door(inventory, inventoryCount);
    
    else if (connType == '2') return lift2Operational || emergencyTripsLeft > 0;
    return true;
}

//adds changes to energy and time, keeping them bw 0 and 100
void applyResourceDelta(int& energy, int& time, int deltaEnergy, int deltaTime){
    energy = clampInt(energy + deltaEnergy, 0, MAX_ENERGY);
    time = clampInt(time + deltaTime, 0, MAX_TIME);
}

//adds changes to score, keeping it bw 0 and 100
void applyScoreDelta(int& score, int deltaScore){
    score = score + deltaScore;
}

//Inventory loop
bool hasItem(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, char itemSymbol){
    for (int i = 0; i < inventoryCount; i++){
        if (inventory[i] == itemSymbol) return true;
    }
    return false;
}

// if inventory full refuse or put item in first empty slot
bool addItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol){
    if (inventoryCount >= MAX_INVENTORY_CAPACITY) return false;
    inventory[inventoryCount] = itemSymbol;
    inventoryCount += 1;
    return true;
}

//remove item from inventory and shift others left
bool removeItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol){
    for (int i = 0; i < inventoryCount; i++){
        if (inventory[i] == itemSymbol){
            for (int j = i; j < inventoryCount - 1; j++){
                inventory[j] = inventory[j + 1];
            }
            inventoryCount -= 1;
            return true;
        }
    }
    return false;
}

