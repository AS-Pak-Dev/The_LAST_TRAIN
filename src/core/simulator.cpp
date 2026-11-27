#include "simulator.h"
#include "physics.h"
#include "switches.h"
#include "logger.h"
#include "viewer_iface.h"
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