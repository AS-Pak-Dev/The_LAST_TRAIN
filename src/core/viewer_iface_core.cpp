#include "viewer_iface.h"
#include "util.h"
#include "level.h"
#include<iostream>
using namespace std;

// Terminal UI and dashboard rendering using primitive parameters

//20 room names 
const char *roomNames[NUM_FLOORS][ROOMS_PER_FLOOR] = {
    {"Entrance Lobby", "Reception", "Waiting Area", "Utility Room"},
    {"Lounge", "Archive", "Lab", "Meeting Room"},
    {"Stairwell Annex", "Security Vestibule", "Animal Room", "Control Room"},
    {"First Aid Station", "Medical Room", "Maintenance Corridor", "Office"},
    {"Security Room", "Train Boarding Platform", "Storage Room", "Observation Deck"},
};

//link bw rooms
const char buildingLinks[NUM_FLOORS][ROOMS_PER_FLOOR - 1] = {
    {'W', '1', 'D'},
    {'D', '1', 'D'},
    {'L', 'W', 'D'},
    {'W', '#', 'D'},
    {'D', 'D', 'W'}
};

//checklist of tasks done
const char* objectiveNames[NUM_OBJECTIVES - 2] = {"Master Key Collected", "Clue digit 4 found", "Clue Digit 2 found", "Clue digit 7 found", "Cabinet Unlocked", "Train Key collected", "Fuse Collected", "Fuse installed", "Lift Pass collected", "Lift 2 active", "Passenger Rescued", "Cat carrier collected", "Cat secured", "Train prepared"};


//Print energy, time, score, inventory and room and emergency trip only shown when trips are left else not shown
void renderDashboard(int playerFloor, int playerRoom, int energy, int time, int score, const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, int emergencyTripsLeft) {
    cout << "----------------------------------------\n";
    cout << "Room: F" << playerFloor << "-R" << playerRoom;
    cout << " (" << roomNames[playerFloor - 1][playerRoom - 1] << ")\n";
    cout << "Energy: " << energy << "  Time: " << time << "  Score: " << score << "\n";
    cout << "Inventory: ";

    if (inventoryCount == 0) cout << "Empty";
    for (int i = 0; i < inventoryCount; ++i) {
            cout << inventory[i] << " ";
        }
    cout << "\n";
    if (emergencyTripsLeft > 0) {
        cout << "Emergency Trips Left: " << emergencyTripsLeft << "\n";
    }
}

void renderRoomDetails(int floor, int room, const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR], int entityCount){

    cout << "You search F" << floor << "-R" << room;
    cout << " (" << roomNames[floor - 1][room - 1] << "):\n";

    if (entityCount == 0) cout << "Nothing here.\n";
    
    for (int i = 0; i < entityCount; i++) {
        char symbol = roomEntities[floor - 1][room - 1];
        cout << " " << symbol << " = " << getEntityName(symbol) << "\n";
    }
}

// drawing the building with the top floor first to the bottom
void renderAsciiBuilding(int playerFloor, int playerRoom){
    cout << "\n";
    for(int f=NUM_FLOORS; f>=1; f++){
        cout << "F" << f << "  ";
        for(int r=1; r<=ROOMS_PER_FLOOR; r++){
            if(f == playerFloor && r == playerRoom) cout << "[ P ]";
            else cout << "[F" << f << "-R" << r << "]";
            if(r < ROOMS_PER_FLOOR) {
                char link = buildingLinks[f-1][r-1];
                if(link == '1') cout << "L1";
                else cout << " " << link << " ";
            }
        }
        cout << endl;
    }
    cout << "P = YOU  D = door  W=window  L=locked door  #=wall  L1=lift  1" << endl;
    cout << "Stairs (room 1): F1-F2, F2-F3, F4-F5.  Lift 1 (rooms 2-3).  Lift 2 (room 4). "<< endl;
}

// printing the checklist
void renderObjectivesChecklist(const bool objectives[NUM_OBJECTIVES]){
    cout << endl << "OBJECTIVES: " << endl;
    for(int i=0; i<NUM_OBJECTIVES-2; i++){
        cout << (objectives[i] ? "[X] " : "[ ] ");
        cout << objectiveNames[i] << endl;
    }
}
