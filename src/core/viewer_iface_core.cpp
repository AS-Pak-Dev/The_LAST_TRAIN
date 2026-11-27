#include "viewer_iface.h"
#include "util.h"
#include "level.h"
#include<iostream>
using namespace std;

// Terminal UI and dashboard rendering using primitive parameters

//20 room names 
const char *roomNames[NUM_FLOORS][ROOMS_PER_FLOOR] = {
    {"Entrance Lobby", "Reception", "Waiting Area", "Utility Room"}
    {"Lounge", "Archive", "Lab", "Meeting Room"}
    {"Stairwell Annex", "Security Vestibule", "Animal Room", "Control Room"}
    {"First Aid Station", "Medical Room", "Maintenance Corridor", "Office"}
    {"Security Room", "Train Boarding Platform", "Storage Room", "Observation Deck"}
};

//link bw rooms
const char buildingLinks[NUM_FLOORS][ROOMS_PER_FLOOR - 1] = {
    {'W', '1', 'D'}
    {'D', '1', 'D'}
    {'L', 'W', 'D'}
    {'W', '#', 'D'}
    {'D', 'D', 'W'}
};

//checklist of tasks done
const char* objectiveNames[NUM_OBJECTIVES - 2] = {"Master Key Collected", "Clue digit 4 found", "Clue Digit 2 found", "Clue digit 7 found", "Cabinet Unlocked", "Train Key collected", "Fuse Collected", "Fuse installed", "Lift Pass collected", "Lift 2 active", "Passenger Rescued", "Cat carrier collected", "Cat secured", "Train prepared"};



